//Librerias en uso:
#include <ComunicacionRF.hpp>
//Variables de código
#define IRQ 3

  int t1 = 1000;
  Comunicacion RFCom;
  
void setup() {
  Serial.begin(115200);             //Iniciamos el buffer de comunicacion serie a 9600 baudios
  while(!Serial);
  RFCom.ConfigurarComunicacion();
  attachInterrupt(digitalPinToInterrupt(IRQ), InterrupcionMensaje, FALLING);
}

void loop() {
  RFCom.Estado(TestNodo, 1);
//  RFCom.CambiarModoLimpieza(TestNodo, 1);
//  RFCom.CodigoError(TestNodo, 0x1);
  delay(t1);
//  RFCom.MostrarMensaje(RFCom.msg.p, 1);         //Mostramos el mensaje (Solo para pruebas)
//  Serial.println("--------------------------------");
//  delay(t1);
}

void InterrupcionMensaje() {        //Función que se activará cuando hagas una interrupción, por el pin IRQ
  RFCom.msg.p = RFCom.LeerRF(TestNodo);                 //Damos a msg.p el valor que leamos por el canal RF
  RFCom.MostrarMensaje(RFCom.msg.p, 1);         //Mostramos el mensaje (Solo para pruebas)
}
