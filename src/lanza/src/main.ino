//Librerias en uso:
  #include <com.hpp>
  #include <gestos.hpp>

void setup(){
  //Comunicacion
  ConfigurarComunicacion();
  //Sensor gestos
  ConfigurarGestos();
}

void loop() {
  gesto();
  if (Seguridad == 1) EnviarRF(msg.p);        //Si la variable de mensaje sigue activa volvemos a enviar el mensaje por si se ha perdido
  delay(t1);                                  //Esperamos un pequeño tiempo
}
