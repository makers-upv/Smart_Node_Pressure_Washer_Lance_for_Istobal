//LA LECTURA APARECE EN EL MONITRO SERIE 

#include <Wire.h>
#include <SparkFun_APDS9960.h>
#include "libreria_gestos.h"

// Constants
#define VERBOSE

// Global Variables
SparkFun_APDS9960 apds = SparkFun_APDS9960();
int isr_flag = 0;

// Rutina de la interrupción
void interruptRoutine() {
  isr_flag = 1;
}

// Función para inicializar sensor
void inicializar_sensor(){

  // Set interrupt pin as input
  pinMode(APDS9960_INT, INPUT);

  Serial.print(".........");

#if defined(VERBOSE)
  // Initialize Serial port
  Serial.println(F("[SENS][APDS-9960] SparkFun APDS-9960 - GestureTest"));
#endif

  // Initialize interrupt service routine
  attachInterrupt(digitalPinToInterrupt(APDS9960_INT), interruptRoutine, FALLING);

  // Initialize APDS-9960 (configure I2C and initial values)
  if ( apds.init() ) {
#if defined(VERBOSE)
    Serial.println(F("[SENS][APDS-9960] APDS-9960 initialization complete"));
#endif
  } else {
#if defined(VERBOSE)
    Serial.println(F(" [SENS][APDS-9960] Something went wrong during APDS-9960 init!"));
#endif
  }
  
  // Start running the APDS-9960 gesture sensor engine
  if ( apds.enableGestureSensor(true) ) {
#if defined(VERBOSE)
    Serial.println(F("[SENS][APDS-9960] Gesture sensor is now running"));
#endif
  } else {
#if defined(VERBOSE)
    Serial.println(F("[SENS][APDS-9960] Something went wrong during gesture sensor init!"));
#endif
  }

  //apds.setLEDBoost(3); // No es necesario, tiene este valor por defecto
}


// Lectura del sensor
int handleGesture() {
    if ( apds.isGestureAvailable() ) {
#if defined(VERBOSE)
      int ret = apds.readGesture();
      switch (ret) {
        case DIR_UP:
          Serial.println("[SENS][APDS-9960] UP");
          break;
        case DIR_DOWN:
          Serial.println("[SENS][APDS-9960] DOWN");
          break;
        case DIR_LEFT:
          Serial.println("[SENS][APDS-9960] LEFT");
          break;
        case DIR_RIGHT:
          Serial.println("[SENS][APDS-9960] RIGHT");
          break;
        case DIR_NEAR:
          Serial.println("[SENS][APDS-9960] NEAR");
          break;
        case DIR_FAR:
          Serial.println("[SENS][APDS-9960] FAR");
          break;
        default:
          Serial.println("[SENS][APDS-9960] NONE");
      }
      return ret;
#else
    return apds.readGesture();
#endif
  }
  return -1;
}

// Función para activar el flag
void flag_sensor(){
  if( isr_flag == 1 ) {
    detachInterrupt(digitalPinToInterrupt(APDS9960_INT));
    //noInterrupts();
    handleGesture();
    isr_flag = 0;
    attachInterrupt(digitalPinToInterrupt(APDS9960_INT), interruptRoutine, FALLING);
    //interrupts();
  }
}
