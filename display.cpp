#include "display.h"

Display::Display() {
  oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    oled.setTextSize(1);
  oled.setTextColor(WHITE);
  oled.setCursor(10,10);
  oled.println("Hello, world!");
  oled.display();
}

