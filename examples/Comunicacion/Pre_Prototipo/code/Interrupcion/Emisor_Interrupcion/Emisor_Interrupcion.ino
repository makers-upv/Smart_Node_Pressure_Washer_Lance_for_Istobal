//Librerias en uso:
  #include <SPI.h>
  #include <nRF24L01.h>
  #include <printf.h>
  #include <RF24.h>
  #include <RF24_config.h>
//Variables referentes a la comunicación
  const int CE = 9, CSN = 10;             //Declaramos en variables constantes los pines de CE y CSN 
  const uint64_t canal = 0xE8E8F0F0E1LL;  //Canal de comunicación
  RF24 RF (CE,CSN);                       //Declaramos la variable del canal, como en los servos de arduino
//Variables de código
int t1=1000, SWT = 5;                        //Variables del estado/mensaje a enviar y la otra para el tiempo del blink

union UnionMsg{
  uint32_t p;
  uint8_t v[4];
} msg;

void setup() {
  msg.p = 0xFDCACACB;  //Mensaje que enviaremos para las pruebas
  Serial.begin(9600);         //Iniciamos el buffer de comunicacion serie a 9600 baudios
  RF.begin();                 //Iniciamos la comunicación RF
  RF.openWritingPipe(canal);  //Iniciamos la escritura por el canal RF
  pinMode(SWT, INPUT_PULLUP);
}

void loop() {
    EnviarRF(msg.p);
    MostrarMensaje(msg.p);
  delay(t1);
}

void EnviarRF(uint32_t m){
  RF.write(&m,sizeof(m)); //Escribimos en el canal de RF el valor introducido en la función
}

void MostrarMensaje(uint32_t msg){
  UnionMsg m;
  if (msg!=0){
    m.p = msg;
    Serial.print("Mensaje enviado: ");
    for(int i=3;i>=0;i--){Serial.print(m.v[i], HEX);}
    Serial.println("");
  }
}
