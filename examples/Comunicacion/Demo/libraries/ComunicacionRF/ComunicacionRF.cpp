#include "ComunicacionRF.hpp"

//Funciones:
    void Comunicacion :: ConfigurarComunicacion() {
        msg.p = 0x0;                    //Mensaje que enviaremos para las pruebas

        if(!RF.begin()) {
            Serial.println("Error al inicializar el dispositivo");
            while(1);
        }

        //RF.begin();                     //Iniciamos la comunicación RF
        RF.openWritingPipe(CANALRF);      //Iniciamos la escritura por el canal RF
        RF.openReadingPipe(1, CANALRF);   //Abrimos la escucha por el canal RF
        RF.startListening();            //Empezamos a escuchar
        RF.maskIRQ(1, 1, 0);            //Solo permitimos que se active el pin de interrupcion cuando recibe datos
    }
    void Comunicacion :: EnviarRF(uint32_t m){
        RF.stopListening();     //Paramos de escuchar el canal
        RF.write(&m, sizeof(m)); //Escribimos en el canal de RF el valor introducido en la función
        MostrarMensaje(m, 0); //Mostramos el mensaje (Solo para pruebas)
        RF.startListening();    //Iniciamos la escucha por el canal
    }
    uint32_t Comunicacion :: LeerRF(int nodo){
        UnionMsg msgReceived;
        if (RF.available()) RF.read(&msgReceived.p, sizeof(msgReceived.p)); //Leemos el mensaje y lo guardamos en la variable antes creada
        //Comprobamos si el CRC es correcto
        Serial.println(",");
        if(msgReceived.v[3]==TestNodo){
            if (msgReceived.v[0]==TestCRC){
                return msgReceived.p;                 //Devolvemos el valor leido         
            }
            else{
                Serial.println("Mensaje NO_OK");
                return 0;
            }                      
        }
        return 0;
    }
    void Comunicacion :: MostrarMensaje(uint32_t mensaje, int n){
        UnionMsg m;       //Declaramos una nueva union con la varibale m
        m.p = mensaje;  //Cargamos el valor en la variable union
        //Serial.println(".");
        if (m.p != 0) { //Si el mensaje es diferente a 0(para evitar lecturas falsas)
            if (n == 0) Serial.print("Mensaje enviado: ");  //Si n es 0 decimos mensaje enviado
            if (n == 1) Serial.print("Mensaje recibido: "); //Si n es 1 decimos mensaje recibido
            for (int i = 3; i >= 0; i--) {Serial.print(" ");Serial.print(m.v[i], HEX);}Serial.println(""); //Lo mostramos en el puerto serie
        }
    }
    void Comunicacion :: Estado(int nodo, int estado_maquina){
        msg.v[3] = nodo;
        msg.v[2] = 0x01;
        msg.v[1] = estado_maquina;
        msg.v[0] = TestCRC;
        EnviarRF(msg.p);
    }
    void Comunicacion :: CambiarModoLimpieza(int nodo, int modo_trabajo){
        msg.v[3] = nodo;
        msg.v[2] = 0x02;
        msg.v[1] = modo_trabajo;
        msg.v[0] = TestCRC;
        EnviarRF(msg.p);
    }
    void Comunicacion :: CodigoError(int nodo, int ErrorCode){
        msg.v[3] = nodo;
        msg.v[2] = 0xFF;
        msg.v[1] = ErrorCode;
        msg.v[0] = TestCRC;
        EnviarRF(msg.p);
    }