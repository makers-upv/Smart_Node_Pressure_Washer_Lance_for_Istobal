#include <oledLibrary.hpp>


void setup(void) {
  Screen screen;
  screen.begin();
  delay(1000);
  screen.nextStartBMP();
  delay(1000);
  screen.nextStartBMP();
  delay(1000);

  
  screen.setTemplate(2);

  screen.setModeHeader(119, WHITE, TEMPLATERED);
  screen.setMode(61, 84, WHITE, BLACK);
  screen.setPrice(89, 24, WHITE, BLACK);
  screen.setTimer(30, 24, WHITE, BLACK);

  
  for (int i = 1; i < 1000; i++) {
    screen.updateScreen();
    screen.changeModeTo(i%5+1);
    delay(1000);
  }
}

void loop() {
}
