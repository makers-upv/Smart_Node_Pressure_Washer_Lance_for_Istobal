//Librerias en uso:
  #include <Arduino.h>          //Libreria de arduino
  #include <SPI.h>              //libreria comunicacion SPI
  #include <nRF24L01.h>         //Libreria de la parte de comunicacion
  #include <printf.h>           //Libreria de la parte de comunicacion
  #include <RF24.h>             //Libreria de la parte de comunicacion
  #include <RF24_config.h>      //Libreria de la parte de comunicacion

//Nuestras librerias:
  #include <com.hpp>

#pragma region Variables_Comuniacion  
  //Variables referentes a la comunicación
  const int IRQ = 3, CE = 9, CSN = 10;    //Declaramos en variables constantes los pines de CE, CSN y IRQ
  const uint64_t canal = 0xE8E8F0F0E1LL;  //Canal de comunicación
  const uint32_t msgOK = 0x01FF00DD;      //Variable equivalente a que un mensaje es correcto
  RF24 RF (CE,CSN);                       //Declaramos la variable del canal de comunicacion, como en los servos de arduino
  //Variables de código
  int t1=50, Seguridad = 0, cuenta = 0;
  //Uniones
  union UnionMsg{ //Union para poder ver unicamente cada una de las 4 parejas de valores hexadecimales que componen el mensaje que se envia
    uint32_t p;
    uint8_t v[4];
  } msg;
#pragma endregion

#pragma region Funciones_de_comuniacion
  void ConfigurarComuniacion(){
      msg.p = 0x0;//0xFFCACADD        //Mensaje que enviaremos para las pruebas
      Serial.begin(9600);             //Iniciamos el buffer de comunicacion serie a 9600 baudios
      RF.begin();                     //Iniciamos la comunicación RF
      RF.openWritingPipe(canal);      //Iniciamos la escritura por el canal RF
      RF.openReadingPipe(1, canal);   //Abrimos la escucha por el canal RF
      RF.startListening();            //Empezamos a escuchar
      pinMode(SWT, INPUT_PULLUP);     //Declaramos el pin del SWT como entrada pullup
      RF.maskIRQ(1,1,0);              //Solo permitimos que se active el pin de interrupcion cuando recibe datos
      attachInterrupt(digitalPinToInterrupt(IRQ), InterrupcionMensaje, FALLING);  //Interrupir el codigo cuando se active el pin 5 por una bajada de la señal
    }
  void SubirCuenta(){     //Función para subir la cuenta
    msg.v[1] = cuenta++;  //Damos al valor que se enviara el de la cuenta + 1
    RFCambiarValorContaje(msg.v[1]);  //Enviamos el nuevo valor de contaje
  }
  void BajarCuenta(){     //Función para bajar la cuenta
    msg.v[1] = cuenta--;  //Damos al valor que se enviara el de la cuenta - 1
    RFCambiarValorContaje(msg.v[1]);  //Enviamos el nuevo valor de contaje
  }
  void RFCambiarValorContaje(int n){  //Funcion para cambiar el valor del contaje
    msg.v[3] = 0x01;    //ID  -> Identificador, siempre sera 0x01
    msg.v[2] = 0x01;    //FID -> 0x01 Lo asigno a que es el cambio de variable del contador
    msg.v[1] = n;       //Valor -> Valor nuevo que tendra la variable
    msg.v[0] = 0xDD;    //CRC -> CRC, para las demos sera siempre 0xDD
    EnviarRF(msg.p);    //Enviamos el valor del mensaje con los nuevos parametros introducidos
  }
  void EnviarRF(uint32_t m){          //Función que enviara el mensaje que le cargemos
    if (m != msgOK){          //Si el mensaje es diferente a que esta OK
      RF.stopListening();     //Paramos de escuchar el canal
      RF.write(&m,sizeof(m)); //Escribimos en el canal de RF el valor introducido en la función
      //MostrarMensaje(msg.p, 0); //Mostramos el mensaje (Solo para pruebas)
      RF.startListening();    //Iniciamos la escucha por el canal
      Seguridad = 0/*1*/;          //Activamos una variable para asegurarnos que se ha recibido el mensaje
    }
  }
  void InterrupcionMensaje(){         //Función que se activará cuando hagas una interrupción, por el pin IRQ
    //Serial.println("HOLA SOY UNA INTERRUPCION");
    msg.p = LeerRF();                 //Damos a msg.p el valor que leamos por el canal RF
    MostrarMensaje(msg.p, 1);         //Mostramos el mensaje (Solo para pruebas)
    /*if (msg.v[0]==0xDD)*/ EnviarRF(msgOK);              //Enviamos el mensaje de que se ha recibido correctamente
    if (msg.p==msgOK) Seguridad = 0; //Si el mensaje se ha recibido correctamente desactivamos la seguridad
  }
  uint32_t LeerRF(){          //Función que devolvera el valor del mensaje leido por el canal RF
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
#pragma endregion

