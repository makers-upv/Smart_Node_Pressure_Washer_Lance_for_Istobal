#define DEBUG // Activar proceso de debugging

/* MAQUINA DE ESTADOS */
/* LA EJECUCIÓN SE PRODUCE CUANDO HAY UN EVENTO */
/* ENTRE CAMBIO DE ESTADOS */
enum estados {SOPORTE, STANDBY, WAIT_COM, LAVADO};
enum eventos {CARGANDO, NO_CARGANDO, BACK, NEXT, EXIT_, UPDATE_VIEW, TIMEOUT};

struct maquina_estados_struct {
  estados estado;     //Estado actual en el que nos encontramos
  eventos evento;     //Evento que acaba de suceder
  bool flag_evento;   //Flag de que hay un evento que no se ha procesado
};

maquina_estados_struct Maquina_Estados = {SOPORTE, CARGANDO, false}; //Estructura que nos almacenará las variables de la maquina de estado.

//#define EMULATE_CHARGE_BTN
#ifdef EMULATE_CHARGE_BTN
const byte interruptPin_Cargando = 17;
#endif

//#define EMULATE_GESTURES_BTN
#ifdef EMULATE_GESTURES_BTN
const byte interruptPin_Btn_BACK = 16;
const byte interruptPin_Btn_NEXT = 15;
const byte interruptPin_Btn_EXIT = 14;
#endif

/* TIMER */
#define USE_TIMER_2 true // TIMER EN USO - TIMER EN USO
#include <megaAVR_TimerInterrupt.h>
#include <megaAVR_TimerInterrupt.hpp>
#define TIMER2 1  // Frecuencia (Hz)

/* PANTALLA */
#include "tftLibrary.hpp"
Screen screen;
int ModoTrabajo = 1;
int counterTime = 0;
const int counterTimeOut = 29;
const byte pin_screen_OnOff = 2;

/* GESTOS */
#define GESTOS
#ifdef GESTOS
#include <Wire.h>
#include "paj7620.h"
uint8_t ges_data = 0, ges_error;
#define GES_REACTION_TIME 500       // You can adjust the reaction time according to the actual circumstance.
unsigned long tgesture = 0;
#endif

// CARGA
#define CHARGE
#ifdef CHARGE
#define CHARGE_REACTION_TIME 500       // You can adjust the reaction time according to the actual circumstance.
#define CHARGE_THRESHOLD 800 // Aprox ??V ->
const byte pin_Cargando = A7;
int carga_value = 0;  // variable to store the value coming from the sensor
unsigned long tcharge = 0;
#endif

/* DEFINICIÓN DE FUNCIONES */
void Proceso_Maquina(maquina_estados_struct *Maquina_Estados_puntero);
void interruptHandler_Btn_NEXT();
void interruptHandler_Btn_BACK();
void interruptHandler_Btn_EXIT();
void interruptHandler_Cargando();
void interruptHandler_Timer();
void LAVADO_ON();
void LAVADO_OFF();
void TO_CHARGE();
void screen_turnOff();
void screen_turnOn();

void resetMCU() {
  while (1);
}

void setup() {
#ifdef DEBUG
  Serial.begin(115200);
  while (!Serial); // wait for serial to be ready
  for (int i = 0; i < 50; i++) Serial.println(); // Clear screen
#endif

  /* Inicialización de los pulsadores de simulación de eventos */
#ifdef EMULATE_GESTURES_BTN
  pinMode(interruptPin_Btn_NEXT, INPUT_PULLUP);
  pinMode(interruptPin_Btn_BACK, INPUT_PULLUP);
  pinMode(interruptPin_Btn_EXIT, INPUT_PULLUP);
#endif
#ifdef EMULATE_CHARGE_BTN
  pinMode(interruptPin_Cargando, INPUT_PULLUP);
#endif

#ifdef EMULATE_GESTURES_BTN
  attachInterrupt(digitalPinToInterrupt(interruptPin_Btn_BACK), interruptHandler_Btn_BACK, RISING);
  attachInterrupt(digitalPinToInterrupt(interruptPin_Btn_NEXT), interruptHandler_Btn_NEXT, RISING);
  attachInterrupt(digitalPinToInterrupt(interruptPin_Btn_EXIT), interruptHandler_Btn_EXIT, RISING);
#endif
#ifdef EMULATE_CHARGE_BTN
  attachInterrupt(digitalPinToInterrupt(interruptPin_Cargando), interruptHandler_Cargando, RISING);
#endif

  /* Inicialización del Temporizador */
  ITimer2.init();
  ITimer2.pauseTimer();
#ifdef DEBUG
  if (ITimer2.attachInterrupt(TIMER2, interruptHandler_Timer))  Serial.println("Se ha iniciado el temporizador 2 (timer2).");
  else {
    Serial.println("Ha habido un error con el timer2.");
    resetMCU();
  }
#else
  if (!ITimer2.attachInterrupt(TIMER2, interruptHandler_Timer)) while (1);
#endif

#ifdef GESTOS
  /* Inicialización del sensor de gestos */
#ifdef DEBUG
  uint8_t error = 0;
  Serial.println("\nGESTURE SENSOR PAJ7620U2");

  ges_error = paj7620Init();      // initialize Paj7620 registers
  if (ges_error)
  {
    Serial.print("INIT ERROR,CODE:");
    Serial.println(ges_error);
    resetMCU();
  }
  else
  {
    Serial.println("INIT OK");
  }
#else
  if (paj7620Init()) resetMCU(); // initialize Paj7620 registers
#endif
#endif

#ifdef CHARGE
#ifdef DEBUG

#endif
#endif

  /* Inicialización de la pantalla */
  pinMode(pin_screen_OnOff, OUTPUT);
  screen_turnOff();
  screen.begin(); // TODO: ¿Puede haber error al inicializar la pantalla?


#ifdef DEBUG
  Serial.println("LANZA ENCENDIDA");
#endif

  // TODO: ¿Si se reinicia cuando esta funcionando?
  // Recuperación del estado
  // TODO: FUNCIÓN DE AUTOREINICIO CUANDO INICIALIZAMOS Y SE QUEDA COLGADA
}

void loop() {

  // Maquina de estados
  if (Maquina_Estados.flag_evento == true) {
    Maquina_Estados.flag_evento = false; // ¿Y si hay más de un evento? Hay colisión de eventos
    Proceso_Maquina(&Maquina_Estados);
  }

#ifdef GESTOS
  if (tgesture + GES_REACTION_TIME < millis()) {
    tgesture = millis();
#ifdef DEBUG
    ges_error = paj7620ReadReg(0x43, 1, &ges_data);       // Read Bank_0_Reg_0x43/0x44 for gesture result.
    if (!ges_error)
    {
      switch (ges_data)                   // When different gestures be detected, the variable 'data' will be set to different values by paj7620ReadReg(0x43, 1, &data).
      {
        case GES_RIGHT_FLAG:
          Serial.println("Right");
          interruptHandler_Btn_BACK();
          break;
        case GES_LEFT_FLAG:
          Serial.println("Left");
          interruptHandler_Btn_NEXT();
          break;
        case GES_UP_FLAG:
          Serial.println("Up");
          interruptHandler_Btn_EXIT();
          break;
        case GES_DOWN_FLAG:
          Serial.println("Down");
          break;
        case GES_CLOCKWISE_FLAG:
          Serial.println("Clockwise");
          break;
        case GES_COUNT_CLOCKWISE_FLAG:
          Serial.println("anti-clockwise");
          break;
        default:
          paj7620ReadReg(0x44, 1, &ges_data);
          if (ges_data == GES_WAVE_FLAG)
          {
            Serial.println("wave");
          }
          break;
      }
    }
#else
    ges_error = paj7620ReadReg(0x43, 1, &ges_data);       // Read Bank_0_Reg_0x43/0x44 for gesture result.
    if (!ges_error)
    {
      switch (ges_data)                   // When different gestures be detected, the variable 'data' will be set to different values by paj7620ReadReg(0x43, 1, &data).
      {
        case GES_RIGHT_FLAG:
          interruptHandler_Btn_BACK();
          break;
        case GES_LEFT_FLAG:
          interruptHandler_Btn_NEXT();
          break;
        case GES_UP_FLAG:
          interruptHandler_Btn_EXIT();
          break;
      }
    }
#endif
  }
#endif

#ifdef GESTOS
  if (tcharge + CHARGE_REACTION_TIME < millis()) {
    tcharge = millis();
#ifdef DEBUG
    carga_value = analogRead(pin_Cargando);
    Serial.print("Analog read charge pin: ");
    Serial.println(carga_value);
    
    if(Maquina_Estados.estado == SOPORTE) {
      if(carga_value < CHARGE_THRESHOLD) {
        Serial.println("EVENTO: NO CARGANDO");
        Maquina_Estados.evento = NO_CARGANDO;
        Maquina_Estados.flag_evento = true;       
      }
//      Serial.println("NO EVENTO");
    }
    else {
      if(carga_value > CHARGE_THRESHOLD) {
        Serial.println("EVENTO: CARGANDO");
        Maquina_Estados.evento = CARGANDO;
        Maquina_Estados.flag_evento = true;       
      }
//      Serial.println("NO EVENTO");      
    }
#else
    carga_value = analogRead(pin_Cargando);
    
    if(Maquina_Estados.estado == SOPORTE) {
      if(carga_value < CHARGE_THRESHOLD) {
        Maquina_Estados.evento = NO_CARGANDO;
        Maquina_Estados.flag_evento = true;       
      }
    }
    else {
      if(carga_value > CHARGE_THRESHOLD) {
        Maquina_Estados.evento = CARGANDO;
        Maquina_Estados.flag_evento = true;       
      }
    }  
#endif
  }
#endif


}

void Proceso_Maquina(maquina_estados_struct * Maquina_Estados_puntero) {
  switch (Maquina_Estados_puntero->estado) {
    case SOPORTE:
      switch (Maquina_Estados_puntero->evento) {
        case NO_CARGANDO:
#ifdef DEBUG
          Serial.println("SOPORTE -> NO_CARGANDO -> STANDBY");
#endif
          screen_turnOn();
          screen.redScreen();
          screen.logoIstobal();
          delay(1000);
          screen.welcome();
          delay(2000);
          screen.waveStart();
          delay(2000);
          Maquina_Estados.estado = STANDBY;
          break;
        default:
#ifdef DEBUG
          Serial.println("SOPORTE - NO TRANSITION");
#endif
          break;
      }
      break;
    case STANDBY:
      switch (Maquina_Estados_puntero->evento) {
        case CARGANDO:
#ifdef DEBUG
          Serial.println("STANDBY -> CARGANDO -> SOPORTE");
#endif
          TO_CHARGE();
          Maquina_Estados.estado = SOPORTE;
          break;
        case NEXT:
#ifdef DEBUG
          Serial.println("STANDBY -> NEXT -> LAVADO");
#endif
          Maquina_Estados.estado = LAVADO;
          LAVADO_ON();
          break;
        default:
#ifdef DEBUG
          Serial.println("STANDBY - NO TRANSITION");
#endif
          break;
      }
      break;
    case LAVADO:
      switch (Maquina_Estados_puntero->evento) {
        case CARGANDO:
          Maquina_Estados.estado = SOPORTE;
          LAVADO_OFF();
          TO_CHARGE();
#ifdef DEBUG
          Serial.println("LAVADO -> FINALIZADO -> CARGANDO -> SOPORTE");
#endif
          break;
        case TIMEOUT:
          Maquina_Estados.estado = STANDBY;
          LAVADO_OFF();
#ifdef DEBUG
          Serial.println("LAVADO -> FINALIZADO -> TIMEOUT -> STANDBY");
#endif
          break;
        case EXIT_:
          Maquina_Estados.estado = STANDBY;
          LAVADO_OFF();
#ifdef DEBUG
          Serial.println("LAVADO -> FINALIZADO -> EXIT -> STANDBY");
#endif
          break;
        case NEXT:
          if (ModoTrabajo < 5) {
            ModoTrabajo++;
            screen.changeModeTo(ModoTrabajo);
#ifdef DEBUG
            Serial.println("LAVADO -> INCREMENTANDO MODO");
            Serial.print("MODO DE LAVADO: ");
            Serial.println(ModoTrabajo);
#endif
          } else {
            // FINALIZANDO LAVADO
            Maquina_Estados.estado = STANDBY;
            LAVADO_OFF();
#ifdef DEBUG
            Serial.println("LAVADO -> FINALIZADO -> STANDBY");
#endif
          }

          break;
        case BACK:
          if (ModoTrabajo > 1) {
            ModoTrabajo--;
            screen.changeModeTo(ModoTrabajo);
#ifdef DEBUG
            Serial.println("LAVADO -> DECREMENTANDO MODO");
#endif
          }
#ifdef DEBUG
          Serial.print("MODO DE LAVADO: ");
          Serial.println(ModoTrabajo);
#endif
          break;
        case UPDATE_VIEW:
          screen.updateScreen(); //Actualiza el tiempo y el dinero
#ifdef DEBUG
          Serial.print(".");
#endif
          break;
        default:
#ifdef DEBUG
          Serial.println("LAVADO - NO TRANSITION");
#endif
          break;
      }
      break;
    default:
      break;
  }
}

void interruptHandler_Btn_NEXT() {
#ifdef DEBUG
  Serial.println("NEXT");
#endif
  Maquina_Estados.evento = NEXT;
  Maquina_Estados.flag_evento = true;
}

void interruptHandler_Btn_BACK() {
#ifdef DEBUG
  Serial.println("BACK");
#endif
  Maquina_Estados.evento = BACK;
  Maquina_Estados.flag_evento = true;
}

void interruptHandler_Btn_EXIT() {
#ifdef DEBUG
  Serial.println("EXIT");
#endif
  Maquina_Estados.evento = EXIT_;
  Maquina_Estados.flag_evento = true;
}

#ifdef EMULATE_CHARGE_BTN
void interruptHandler_Cargando() {
  if (Maquina_Estados.estado == SOPORTE) Maquina_Estados.evento = NO_CARGANDO;
  else Maquina_Estados.evento = CARGANDO;
  Maquina_Estados.flag_evento = true;
}
#endif

void interruptHandler_Timer() {
  if (counterTime < counterTimeOut) {
    counterTime++;
    Maquina_Estados.evento = UPDATE_VIEW;
  }
  else {
    Maquina_Estados.evento = TIMEOUT;
  }
  Maquina_Estados.flag_evento = true;
}

void LAVADO_ON() {
#ifdef DEBUG
  Serial.println("LAVADO ENMARCHA");
  Serial.print("MODO DE LAVADO: ");
  Serial.println(ModoTrabajo);
#endif
  //Preparamos la pantalla
  screen.setTemplate(4);
  screen.setModeHeader(20, WHITE, BLACK);
  screen.setMode(62, 80, WHITE, BLACK);
  screen.setPrice(10, 119, TEMPLATERED, WHITE);
  screen.setTimer(75, 119, WHITE, TEMPLATERED);
  screen.changeModeTo(ModoTrabajo);
  screen.clearTimeAndMoney();
  ITimer2.resumeTimer();  //Iniciamos el temporizador
  screen.updateScreen(); //Actualiza el tiempo y el dinero
}

void LAVADO_OFF() {
  ITimer2.pauseTimer();
  screen.changeModeTo(6);
  ModoTrabajo = 1;
  counterTime = 0;
  delay(6000); /* TODO: PANTALLA DE UNOS SEGUNDOS CON LA INFO DEL LAVADO */
  if (Maquina_Estados.estado != SOPORTE) {
    screen.redScreen();
    screen.logoIstobal();
    delay(1000);
    screen.welcome();
    delay(2000);
    screen.waveStart();
    delay(2000);
  }
#ifdef DEBUG
  Serial.println("FIN DEL LAVADO");
#endif
}

void TO_CHARGE() {
  screen.redScreen();
  screen.logoIstobal();
  delay(1000);
  screen.greyScreen();
  screen_turnOff();
}

void screen_turnOff() {
  digitalWrite(pin_screen_OnOff, LOW);
}

void screen_turnOn() {
  digitalWrite(pin_screen_OnOff, HIGH);
}
