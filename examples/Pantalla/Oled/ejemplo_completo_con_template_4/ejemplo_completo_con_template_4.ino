#include <oledLibrary.hpp>

void setup(void) {
  Screen screen;
  screen.begin();
  screen.startBMP(1);
  delay(1000);
  screen.startBMP(2);
  delay(1000);
  screen.startBMP(3);
  delay(1000);

  screen.setTemplate(4);

  screen.setModeHeader(20, WHITE, BLACK);
  screen.setMode(62, 80, WHITE, BLACK);
  screen.setPrice(10, 119, TEMPLATERED, WHITE);
  screen.setTimer(75, 119, WHITE, TEMPLATERED);


  for (int i = 0; i < 1000; i++) {
    screen.updateTimer(i);
    screen.updatePrice();
    screen.changeModeTo(i%5+1);
    delay(1000);
  }
}

void loop() {
}
