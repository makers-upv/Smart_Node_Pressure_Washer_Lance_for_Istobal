#include "ComunicacionRF.hpp"

//Funciones:
    void Comunicacion :: ConfigurarComunicacion() {
        msg.p = 0x0;                    //Mensaje que enviaremos para las pruebas
        RF.begin();                     //Iniciamos la comunicación RF
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
    uint32_t Comunicacion :: LeerRF(){
        uint32_t m = 0;           //Generamos una variable que luego devolveremos
        if (RF.available()) RF.read(&m, sizeof(m)); //Leemos el mensaje y lo guardamos en la variable antes creada
        return m;                 //Devolvemos el valor leido
    }
    void Comunicacion :: MostrarMensaje(uint32_t mensaje, int n){
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