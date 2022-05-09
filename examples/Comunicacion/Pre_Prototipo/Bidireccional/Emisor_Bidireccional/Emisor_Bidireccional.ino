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
int t1=50, SWT = 5;                        //Variables del estado/mensaje a enviar y la otra para el tiempo del blink

union UnionMsg{
  uint32_t p;
  uint8_t v[4];
} msg;

void setup() {
  msg.p = 0xFDCACACB;  //Mensaje que enviaremos para las pruebas
  Serial.begin(9600);         //Iniciamos el buffer de comunicacion serie a 9600 baudios
  RF.begin();                 //Iniciamos la comunicación RF
  RF.openWritingPipe(canal);  //Iniciamos la escritura por el canal RF
  RF.openReadingPipe(1, canal);  //Abrimos la escucha por el canal RF
  RF.startListening();           //Empezamos a escuchar
  pinMode(SWT, INPUT_PULLUP);
  RF.maskIRQ(1,1,0);              //Solo permitimos que se active el pin de interrupcion cuando recibe datos
  attachInterrupt(digitalPinToInterrupt(IRQ), InterrupcionMensaje, FALLING);  //Interrupir el codigo cuando se active el pin 5 por una bajada de la señal
}

void loop() {
  //Serial.println("Me estoy esperando hasta recibir un mensaje");
  delay(t1);
  if (digitalRead(SWT) == LOW)  EnviarRF(0xFBCACACD);
}

void EnviarRF(uint32_t m){
  RF.stopListening();
  RF.write(&m,sizeof(m)); //Escribimos en el canal de RF el valor introducido en la función
  //MostrarMensaje(msg.p, 0);
  RF.startListening();
}

void InterrupcionMensaje(){
  //Serial.println("HOLA SOY UAN INTERRUPCION");
  msg.p = LeerRF();
  MostrarMensaje(msg.p, 1);
  EnviarRF(0xFBCACACD);
}

uint32_t LeerRF(){
  uint32_t m=0;
  if(RF.available()){
    RF.read(&m, sizeof(m));
  }
  return m;
}

void MostrarMensaje(uint32_t msg, int n){
  UnionMsg m;
  if (msg!=0){
    m.p = msg;
    if (n == 0) Serial.print("Mensaje enviado: ");
    if (n == 1) Serial.print("Mensaje recibido: ");
    for(int i=3;i>=0;i--){Serial.print(m.v[i], HEX);}
    Serial.println("");
  }
}
