#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "Arduino.h"

#pragma once

class Display {
public:
  Display();
  void begin();
private:
  Adafruit_SSD1306 oled;
};

