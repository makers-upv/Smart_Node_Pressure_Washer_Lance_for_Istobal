#include "tftLibrary.hpp"
#include "Melopero_APDS9960.h"


Melopero_APDS9960 device_Gestos;


enum estados{SOPORTE, STANDBY, LAVADO};
enum eventos{CARGANDO,NO_CARGANDO,SALDO,START,NEXT,BACK,END};

struct maquina_estados_struct
{
estados estado;         //Estado actual en el que nos encontramos
eventos evento;         //Evento que acaba de suceder
bool flag_evento;    //Flag de que hay un evento que no se ha procesado
};

maquina_estados_struct Maquina_Estados = {SOPORTE,CARGANDO,false}; //Estructura que nos almacenará las variables de la maquina de estado.


bool interruptOccurred_Gestos = false;
//This is the pin that will listen for the hardware interrupt.
const byte interruptPin_Gestos = 34;
const byte interruptPin_Cargando = 35;
const byte interruptPin_Gesture_but = 32;

void Proceso_Maquina(maquina_estados_struct *Maquina_Estados_puntero);

void interruptHandler_Gestos() {
  interruptOccurred_Gestos = true;
}
void interruptHandler_Cargando() {
  
  Maquina_Estados.evento = NO_CARGANDO;
  Maquina_Estados.flag_evento = true;

}
void interruptHandler_Gesture_but() {
  Maquina_Estados.evento = START;
  Maquina_Estados.flag_evento = true;
}

Screen screen;




void setup() {
  Serial.begin(115200);
  while (!Serial); // wait for serial to be ready

  Serial.begin(115200);
  screen.begin();
  delay(1000);
  screen.nextStartBMP();
  delay(1000);
  screen.nextStartBMP();
  delay(1000);


  screen.setTemplate(1);

  screen.setModeHeader(18, WHITE, TEMPLATERED);
  screen.setMode(62, 80, WHITE, BLACK);
  screen.setPrice(88, 113, WHITE, BLACK);
  screen.setTimer(29, 113, WHITE, BLACK);

  int8_t status = NO_ERROR;

  Wire.begin();
  status = device_Gestos.initI2C(0x39, Wire); // Initialize the comunication library
  if (status != NO_ERROR) {
    Serial.println("Error during initialization");
    while (true);
  }
  status = device_Gestos.reset(); // Reset all interrupt settings and power off the device
  if (status != NO_ERROR) {
    Serial.println("Error during reset.");
    while (true);
  }

  Serial.println("device_Gestos initialized correctly!");

  device_Gestos.setLedBoost(LED_BOOST_300);

  // Gesture engine settings
  device_Gestos.enableGesturesEngine(); // enable the gesture engine
  device_Gestos.setGestureProxEnterThreshold(50); // Enter the gesture engine only when the proximity value
  // is greater than this value proximity value ranges between 0 and 255 where 0 is far away and 255 is very near.
  device_Gestos.setGestureExitThreshold(0); // Exit the gesture engine only when the proximity value is less
  // than this value.
  device_Gestos.setGestureExitPersistence(EXIT_AFTER_4_GESTURE_END); // Exit the gesture engine only when 4
  // consecutive gesture end signals are fired (distance is greater than the threshold)

  // Gesture engine interrupt settings
  device_Gestos.enableGestureInterrupts();
  device_Gestos.setGestureFifoThreshold(FIFO_INT_AFTER_16_DATASETS); // trigger an interrupt as soon as there are 16 datasets in the fifo
  // To clear the interrupt pin we have to read all datasets that are available in the fifo.
  // Since it takes a little bit of time to read alla these datasets the device may collect
  // new ones in the meantime and prevent us from clearing the interrupt ( since the fifo
  // would not be empty ). To prevent this behaviour we tell the device to enter the sleep
  // state after an interrupt occurred. The device will exit the sleep state when the interrupt
  // is cleared.
  device_Gestos.setSleepAfterInterrupt(true);

  //Next we want to setup our interruptPin to detect the interrupt and to call our
  //interruptHandler_Gestos function each time an interrupt is triggered.
  pinMode(interruptPin_Gestos, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin_Gestos), interruptHandler_Gestos, FALLING);

  device_Gestos.wakeUp(); // wake up the device
///// Config Interrup Buttons ////////
/// Cargador  ///
  pinMode(interruptPin_Cargando, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin_Cargando), interruptHandler_Cargando, FALLING);
/// Gestos  ///
  pinMode(interruptPin_Gesture_but, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin_Gesture_but), interruptHandler_Gesture_but, FALLING);

}

void loop() {

  if(Maquina_Estados.flag_evento == true){
    Maquina_Estados.flag_evento = false;
    Proceso_Maquina(&Maquina_Estados);
  }

/*
  
  for (int i = 1; i < 1000; i++) {
    screen.updateScreen();
    screen.changeModeTo(i % 5 + 1);

    if (interruptOccurred_Gestos) {
      // clear interrupt
      interruptOccurred_Gestos = false;

      // The interrupt is cleared by reading all available datasets in the fifo
      // When an interrupt occurs we know we have 16 datasets in the fifo, by analyzing
      // these datasets we can detect a gesture.

      // Reads the gesture data currently available in the fifo and tries to interpret a gesture.
      // The device tries to detect a gesture by comparing the gesture data values through time.
      // The device compares the up data with the down data to detect a gesture on the up-down axis and
      // it compares the left data with the right data to detect a gesture on the left right axis.
      //
      // ADVANCED SETTINGS:
      // device.parseGestureInFifo(uint8_t tolerance = 12, uint8_t der_tolerance = 6, uint8_t confidence = 6);
      //
      // The tolerance parameter determines how much the two values (on the same axis) have to differ to interpret
      // the current dataset as valid for gesture detection (if the values are nearly the same then its not possible to decide the direction
      // in which the object is moving).
      //
      // The der_tolerance does the same for the derivative of the two curves (the values on one axis through time):
      // this prevents the device from detecting a gesture if the objects surface is not even...
      //
      // The confidence tells us the minimum amount of "detected gesture samples" needed for an axis to tell that a gesture has been detected on that axis:
      // How its used in the source code: if (detected_up_gesture_samples > detected_down_gesture_samples + confidence) gesture_up_down = GESTURE_UP
      device_Gestos.parseGestureInFifo();

      if (device_Gestos.parsedUpDownGesture != NO_GESTURE || device_Gestos.parsedLeftRightGesture != NO_GESTURE)
        Serial.print("Gesture : ");

      if (device_Gestos.parsedUpDownGesture == UP_GESTURE)
        Serial.print("UP ");
      else if (device_Gestos.parsedUpDownGesture == DOWN_GESTURE)
        Serial.print("DOWN ");

      if (device_Gestos.parsedLeftRightGesture == LEFT_GESTURE)
        Serial.print("LEFT ");
      else if (device_Gestos.parsedLeftRightGesture == RIGHT_GESTURE)
        Serial.print("RIGHT ");

      if (device_Gestos.parsedUpDownGesture != NO_GESTURE || device_Gestos.parsedLeftRightGesture != NO_GESTURE)
        Serial.println();
    }

    delay(1000);
  }

  */
}



void Proceso_Maquina(maquina_estados_struct* Maquina_Estados_puntero){
  switch (Maquina_Estados_puntero->estado) {
    case SOPORTE:
        switch (Maquina_Estados_puntero->evento) 
        { 
            case NO_CARGANDO:
             Serial.print("pues va i todo_NO_CARGANDO ");
            break;


            case START:
             Serial.print("pues va i todo_START ");
            break;
            default:

            break;

        }
    break;

    case STANDBY:
            switch (Maquina_Estados_puntero->evento) 
        { 
            case CARGANDO:

            break;

            case SALDO:

            break;

            case START:

            break;

            default:

            break;

        }

    break;

    case LAVADO:
            switch (Maquina_Estados_puntero->evento) 
        { 

            case BACK:

            break;

            case NEXT:

            break;

            default:

            break;

        }


    break;

    default:

    break;

  }



}
