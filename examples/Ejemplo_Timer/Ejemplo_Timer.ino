#define USE_TIMER_1 true
#include <megaAVR_TimerInterrupt.h>
#include <megaAVR_TimerInterrupt.hpp>

#define TIMER 1           //ATENCION: Frecuencia, no segundos, no milsiegundos

void TimerHandler(){
Serial.println("Hola");
}

void setup() {
  Serial.begin(115200);
  ITimer1.init();
  if (ITimer1.attachInterrupt(TIMER, TimerHandler)) Serial.println("Inciado el temporizador de puta madre");
  else Serial.println("Ha habido un error con el timer");
}

void loop() {
}
