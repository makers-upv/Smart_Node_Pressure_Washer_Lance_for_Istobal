//Librerias en uso:
  #include <Arduino.h>
  #include <SPI.h>
  #include <nRF24L01.h>
  #include <printf.h>
  #include <RF24.h>
  #include <RF24_config.h>
//Variables referentes a la comunicación
  const int IRQ = 2, CE = 9, CSN = 10;    //Declaramos en variables constantes los pines de CE, CSN y IRQ
  const uint64_t canal = 0xE8E8F0F0E1LL;  //Canal de comunicación
  const uint32_t msgOK = 0x01FF00DD;      //Variable equivalente a que un mensaje es correcto
  RF24 RF (CE,CSN);                       //Declaramos la variable del canal, como en los servos de arduino
//Variables de código
int t1=500, SWT = 7, L1 = 3, L2 = 4, L3 = 5;
/*
t1 es la variable que hace referencia al temportizador que usamos en el void loop
SWT es la variable que hace referencia al pin que usamos como entrada para las pruebas
L1, L2 y L3 hacen referencia a los 3 leds que tendremos en la demo
*/
uint32_t estado;    //Variable auxiliar para parchear

union UnionMsg{ //Union para poder ver unicamente cada una de las 4 parejas de valores hexadecimales que componen el mensaje que se envia
  uint32_t p;
  uint8_t v[4];
} msg;

void setup() {
  msg.p = 0x0;//0xFFCACADD        //Mensaje que enviaremos para las pruebas
  Serial.begin(9600);             //Iniciamos el buffer de comunicacion serie a 9600 baudios
  RF.begin();                     //Iniciamos la comunicación RF
  RF.openWritingPipe(canal);      //Iniciamos la escritura por el canal RF
  RF.openReadingPipe(1, canal);   //Abrimos la escucha por el canal RF
  RF.startListening();            //Empezamos a escuchar
  pinMode(SWT, INPUT_PULLUP);     //Declaramos el pin del SWT como entrada pullup
  RF.maskIRQ(1,1,0);              //Solo permitimos que se active el pin de interrupcion cuando recibe datos
  attachInterrupt(digitalPinToInterrupt(IRQ), InterrupcionMensaje, FALLING);  //Interrupir el codigo cuando se active el pin 5 por una bajada de la señal
  pinMode(L1, OUTPUT);  pinMode(L2, OUTPUT);  pinMode(L3, OUTPUT);            //Declaramos los pines de las salidas para los leds
}

void loop() {
  //Serial.println("Me estoy esperando hasta recibir un mensaje");
  //if (digitalRead(SWT) == LOW)  EnviarRF(0xBACAFBCD); //Prueba de envio bidireccional, ignorar en la demo
  digitalWrite(L1, bitRead(estado,8));  //Damos al led 1 el valor correspondiente al primer bit de la variable entera que contiene el valor del contador
  digitalWrite(L2, bitRead(estado,9));  //Damos al led 2 el valor correspondiente al segundo bit de la variable entera que contiene el valor del contador
  digitalWrite(L3, bitRead(estado,10)); //Damos al led 3 el valor correspondiente al tercer bit de la variable entera que contiene el valor del contador
  //Serial.println(estado,HEX);
  delay(t1);                                  //Esperamos un pequeño tiempo
}

void EnviarRF(uint32_t m){          //Función que enviara el mensaje que le cargemos
    RF.stopListening();     //Paramos de escuchar el canal
    RF.write(&m,sizeof(m)); //Escribimos en el canal de RF el valor introducido en la función
    //MostrarMensaje(msg.p, 0); //Mostramos el mensaje (Solo para pruebas)
    RF.startListening();    //Iniciamos la escucha por el canal
}

void InterrupcionMensaje(){     //Función que se activará cuando hagas una interrupción, por el pin IRQ
  //Serial.println("HOLA SOY UNA INTERRUPCION");
  msg.p = LeerRF();             //Damos a msg.p el valor que leamos por el canal RF
  estado = msg.p;               //Guardamos el estado del contaje para luego seleccionar el bit que queremos
  MostrarMensaje(msg.p, 1);   //Mostramos el mensaje (Solo para pruebas)
  /*if (msg.v[0]==0xDD)*/ EnviarRF(msgOK);              //Enviamos el mensaje de que se ha recibido correctamente
  //if (msg.p==msgOK) Seguridad = 0;  //Si el mensaje se ha recibido correctamente desactivamos la seguridad (anulado de momento)
  msg.p = 0;                        //Reiniciamos el valor de la union
}

uint32_t LeerRF(){  //Función que devolvera el valor del mensaje leido por el canal RF
  uint32_t m=0;             //Generamos una variable que luego devolveremos
  if(RF.available()){       //Si existe algo en el buffer del canal:
    RF.read(&m, sizeof(m)); //Leemos el mensaje y lo guardamos en la variable antes creada
  }
  return m;                 //Devolvemos el valor leido
}

void MostrarMensaje(uint32_t mensaje, int n){ //Función que muestra por el puerto serie el mensaje recibido
  UnionMsg m;       //Declaramos una nueva union con la varibale m
  if (mensaje!=0){  //Si el mensaje es diferente a 0(para evitar lecturas falsas)
    m.p = mensaje;  //Cargamos el valor en la variable union
    if (n == 0) Serial.print("Mensaje enviado: ");  //Si n es 0 decimos mensaje enviado
    if (n == 1) Serial.print("Mensaje recibido: "); //Si n es 0 decimos mensaje recibido
    for(int i=3;i>=0;i--){Serial.print(" "); Serial.print(m.v[i], HEX);}  //Mostramos por parejas todo el valor en hexadecimal del mensaje
    Serial.println(""); //Hacemos un salto de linea
  }
}
