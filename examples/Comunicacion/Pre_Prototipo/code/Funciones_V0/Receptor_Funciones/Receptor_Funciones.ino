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
int t1=50;
union VectorMensaje{
  uint32_t p;
  uint8_t v[4];
} msg;

void setup() {
  Serial.begin(9600);               //Iniciamos el buffer de comunicacion serie a 9600 baudios
  RF.begin();                       //Iniciamos la comunicación RF
  RF.openReadingPipe(1, canal);  //Abrimos la escucha por el canal RF
  RF.startListening();           //Empezamos a escuchar
}

void loop() {
  msg.p = LeerRF();
  MostrarMensaje(msg.p);
  delay(t1);
}

uint32_t LeerRF(){
  uint32_t m=0;
  if(RF.available()){
    RF.read(&m, sizeof(m));
  }
  return m;
}

void MostrarMensaje(uint32_t msg){
  union UnionFuncion{
    uint32_t p;
    uint8_t v[4];
  } m;
  if (msg!=0){
    m.p = msg;
    Serial.print("Mensaje enviado: ");
    for(int i=3;i>=0;i--){Serial.print(m.v[i], HEX);}
    Serial.println("");
  }
}
