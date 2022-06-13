//Librerias en uso:
#include <SPI.h>
#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>
//Variables referentes a la comunicación
const int IRQ = 8, CE = 9, CSN = 10;             //Declaramos en variables constantes los pines de CE y CSN
const uint64_t canal = 0xE8E8F0F0E1LL;  //Canal de comunicación
RF24 RF (CE, CSN);                      //Declaramos la variable del canal, como en los servos de arduino
//Variables de código
int t1 = 5000;
union UnionMsg {
  uint32_t p;
  uint8_t v[4];
} msg;

void setup() {
  Serial.begin(115200);               //Iniciamos el buffer de comunicacion serie a 9600 baudios
  ConfigurarComuniacion();
}

void loop() {
  EnviarRF(0x112233DD);
  delay(t1);
}
void ConfigurarComuniacion() {
  msg.p = 0x0;                    //Mensaje que enviaremos para las pruebas
  RF.begin();                     //Iniciamos la comunicación RF
  RF.openWritingPipe(canal);      //Iniciamos la escritura por el canal RF
  RF.openReadingPipe(1, canal);   //Abrimos la escucha por el canal RF
  RF.startListening();            //Empezamos a escuchar
  RF.maskIRQ(1, 1, 0);            //Solo permitimos que se active el pin de interrupcion cuando recibe datos
  attachInterrupt(digitalPinToInterrupt(IRQ), InterrupcionMensaje, FALLING);  //Interrupir el codigo cuando se active el pin 5 por una bajada de la señal
}
void EnviarRF(uint32_t m) {         //Función que enviara el mensaje que le cargemos
  RF.stopListening();     //Paramos de escuchar el canal
  RF.write(&m, sizeof(m)); //Escribimos en el canal de RF el valor introducido en la función
  MostrarMensaje(m, 0); //Mostramos el mensaje (Solo para pruebas)
  RF.startListening();    //Iniciamos la escucha por el canal
}
void InterrupcionMensaje() {        //Función que se activará cuando hagas una interrupción, por el pin IRQ
  msg.p = LeerRF();                 //Damos a msg.p el valor que leamos por el canal RF
  MostrarMensaje(msg.p, 1);         //Mostramos el mensaje (Solo para pruebas)
}
uint32_t LeerRF() {         //Función que devolvera el valor del mensaje leido por el canal RF
  uint32_t m = 0;           //Generamos una variable que luego devolveremos
  if (RF.available()) {     //Si existe algo en el buffer del canal:
    RF.read(&m, sizeof(m)); //Leemos el mensaje y lo guardamos en la variable antes creada
  }
  return m;                 //Devolvemos el valor leido
}
void MostrarMensaje(uint32_t mensaje, int n) { //Función que muestra por el puerto serie el mensaje recibido
  UnionMsg m;       //Declaramos una nueva union con la varibale m
  m.p = mensaje;  //Cargamos el valor en la variable union
  if (m.p != 0) { //Si el mensaje es diferente a 0(para evitar lecturas falsas)
    if (n == 0) Serial.print("Mensaje enviado: ");  //Si n es 0 decimos mensaje enviado
    if (n == 1) Serial.print("Mensaje recibido: "); //Si n es 1 decimos mensaje recibido
    for (int i = 3; i >= 0; i--) {
      Serial.print(" ");  //Mostramos por parejas todo el valor en hexadecimal del mensaje
      Serial.print(m.v[i], HEX);
    }
    Serial.println(""); //Hacemos un salto de linea
  }
}
