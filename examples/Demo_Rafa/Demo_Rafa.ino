#define DEBUG // Activar proceso de debugging

/* MAQUINA DE ESTADOS */
/* LA EJECUCIÓN SE PRODUCE CUANDO HAY UN EVENTO */
/* ENTRE CAMBIO DE ESTADOS */
enum estados {SOPORTE, STANDBY, WAIT_COM, LAVADO};
enum eventos {CARGANDO, NO_CARGANDO, BACK, NEXT, EXIT_, UPDATE_VIEW, TIMEOUT};

struct maquina_estados_struct {
  estados estado;     //Estado actual en el que nos encontramos
  eventos evento;     //Evento que acaba de suceder
  bool flag_evento;   //Flag de que hay un evento que no se ha procesado
};

maquina_estados_struct Maquina_Estados = {SOPORTE, CARGANDO, false}; //Estructura que nos almacenará las variables de la maquina de estado.

const byte interruptPin_Cargando = 17;

#define EMULATE_GESTURES_BTN
#ifdef EMULATE_GESTURES_BTN
  const byte interruptPin_Btn_BACK = 16;
  const byte interruptPin_Btn_NEXT = 15;
  const byte interruptPin_Btn_EXIT = 14;
#endif

/* TIMER */
#define USE_TIMER_2 true // TIMER EN USO - TIMER EN USO
#include <megaAVR_TimerInterrupt.h>
#include <megaAVR_TimerInterrupt.hpp>
#define TIMER2 1  // Frecuencia (Hz)

/* PANTALLA */
#include "tftLibrary.hpp"
Screen screen;
int ModoTrabajo = 1;
int counterTime = 0;
const int counterTimeOut = 30;

/* GESTOS */
//#define GESTOS
#ifdef GESTOS
  #include <Wire.h>
  #include <SparkFun_APDS9960.h>
  SparkFun_APDS9960 apds = SparkFun_APDS9960();
#endif

/* DEFINICIÓN DE FUNCIONES */
void Proceso_Maquina(maquina_estados_struct *Maquina_Estados_puntero);
void interruptHandler_Btn_NEXT();
void interruptHandler_Btn_BACK();
void interruptHandler_Btn_EXIT();
void interruptHandler_Cargando();
void interruptHandler_Timer();
void LAVADO_ON();
void LAVADO_OFF();

void setup() {
#ifdef DEBUG
  Serial.begin(115200);
  while (!Serial); // wait for serial to be ready
  for (int i = 0; i < 20; i++) Serial.println();
#endif

  /* Inicialización de los pulsadores de simulación de eventos */
#ifdef EMULATE_GESTURES_BTN
  pinMode(interruptPin_Btn_NEXT, INPUT_PULLUP);
  pinMode(interruptPin_Btn_BACK, INPUT_PULLUP);
  pinMode(interruptPin_Btn_EXIT, INPUT_PULLUP);
#endif
  pinMode(interruptPin_Cargando, INPUT_PULLUP);

#ifdef EMULATE_GESTURES_BTN
  attachInterrupt(digitalPinToInterrupt(interruptPin_Btn_BACK), interruptHandler_Btn_BACK, RISING);
  attachInterrupt(digitalPinToInterrupt(interruptPin_Btn_NEXT), interruptHandler_Btn_NEXT, RISING);
  attachInterrupt(digitalPinToInterrupt(interruptPin_Btn_EXIT), interruptHandler_Btn_EXIT, RISING);
#endif
  attachInterrupt(digitalPinToInterrupt(interruptPin_Cargando), interruptHandler_Cargando, RISING);

  /* Inicialización del Temporizador */
  ITimer2.init();
  ITimer2.pauseTimer();
#ifdef DEBUG
  if (ITimer2.attachInterrupt(TIMER2, interruptHandler_Timer))  Serial.println("Se ha iniciado el temporizador 2 (timer2).");
  else {
    Serial.println("Ha habido un error con el timer2.");
    while (1);
  }
#else
  if (!ITimer2.attachInterrupt(TIMER2, interruptHandler_Timer)) while (1);
#endif

#ifdef GESTOS
  /* Inicialización del sensor de gestos */
#ifdef DEBUG
  if (apds.init())  Serial.println("Inicialización del APDS-9960.");
  else {
    Serial.println("¡Algo salió mal durante la inicialización de APDS-9960!");
    while (1);
  }
  if (apds.enableGestureSensor(true))   Serial.println("El sensor de gestos inicializado correctamente!.");
  else {
    Serial.println("¡Algo salió mal durante el inicio del sensor de gestos!");
    while (1);
  }
#else
  if (!apds.init()) while (1);
  if (!apds.enableGestureSensor(true))    while (1);
  if (!apds.enableProximitySensor(true))  while (1);
#endif
#endif

  /* Inicialización de la pantalla */
  screen.begin();
  screen.logoIstobal();

#ifdef DEBUG
  Serial.println("LANZA ENCENDIDA");
#endif

  // TODO: ¿Si se reinicia cuando esta funcionando?
  // Recuperación del estado
  // TODO: FUNCIÓN DE AUTOREINICIO CUANDO INICIALIZAMOS Y SE QUEDA COLGADA
}

void loop() {

  // Maquina de estados
  if (Maquina_Estados.flag_evento == true) {
    Maquina_Estados.flag_evento = false; // ¿Y si hay más de un evento? Hay colisión de eventos
    Proceso_Maquina(&Maquina_Estados);
  }

#ifdef GESTOS
  #ifdef DEBUG
    if ( apds.isGestureAvailable() ) {
      switch ( apds.readGesture() ) {
        //      case DIR_UP:
        //        Serial.println("UP");
        //        break;
        case DIR_DOWN:
          Serial.println("DOWN");
          interruptHandler_Btn_EXIT();
          break;
        case DIR_LEFT:
          Serial.println("LEFT");
          interruptHandler_Btn_NEXT();
          break;
        case DIR_RIGHT:
          Serial.println("RIGHT");
          interruptHandler_Btn_BACK();
          break;
        //      case DIR_NEAR:
        //        Serial.println("NEAR");
        //        break;
        //      case DIR_FAR:
        //        Serial.println("FAR");
        //        break;
        default:
          Serial.println("NONE");
      }
    }
  #else
    if ( apds.isGestureAvailable() ) {
      switch ( apds.readGesture() ) {
        case DIR_DOWN:
          interruptHandler_Btn_EXIT();
          break;
        case DIR_LEFT:
          interruptHandler_Btn_NEXT();
          break;
        case DIR_RIGHT:
          interruptHandler_Btn_BACK();
          break;
      }
    }
  #endif
#endif


}

void Proceso_Maquina(maquina_estados_struct * Maquina_Estados_puntero) {
  switch (Maquina_Estados_puntero->estado) {
    case SOPORTE:
      switch (Maquina_Estados_puntero->evento) {
        case NO_CARGANDO:
#ifdef DEBUG
          Serial.println("SOPORTE -> NO_CARGANDO -> STANDBY");
#endif
          screen.greyScreen();
          screen.logoIstobal();
          delay(1000);
          screen.welcome();
          delay(2000);
          screen.waveStart();
          delay(2000);
          Maquina_Estados.estado = STANDBY;
          break;
        default:
#ifdef DEBUG
          Serial.println("SOPORTE - NO TRANSITION");
#endif
          break;
      }
      break;
    case STANDBY:
      switch (Maquina_Estados_puntero->evento) {
        case CARGANDO:
#ifdef DEBUG
          Serial.println("STANDBY -> CARGANDO -> SOPORTE");
#endif
          screen.redScreen();
          screen.logoIstobal();
          Maquina_Estados.estado = SOPORTE;
          break;
        case NEXT:
#ifdef DEBUG
          Serial.println("STANDBY -> NEXT -> LAVADO");
#endif
          Maquina_Estados.estado = LAVADO;
          LAVADO_ON();
          break;
        default:
#ifdef DEBUG
          Serial.println("STANDBY - NO TRANSITION");
#endif
          break;
      }
      break;
    case LAVADO:
      switch (Maquina_Estados_puntero->evento) {
        case CARGANDO:
          Maquina_Estados.estado = SOPORTE;
          LAVADO_OFF();
#ifdef DEBUG
          Serial.println("LAVADO -> FINALIZADO -> CARGANDO -> SOPORTE");
#endif
          break;
        case TIMEOUT:
          Maquina_Estados.estado = STANDBY;
          LAVADO_OFF();
#ifdef DEBUG
          Serial.println("LAVADO -> FINALIZADO -> TIMEOUT -> STANDBY");
#endif
          break;
        case EXIT_:
          Maquina_Estados.estado = STANDBY;
          LAVADO_OFF();
#ifdef DEBUG
          Serial.println("LAVADO -> FINALIZADO -> EXIT -> STANDBY");
#endif
          break;
        case NEXT:
          if (ModoTrabajo < 5) {
            ModoTrabajo++;
            screen.changeModeTo(ModoTrabajo);
#ifdef DEBUG
            Serial.println("LAVADO -> INCREMENTANDO MODO");
            Serial.print("MODO DE LAVADO: ");
            Serial.println(ModoTrabajo);
#endif
          } else {
            // FINALIZANDO LAVADO
            Maquina_Estados.estado = STANDBY;
            LAVADO_OFF();
#ifdef DEBUG
            Serial.println("LAVADO -> FINALIZADO -> STANDBY");
#endif
          }

          break;
        case BACK:
          if (ModoTrabajo > 1) {
            ModoTrabajo--;
            screen.changeModeTo(ModoTrabajo);
#ifdef DEBUG
            Serial.println("LAVADO -> DECREMENTANDO MODO");
#endif
          }
#ifdef DEBUG
          Serial.print("MODO DE LAVADO: ");
          Serial.println(ModoTrabajo);
#endif
          break;
        case UPDATE_VIEW:
          screen.updateScreen(); //Actualiza el tiempo y el dinero
#ifdef DEBUG
          Serial.print(".");
#endif
          break;
        default:
#ifdef DEBUG
          Serial.println("LAVADO - NO TRANSITION");
#endif
          break;
      }
      break;
    default:
      break;
  }
}

void interruptHandler_Btn_NEXT() {
#ifdef DEBUG
  Serial.println("NEXT");
#endif
  Maquina_Estados.evento = NEXT;
  Maquina_Estados.flag_evento = true;
}

void interruptHandler_Btn_BACK() {
#ifdef DEBUG
  Serial.println("BACK");
#endif
  Maquina_Estados.evento = BACK;
  Maquina_Estados.flag_evento = true;
}

void interruptHandler_Btn_EXIT() {
#ifdef DEBUG
  Serial.println("EXIT");
#endif
  Maquina_Estados.evento = EXIT_;
  Maquina_Estados.flag_evento = true;
}

void interruptHandler_Cargando() {
  if (Maquina_Estados.estado == SOPORTE) Maquina_Estados.evento = NO_CARGANDO;
  else Maquina_Estados.evento = CARGANDO;
  Maquina_Estados.flag_evento = true;
}

void interruptHandler_Timer() {
  if (counterTime < counterTimeOut) {
    counterTime++;
    Maquina_Estados.evento = UPDATE_VIEW;
  }
  else {
    Maquina_Estados.evento = TIMEOUT;
  }
  Maquina_Estados.flag_evento = true;
}

void LAVADO_ON() {
  //Preparamos la pantalla
  screen.setTemplate(4);
  screen.setModeHeader(20, WHITE, BLACK);
  screen.setMode(62, 80, WHITE, BLACK);
  screen.setPrice(10, 119, TEMPLATERED, WHITE);
  screen.setTimer(75, 119, WHITE, TEMPLATERED);
  screen.changeModeTo(ModoTrabajo);
  screen.clearTimeAndMoney();
  screen.updateScreen(); //Actualiza el tiempo y el dinero
  ITimer2.resumeTimer();  //Iniciamos el temporizador
#ifdef DEBUG
  Serial.println("LAVADO ENMARCHA");
  Serial.print("MODO DE LAVADO: ");
  Serial.println(ModoTrabajo);
#endif
}

void LAVADO_OFF() {
  ITimer2.pauseTimer();
  ModoTrabajo = 1;
  counterTime = 0;
  delay(5000); /* PANTALLA DE UNOS SEGUNDOS CON LA INFO DEL LAVADO */
  screen.greyScreen();
  screen.logoIstobal();
#ifdef DEBUG
  Serial.println("FIN DEL LAVADO");
#endif
}
