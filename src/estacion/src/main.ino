//Librerias en uso:
  #include <Arduino.h>
  #include <com.hpp>

void setup(){
  //Comunicacion
  ConfigurarComunicacion();
  //Sensor gestos
  //ConfigurarGestos();
}

void loop() {
  Serial.println("Me estoy esperando a recibir un mensaje");
  delay(5000);
}
