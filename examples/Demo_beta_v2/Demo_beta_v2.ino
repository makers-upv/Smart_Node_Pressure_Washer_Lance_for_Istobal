
#define USE_TIMER_2 true
#include <megaAVR_TimerInterrupt.h>
#include <megaAVR_TimerInterrupt.hpp>
#define TIMER2 1           //ATENCION: Frecuencia, no segundos, no milsiegundos
#include "tftLibrary.hpp"
//Lirberias gestos
#include <Wire.h>
#include <SparkFun_APDS9960.h>
#define APDS9960_INT    3
SparkFun_APDS9960 apds = SparkFun_APDS9960();

enum estados {SOPORTE, STANDBY, LAVADO};
enum eventos {CARGANDO, NO_CARGANDO, SALDO, START, UP, DOWN, END, TIEMPO, GESTO, GESTOERROR};

struct maquina_estados_struct {
  estados estado;         //Estado actual en el que nos encontramos
  eventos evento;         //Evento que acaba de suceder
  bool flag_evento;    //Flag de que hay un evento que no se ha procesado
};

maquina_estados_struct Maquina_Estados = {SOPORTE, CARGANDO, false}; //Estructura que nos almacenará las variables de la maquina de estado.

unsigned long tnext;
unsigned long tprint = 500;

bool interruptOccurred_Gestos = false;
//This is the pin that will listen for the hardware interrupt.
const byte interruptPin_Gestos = 3;
const byte interruptPin_Cargando = A3;
const byte interruptPin_Moneda = A2;

void Proceso_Maquina(maquina_estados_struct *Maquina_Estados_puntero);
//Variables generales
int ModoTrabajo = 1, CounterTime = 0;
#define TIEMPO_LAVADO 60
void interruptHandler_Gestos() {
  interruptOccurred_Gestos = true;
}

void interruptHandler_Cargando() {
  if (digitalRead(interruptPin_Cargando) == HIGH) Maquina_Estados.evento = NO_CARGANDO;
  else Maquina_Estados.evento = CARGANDO;
  Maquina_Estados.flag_evento = true;
}

void interruptHandler_Gesture() {
  //LeerGesto();
  Maquina_Estados.flag_evento = true;
}

void interruptHandler_Timer2() {
  Maquina_Estados.evento = TIEMPO;    
  Maquina_Estados.flag_evento = true;
}

void interruptHandler_Moneda() {
  Maquina_Estados.evento = START;
  Maquina_Estados.flag_evento = true;
}

Screen screen;

void setup() {
  Serial.begin(115200);
  while (!Serial); // wait for serial to be ready
  //Pulsadores de simulacion
  pinMode(interruptPin_Cargando, INPUT_PULLUP);
  pinMode(interruptPin_Moneda, INPUT_PULLUP);
  attachInterrupt(interruptPin_Cargando, interruptHandler_Cargando, CHANGE);
  attachInterrupt(interruptPin_Moneda, interruptHandler_Moneda, FALLING);
  //Temporizador
  ITimer2.init();
  ITimer2.pauseTimer();
  if (ITimer2.attachInterrupt(TIMER2, interruptHandler_Timer2)) Serial.println("Inciado el temporizador de puta madre");  //ATENCION: Frecuencia, no segundos, no milsiegundos
  else Serial.println("Ha habido un error con el timer");
  screen.begin();
  //Gestos
  pinMode(APDS9960_INT, INPUT);
  attachInterrupt(APDS9960_INT, interruptHandler_Gesture, FALLING);
  if ( apds.init() ) Serial.println(F("APDS-9960 initialization complete"));
  else     Serial.println(F("Something went wrong during APDS-9960 init!"));
  if ( apds.enableGestureSensor(true) )    Serial.println(F("Gesture sensor is now running"));
  else    Serial.println(F("Something went wrong during gesture sensor init!"));

  tnext = millis() + tprint;
}

void loop() {
  //Maquina de estados
  if (Maquina_Estados.flag_evento == true) {
    Maquina_Estados.flag_evento = false;
    //if (Maquina_Estados.estado == LAVADO && Maquina_Estados.evento == GESTO) 
    Proceso_Maquina(&Maquina_Estados);
  }

  if (millis() > tnext) {
    //Serial.println(".");
    Serial.println(Maquina_Estados.estado);
    tnext += tprint;
  }
}

void LeerGesto() {
  detachInterrupt(APDS9960_INT);
  if ( apds.isGestureAvailable() ) {
    
    switch ( apds.readGesture() ) {
      case DIR_UP:
        Maquina_Estados.evento = UP;
        break;
      case DIR_DOWN:
        Maquina_Estados.evento = DOWN;
        break;
      default:
        Maquina_Estados.evento = GESTOERROR;
    }
  }
  attachInterrupt(APDS9960_INT, interruptHandler_Gesture, FALLING);
}

void Proceso_Maquina(maquina_estados_struct * Maquina_Estados_puntero) {
  switch (Maquina_Estados_puntero->estado) {
    case SOPORTE:
      switch (Maquina_Estados_puntero->evento) {
        case NO_CARGANDO:
          Serial.println("pues va i todo_NO_CARGANDO ");
          delay(1000);
          screen.nextStartBMP();
          delay(1000);
          screen.nextStartBMP();
          delay(1000);
          Maquina_Estados.estado = STANDBY;
          break;
        default:
          break;
      }
      break;

    case STANDBY:
      switch (Maquina_Estados_puntero->evento)        {
        case CARGANDO:
          screen.begin();

          Maquina_Estados.estado = SOPORTE;
          break;

        case START:
        Serial.println("START");
          //Preparamos la pantalla
          screen.setTemplate(4);
          screen.setModeHeader(20, WHITE, BLACK);
          screen.setMode(62, 80, WHITE, BLACK);
          screen.setPrice(10, 119, TEMPLATERED, WHITE);
          screen.setTimer(75, 119, WHITE, TEMPLATERED);
          ITimer2.resumeTimer();  //Iniciamos el temporizador
          ModoTrabajo = 1;
          CounterTime = 0;
          screen.changeModeTo(ModoTrabajo);
          Maquina_Estados.estado = LAVADO;
          break;

        default:

          break;
      }

      break;

    case LAVADO:
      switch (Maquina_Estados_puntero->evento)
      {
        case TIEMPO:
          //Actualiza el tiempo y el dinero
          screen.updateScreen();
          CounterTime++;
          if (CounterTime >= TIEMPO_LAVADO){
            screen.nextStartBMP();
            ITimer2.pauseTimer();
            Maquina_Estados.estado = SOPORTE;
          }
          break;

        case UP:
          if(ModoTrabajo <= 4)  ModoTrabajo++;
          screen.changeModeTo(ModoTrabajo);
          break;

        case DOWN:
          if(ModoTrabajo >= 2)  ModoTrabajo--;
          screen.changeModeTo(ModoTrabajo);
          break;

        default:

          break;
      }
      break;
    default:
      break;
  }



}
