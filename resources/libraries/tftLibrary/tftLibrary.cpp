#include "tftLibrary.hpp"
#include "TitilliumWeb_SemiBold7pt7b.h" // Font timer/header/price
#include "TitilliumWeb_Regular30pt7b.h" // Font mode

SdFat                SD;         // SD card filesystem
Adafruit_ImageReader reader(SD); // Image-reader object, pass in SD filesys

//Adafruit_ST7735 tft = Adafruit_ST7735(tft_CS, tft_DC, TFT_MOSI, TFT_SCLK, tft_RST);
Adafruit_ST7735 tft = Adafruit_ST7735(tft_CS, tft_DC, tft_RST);
ImageReturnCode img_stat; // Status from image-reading functions

void Screen :: erasePrevDigit(int8_t dig, int16_t background) {
  //Get starting position
  int16_t x = tft.getCursorX();
  int16_t y = tft.getCursorY();

  //Overwrite digit
  tft.setTextColor(background);
  tft.print(dig);

  //Set the cursor back to starting position
  tft.setCursor(x, y);
}

void Screen :: showTimer() {
  //Set the font and position of the timer
  tft.setFont(&TitilliumWeb_SemiBold7pt7b);
  tft.setCursor(timerxPos, timeryPos);

  //If the tens of the minutes changes that digit is updated
  if (dmin != secs / 600 % 10) {
    erasePrevDigit(dmin, timerBackgroundColor);
    dmin = secs / 600 % 10;
  }
  tft.setTextColor(timerTextColor);
  tft.print(dmin);

  //If the units of the minutes changes that digit is updated
  if (umin != secs / 60 % 10) {
    erasePrevDigit(umin, timerBackgroundColor);
    umin = secs / 60 % 10;
  }
  tft.setTextColor(timerTextColor);
  tft.print(umin);

  tft.print(":");

  //If the tens of the seconds changes that digit is updated
  if (dseg != (secs % 60) / 10) {
    erasePrevDigit(dseg, timerBackgroundColor);
    dseg = (secs % 60) / 10;
  }
  tft.setTextColor(timerTextColor);
  tft.print(dseg);

  //If the units of the seconds changes that digit is updated
  if (useg != secs % 60 % 10) {
    erasePrevDigit(useg, timerBackgroundColor);
    useg = secs % 60 % 10;
  }
  tft.setTextColor(timerTextColor);
  tft.print(useg);
}



void Screen :: begin() {
  //Serial.begin(9600);
  #if !defined(ESP32)
        while (!Serial);      // Wait for Serial Monitor before continuing
  #endif

  tft.initR(INITR_144GREENTAB); 

  // Initialize screen
  Serial.print(F("[Screen] Initializing filesystem..."));

  if (!SD.begin(SD_CS, SD_SCK_MHZ(10))) { // Breakouts require 10 MHz limit due to longer wires
    Serial.println(F("[Screen] SD begin() failed"));
    for (;;); // Fatal error, do not continue
  }
  Serial.println(F("[Screen] OK"));

  tft.fillScreen(WHITE); //Fills the screen white so no residual images in the screen memory are seen
  img_stat = reader.drawBMP("/Pantalla_1.bmp", tft, 0, 0); //Shows the first initial screen (Istobal logo)
}


void Screen :: startBMP(uint8_t BMP_number){
  //Changes the BMP on screen (input must be 1, 2 or 3)
  if(BMP_number == 1 || BMP_number == 2 || BMP_number == 3){
    startScreenBMPName[10] = BMP_number + 48;
    stat = reader.drawBMP(startScreenBMPName, tft, 0, 0);
  }
}

void Screen :: setTimer(int16_t atimerxPos, int16_t atimeryPos, uint16_t atimerTextColor, uint16_t atimerBackgroundColor) {
  //Saves the received information
  secs = 0, dmin = 0, umin = 0, dseg = 0, useg = 0;
  timerxPos = atimerxPos;
  timeryPos = atimeryPos;
  timerTextColor = atimerTextColor;
  timerBackgroundColor = atimerBackgroundColor;
}

void Screen :: updateTimer(uint16_t asecs) {
  secs = asecs;
  showTimer(); //Updates timer
}

void Screen :: setMode(int16_t amodexPos, int16_t amodeyPos, uint16_t amodeTextColor, uint16_t amodeBackgroundColor) {
  //Saves the received information
  modexPos = amodexPos;
  modeyPos = amodeyPos;
  modeTextColor = amodeTextColor;
  modeBackgroundColor = amodeBackgroundColor;
}

void Screen :: setModeHeader(int16_t amodeHeaderxPos, int16_t amodeHeaderyPos, uint16_t amodeHeaderTextColor, uint16_t amodeHeaderBackgroundColor){
  //Saves the received information
  modeHeaderxPos = modeHeaderxPos;
  modeHeaderyPos = amodeHeaderyPos;
  modeHeaderTextColor = amodeHeaderTextColor;
  modeHeaderBackgroundColor = amodeHeaderBackgroundColor;
  
  //Sets variable to know if the header of the mode is shown to true
  modeHeaderInUse = true;
}
void Screen :: setModeHeader(int16_t amodeHeaderyPos, int16_t amodeHeaderTextColor, uint16_t amodeHeaderBackgroundColor){
  //Saves the received information
  modeHeaderyPos = amodeHeaderyPos;
  modeHeaderTextColor = amodeHeaderTextColor;
  modeHeaderBackgroundColor = amodeHeaderBackgroundColor;
  
  //Sets variable to know if the header of the mode is shown to true
  modeHeaderInUse = true;
  centreModeHeader = true;
}

void Screen :: changeModeTo(int amode) {
  tft.setFont(&TitilliumWeb_Regular30pt7b);
  //Erase previous mode
  tft.setCursor(modexPos, modeyPos);
  erasePrevDigit(mode, modeBackgroundColor);
  //Show new mode
  tft.setTextColor(modeTextColor);
  tft.setCursor(modexPos, modeyPos);
  tft.print(amode);

  if(modeHeaderInUse){
      tft.setFont(&TitilliumWeb_SemiBold7pt7b);
      //Erase previous header
      tft.setCursor(modeHeaderxPos, modeHeaderyPos);
      tft.setTextColor(modeHeaderBackgroundColor);
      tft.print(modeHeaders[mode-1]);
      
      //Show new header
      if(centreModeHeader){
        int16_t  x1, y1;
        uint16_t w, h;
        tft.getTextBounds(modeHeaders[amode-1], 0, modeHeaderyPos, &x1, &y1, &w, &h);

        modeHeaderxPos = (SCREEN_WIDTH - w)/2;
      }
      tft.setCursor(modeHeaderxPos, modeHeaderyPos);
      tft.setTextColor(modeHeaderTextColor);
      tft.print(modeHeaders[amode-1]);
  }
  
  //Update mode value
  mode = amode; 
}

void Screen :: setTemplate(int templt){
  templtPath[13] = templt + 48; //Changes template path to the requiered one
  tft.fillScreen(BLACK);
  img_stat = reader.drawBMP(templtPath, tft, 0, 0); //Shows template on the screen
}

void Screen :: setPrice(int16_t apricexPos, int16_t apriceyPos, uint16_t apriceTextColor, uint16_t apriceBackgroundColor){
  //Saves the received information
  pricexPos = apricexPos;
  priceyPos = apriceyPos;
  priceTextColor = apriceTextColor;
  priceBackgroundColor = apriceBackgroundColor;
}

void Screen :: updatePrice(){
  tft.setFont(&TitilliumWeb_SemiBold7pt7b);
  tft.setCursor(pricexPos, priceyPos);

  if (secs != 0){//If the timer has not just started, the price is updated
    //If the tens of the euros changes that digit is updated
    if ((secs*pricePerSecond)/1000 != ((secs-1)*pricePerSecond)/1000) {
      erasePrevDigit(((secs-1)*pricePerSecond)/1000, priceBackgroundColor);
    }
    tft.setTextColor(priceTextColor);
    tft.print((secs*pricePerSecond)/1000);

    //If the units of the euros changes that digit is updated
    if ((secs*pricePerSecond)/100%10 != ((secs-1)*pricePerSecond)/100%10) {
      erasePrevDigit(((secs-1)*pricePerSecond)/100%10, priceBackgroundColor);
    }
    tft.setTextColor(priceTextColor);
    tft.print((secs*pricePerSecond)/100%10);

    tft.print(",");

    //If the tens of the cents changes that digit is updated
    if ((secs*pricePerSecond)/10%10 != ((secs-1)*pricePerSecond)/10%10) {
      erasePrevDigit(((secs-1)*pricePerSecond)/10%10, priceBackgroundColor);
    }
    tft.setTextColor(priceTextColor);
    tft.print((secs*pricePerSecond)/10%10);

    //If the units of the cents changes that digit is updated
    if ((secs*pricePerSecond)%10 != ((secs-1)*pricePerSecond)%10) {
      erasePrevDigit(((secs-1)*pricePerSecond)%10, priceBackgroundColor);
    }
    tft.setTextColor(priceTextColor);
    tft.print((secs*pricePerSecond)%10);

  }else{//If the timer just started, the price is shown as
    tft.print("00,00");
  }
}