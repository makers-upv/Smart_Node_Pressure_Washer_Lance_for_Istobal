#include <tftLibrary.hpp>

void setup(void) {
  Screen screen;
  screen.begin();
  screen.startBMP(1);
  delay(1000);
  screen.startBMP(2);
  delay(1000);
  screen.startBMP(3);
  delay(1000);

  
  screen.setTemplate(3);

  screen.setModeHeader(121, BLACK, WHITE);
  screen.setMode(16, 74, WHITE, BLACK);
  screen.setPrice(71, 78, WHITE, TEMPLATERED);
  screen.setTimer(72, 41, WHITE, TEMPLATERED);

  
  for (int i = 0; i < 1000; i++) {
    screen.updateTimer(i);
    screen.updatePrice();
    screen.changeModeTo(i%5+1);
    delay(1000);
  }
}

void loop() {
}
