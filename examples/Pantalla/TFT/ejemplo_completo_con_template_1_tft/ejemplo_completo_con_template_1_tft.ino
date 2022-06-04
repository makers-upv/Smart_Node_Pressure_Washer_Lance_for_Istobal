#include <tftLibrary.hpp>


void setup(void) {
  Screen screen;
  screen.begin();
  delay(1000);
  screen.nextStartBMP();
  delay(1000);
  screen.nextStartBMP();
  delay(1000);

  
  screen.setTemplate(1);

  screen.setModeHeader(18, WHITE, TEMPLATERED);
  screen.setMode(62, 80, WHITE, BLACK);
  screen.setPrice(88, 113, WHITE, BLACK);
  screen.setTimer(29, 113, WHITE, BLACK);

  
  for (int i = 1; i < 1000; i++) {
    screen.updateScreen();
    screen.changeModeTo(i%5+1);
    delay(1000);
  }
}

void loop() {
}
