//Librerias en uso:
  #include <com.hpp>
  #include <gestos.hpp>

void setup(){
  //Comunicacion
  ConfigurarComunicacion();
  //Sensor gestos
  //ConfigurarGestos();
}

void loop() {
  EnviarRF(0x12345678);        //Si la variable de mensaje sigue activa volvemos a enviar el mensaje por si se ha perdido
}
