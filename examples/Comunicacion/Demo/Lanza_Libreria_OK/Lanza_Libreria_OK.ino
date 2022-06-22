//Librerias en uso:
#include <ComunicacionRF.hpp>
//Variables de código
#define IRQ 8
  int t1 = 5000;
  Comunicacion RFCom;
  
void setup() {
  Serial.begin(115200);             //Iniciamos el buffer de comunicacion serie a 9600 baudios
  RFCom.ConfigurarComunicacion();
  attachInterrupt(digitalPinToInterrupt(IRQ), InterrupcionMensaje, FALLING);  //Interrupir el codigo cuando se active el pin 5 por una bajada de la señal
}

void loop() {
  RFCom.EnviarRF(0x112233DD);
  delay(t1);
}

void InterrupcionMensaje() {        //Función que se activará cuando hagas una interrupción, por el pin IRQ
  RFCom.msg.p = RFCom.LeerRF();                 //Damos a msg.p el valor que leamos por el canal RF
  RFCom.MostrarMensaje(RFCom.msg.p, 1);         //Mostramos el mensaje (Solo para pruebas)
}
