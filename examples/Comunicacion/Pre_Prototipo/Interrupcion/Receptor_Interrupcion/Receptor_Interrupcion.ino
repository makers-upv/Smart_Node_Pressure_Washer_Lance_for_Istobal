//Librerias en uso:
  #include <SPI.h>
  #include <nRF24L01.h>
  #include <printf.h>
  #include <RF24.h>
  #include <RF24_config.h>
//Variables referentes a la comunicación
  const int IRQ = 2, CE = 9, CSN = 10;             //Declaramos en variables constantes los pines de CE y CSN 
  const uint64_t canal = 0xE8E8F0F0E1LL;  //Canal de comunicación
  RF24 RF (CE,CSN);                       //Declaramos la variable del canal, como en los servos de arduino
//Variables de código
int t1=5000;
union UnionMsg{
  uint32_t p;
  uint8_t v[4];
} msg;

void setup() {
  Serial.begin(9600);               //Iniciamos el buffer de comunicacion serie a 9600 baudios
  RF.begin();                       //Iniciamos la comunicación RF
  RF.openReadingPipe(1, canal);  //Abrimos la escucha por el canal RF
  RF.startListening();           //Empezamos a escuchar
  RF.maskIRQ(1,1,0);              //Solo permitimos que se active el pin de interrupcion cuando recibe datos
  attachInterrupt(digitalPinToInterrupt(IRQ), InterrupcionMensaje, FALLING);
}

void loop() {
  Serial.println("Me estoy esperando hasta recibir un mensaje");
  delay(t1);
}

void EnviarRF(uint32_t m){
  RF.write(&m,sizeof(m)); //Escribimos en el canal de RF el valor introducido en la función
}

void InterrupcionMensaje(){
  //Serial.println("HOLA SOY UAN INTERRUPCION");
  msg.p = LeerRF();
  MostrarMensaje(msg.p);
}

uint32_t LeerRF(){
  uint32_t m=0;
  if(RF.available()){
    RF.read(&m, sizeof(m));
  }
  return m;
}

void MostrarMensaje(uint32_t msg){
  UnionMsg m;
  if (msg!=0){
    m.p = msg;
    Serial.print("Mensaje recibido: ");
    for(int i=3;i>=0;i--){Serial.print(m.v[i], HEX);}
    Serial.println("");
  }
}
