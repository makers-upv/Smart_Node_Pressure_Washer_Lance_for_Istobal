//Librerias en uso:
#include <ComunicacionRF.hpp>
//Variables de código
#define IRQ 3
#define NODO_ESTACION   0x01
#define F_ESTADO        0x01
#define F_MODO_LIMPIEZA 0x02
#define F_CODIGO_ERROR  0xFF
//Constantes de las entradas
#define RESERVA       A0
#define HABILITACION  A1
#define BIT0          A2
#define BIT1          A3
#define BIT2          A4
int t_ini, t_next = 2500;
uint32_t MENSAJE_ANTERIOR = 0;
Comunicacion RFCom;

void setup() {
  Serial.begin(115200);             //Iniciamos el buffer de comunicacion serie a 9600 baudios
  //while(!Serial);
  pinMode(RESERVA, OUTPUT); pinMode(HABILITACION, OUTPUT); pinMode(BIT0, OUTPUT); pinMode(BIT1, OUTPUT); pinMode(BIT2, OUTPUT);
  RFCom.ConfigurarComunicacion();
  attachInterrupt(digitalPinToInterrupt(IRQ), InterrupcionMensaje, FALLING);  //Interrupir el codigo cuando se active el pin 5 por una bajada de la señal
  t_ini = millis();
}

void loop() {
  //digitalWrite(A0, HIGH); digitalWrite(A1, HIGH); digitalWrite(A2, HIGH); digitalWrite(A3, HIGH); digitalWrite(A4, HIGH);
  if (millis() >= (t_ini + t_next)) {
    Serial.println("Me estoy esperando para recibir mensajes");
    t_ini = millis();
  }
  if (MENSAJE_ANTERIOR != RFCom.msg.p) {
    if (RFCom.msg.v[3] == NODO_ESTACION) { //Si nos habla a nosotros
      if (RFCom.msg.v[0] == TestCRC) {       //Si el nodo es correcto
        switch (RFCom.msg.v[2]) {
          case F_ESTADO:
            RFCom.Estado(TestNodo, 0xFF);
            break;
          case F_MODO_LIMPIEZA:
            digitalWrite(BIT0, bitRead(RFCom.msg.v[1], 0));
            digitalWrite(BIT1, bitRead(RFCom.msg.v[1], 1));
            digitalWrite(BIT2, bitRead(RFCom.msg.v[1], 2));
            if((RFCom.msg.v[1]>=1)&&(RFCom.msg.v[1]<=5)){digitalWrite(HABILITACION, HIGH);digitalWrite(RESERVA, LOW);}
            else{digitalWrite(HABILITACION, LOW);digitalWrite(RESERVA, HIGH);}
            RFCom.Estado(TestNodo, 0xFF);
            break;
          case F_CODIGO_ERROR:
            RFCom.Estado(TestNodo, 0xFF);
            break;
          default:
            Serial.println("Funcion no declarada");
            break;
        }
        if (RFCom.msg.v[2] == F_MODO_LIMPIEZA) {     //Si nos habla del modo de limpieza
          RFCom.Estado(TestNodo, 0xFF);
        }
      }
    }
    MENSAJE_ANTERIOR = RFCom.msg.p;
  }
}
void InterrupcionMensaje() {        //Función que se activará cuando hagas una interrupción, por el pin IRQ
  RFCom.msg.p = RFCom.LeerRF(TestNodo);                 //Damos a msg.p el valor que leamos por el canal RF
  RFCom.MostrarMensaje(RFCom.msg.p, 1);         //Mostramos el mensaje (Solo para pruebas)
}
