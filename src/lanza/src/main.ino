//Librerias en uso:
  #include <Arduino.h>
  #include <com.hpp>
  #include <gestos.hpp>

void setup(){
  //Comunicacion
  ConfigurarComunicacion();
  //Sensor gestos
  //ConfigurarGestos();
}

void loop() {
  EnviarRF(0x123456DD);        //Si la variable de mensaje sigue activa volvemos a enviar el mensaje por si se ha perdido
  delay(5000);
}
