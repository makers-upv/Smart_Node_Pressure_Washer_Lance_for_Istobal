#include "libreria_gestos.h" // Sensor ADPS9660

/* PINOUT */
// Sensor ADPS9660
// I2C + INTERRUPT PIN
// INTERRUPT PIN 22

void setup() {
  Serial.begin(115200); // Inicializamos el puerto serie
  while (!Serial); // Leonardo: wait for serial monitor
  inicializar_sensor(); //llama a la función que inicializa el sensor
}

void loop() {
  handleGesture();
  delay(10);
  Serial.print(".");
}
