//LA LECTRUA APARECE EN EL MONITRO SERIE 

#pragma once
// Pins
#define APDS9960_INT    2 // Needs to be an interrupt pin



//Rutina de la interrupción
void interruptRoutine();


//Función para inicializar sensor

void inicializar_sensor();


//lectura del sensor
void handleGesture();

// fucnión para acrivar flag
void flag_sensor();