#include <oledLibrary.hpp>


void setup(void) {
  Screen screen;
  screen.begin();
  delay(1000);
  screen.nextStartBMP();
  delay(1000);
  screen.nextStartBMP();
  delay(1000);

  
  screen.setTemplate(3);

  screen.setModeHeader(121, BLACK, WHITE);
  screen.setMode(16, 74, WHITE, BLACK);
  screen.setPrice(71, 78, WHITE, TEMPLATERED);
  screen.setTimer(72, 41, WHITE, TEMPLATERED);

  
  for (int i = 1; i < 1000; i++) {
    screen.updateScreen();
    screen.changeModeTo(i%5+1);
    delay(1000);
  }
}

void loop() {
}
