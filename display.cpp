#include "display.h"

Display::Display() {
  oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);
}

