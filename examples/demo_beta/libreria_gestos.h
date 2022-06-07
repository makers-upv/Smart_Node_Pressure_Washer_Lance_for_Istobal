//LA LECTURA APARECE EN EL MONITRO SERIE

#pragma once

// Pins
#define APDS9960_INT 34 // Tiene que ser un pin de interrupción

// Rutina de la interrupción
void interruptRoutine();

// Función para inicializar sensor
void inicializar_sensor();

// Lectura del sensor
int handleGesture();

// Función para activar el flag
void flag_sensor();
