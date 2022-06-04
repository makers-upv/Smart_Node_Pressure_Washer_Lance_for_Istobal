#pragma once

#include <Adafruit_GFX.h>         // Core graphics library
#include <Adafruit_ST7735.h>     // Hardware-specific library
#include <SdFat.h>                // SD card & FAT filesystem library
#include <Adafruit_SPIFlash.h>    // SPI / QSPI flash library
#include <Adafruit_ImageReader.h> // Image-reading functions


// Color definitions
#define BLACK           0x0000
#define BLUE            0x001F
#define RED             0xF800
#define GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0
#define WHITE           0xFFFF
#define TEMPLATERED     0xC885

// Screen dimensions
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 128

#define SD_CS    4 // SD card select pin
#define tft_CS   10 // tft select pin
#define tft_DC   8 // tft display/command pin
#define tft_RST  9 // Or set to -1 and connect to Arduino RESET pin



class Screen {
  private:
    //Timer constants
    int8_t dmin = 0, umin = 0, dseg = 0, useg = 0;
    int16_t timerxPos = 0, timeryPos = 0;
    uint16_t timerTextColor, timerBackgroundColor;
    int secs = -1;

    //Mode constants
    int mode = 0;
    int16_t modexPos = 0, modeyPos = 0;
    uint16_t modeTextColor, modeBackgroundColor;
    int16_t modeHeaderxPos = 0, modeHeaderyPos = 0;
    uint16_t modeHeaderTextColor, modeHeaderBackgroundColor;
    bool modeHeaderInUse = false, centreModeHeader = false;
    char modeHeaders[5][20] = {"Mode_1", "Mode_2", "Mode_3", "Mode_4", "Mode_5"};

    //Price constants
    int16_t pricexPos = 0, priceyPos = 0;
    uint16_t priceTextColor, priceBackgroundColor;
    int price = 0; //Price in cents
    int pricePerSecond = 2; //Price increase in cents/second

    //Teplate path in the SD card
    char templtPath[19] = "/Modo_prueba_X.bmp";

    //Start screen path
    uint8_t startScreenNumber = 1;
    char startScreenBMPName[16] = "/Pantalla_X.bmp";


    void erasePrevDigit(int8_t dig, int16_t background);
    void showTimer();

  public:
    void begin();
    void nextStartBMP();

    void setTimer(int16_t atimerxPos, int16_t atimeryPos, uint16_t atimerTextColor, uint16_t atimerBackgroundColor);
    void updateTimer();

    void setMode(int16_t amodexPos, int16_t amodeyPos, uint16_t amodeTextColor, uint16_t amodeBackgroundColor);
    void setModeHeader(int16_t amodeHeaderxPos, int16_t amodeHeaderyPos, uint16_t amodeHeaderTextColor, uint16_t amodeHeaderBackgroundColor);
    void setModeHeader(int16_t amodeHeaderyPos, int16_t amodeHeaderTextColor, uint16_t amodeHeaderBackgroundColor);
    void changeModeTo(int amode);

    void setTemplate(int templt);

    void setPrice(int16_t apricexPos, int16_t apriceyPos, uint16_t apriceTextColor, uint16_t apriceBackgroundColor);
    void updatePrice();
    
    void updateScreen();
};
