#define USE_TIMER_2 true
#include <megaAVR_TimerInterrupt.h>
#include <megaAVR_TimerInterrupt.hpp>
#define TIMER2 1           //ATENCION: Frecuencia, no segundos, no milsiegundos
#include "tftLibrary.hpp"

enum estados {SOPORTE, STANDBY, LAVADO};
enum eventos {CARGANDO, NO_CARGANDO, SALDO, START, NEXT, BACK, END, TIEMPO};

struct maquina_estados_struct {
  estados estado;         //Estado actual en el que nos encontramos
  eventos evento;         //Evento que acaba de suceder
  bool flag_evento;    //Flag de que hay un evento que no se ha procesado
};

maquina_estados_struct Maquina_Estados = {SOPORTE, CARGANDO, false}; //Estructura que nos almacenará las variables de la maquina de estado.


bool interruptOccurred_Gestos = false;
//This is the pin that will listen for the hardware interrupt.
const byte interruptPin_Gestos = 3;
const byte interruptPin_Cargando = A3;
const byte interruptPin_Moneda = A2;

void Proceso_Maquina(maquina_estados_struct *Maquina_Estados_puntero);

void interruptHandler_Gestos() {
  interruptOccurred_Gestos = true;
}

void interruptHandler_Cargando() {
  if (digitalRead(interruptPin_Cargando) == HIGH) Maquina_Estados.evento = NO_CARGANDO;
  else Maquina_Estados.evento = CARGANDO;
  Maquina_Estados.flag_evento = true;
}

void interruptHandler_Gesture_but() {
  Maquina_Estados.evento = START;
  Maquina_Estados.flag_evento = true;
}

void interruptHandler_Timer2() {
  Maquina_Estados.evento = TIEMPO;
  Maquina_Estados.flag_evento = true;
}

void interruptHandler_Moneda() {
  Maquina_Estados.evento = START;
  Maquina_Estados.flag_evento = true;
  Serial.println("Hola");
}

Screen screen;

void setup() {
  Serial.begin(115200);
  while (!Serial); // wait for serial to be ready
  // Suponemos que el estado inicial es cargando
  pinMode(interruptPin_Cargando, INPUT_PULLUP);
  attachInterrupt(interruptPin_Cargando, interruptHandler_Cargando, CHANGE);
  attachInterrupt(interruptPin_Moneda, interruptHandler_Moneda, FALLING);
  ITimer2.init();
  ITimer2.pauseTimer();
  if (ITimer2.attachInterrupt(TIMER2, interruptHandler_Timer2)) Serial.print("Inciado el temporizador de puta madre");  //ATENCION: Frecuencia, no segundos, no milsiegundos
  else Serial.println("Ha habido un error con el timer");
  screen.begin();
}

void loop() {
  if (Maquina_Estados.flag_evento == true) {
    Maquina_Estados.flag_evento = false;
    Proceso_Maquina(&Maquina_Estados);
  }
}



void Proceso_Maquina(maquina_estados_struct* Maquina_Estados_puntero) {
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
      switch (Maquina_Estados_puntero->evento)
      {
        case CARGANDO:
          screen.begin();
          break;

        case TIEMPO:
          screen.updateScreen();
          break;

        case START:
          screen.setTemplate(4);
          screen.setModeHeader(20, WHITE, BLACK);
          screen.setMode(62, 80, WHITE, BLACK);
          screen.setPrice(10, 119, TEMPLATERED, WHITE);
          screen.setTimer(75, 119, WHITE, TEMPLATERED);
          ITimer2.resumeTimer();
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
