//Librerias en uso:
  #include <Arduino.h>          //Libreria de arduino
  #include <Arduino_APDS9960.h> //Libreria del sensor de gestos
  //Nuestra libreria:
  #include <gestos.hpp>
  
#pragma region Funciones_de_gestos
void ConfigurarGestos(){
  if (!APDS.begin()) {
    Serial.println("Error initializing APDS-9960 sensor!");
  }
  APDS.setGestureSensitivity(80);
  Serial.println("Detecting gestures ...");
  APDS.setLEDBoost(3);
}
void gesto(){
  if (APDS.gestureAvailable()) {
    // a gesture was detected, read and print to Serial Monitor
    int gesture = APDS.readGesture();
    switch (gesture) {
      case GESTURE_UP:
        Serial.println("UP");
        //SubirCuenta();
        break;
      case GESTURE_DOWN:
        Serial.println("DOWN");
        //BajarCuenta();
        break;
      case GESTURE_LEFT:
        Serial.println("LEFT");
        break;
      case GESTURE_RIGHT:
        Serial.println("RIGHT");
      default:
        break;
    }
  }
}
#pragma endregion