#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "singleton.h"
#include "Arduino.h"
#include "processor.h"

#pragma once

class Display : public Singleton<Display> {
public:
  Display();
  void begin();
  void update();
private:
  Adafruit_SSD1306 oled;
  GFXcanvas1 osd_canvas;
};

