#pragma once
//Librerias en uso:
#include <SPI.h>
#include <nRF24L01.h>
//#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>
//Constantes de los pines:
#define CE 9
#define CSN 10
#define CANALRF 0xE8E8F0F0E1LL
static RF24 RF(CE, CSN);

class Comunicacion{
    private:

    public:
    union UnionMsg {
        uint32_t p;
        uint8_t v[4];
    } msg;
    void ConfigurarComunicacion();
    void EnviarRF(uint32_t m);
    void InterrupcionMensaje();
    uint32_t LeerRF();
    void MostrarMensaje(uint32_t mensaje, int n);
};