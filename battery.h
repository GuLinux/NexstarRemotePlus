#include "Arduino.h"
#include "singleton.h"
#pragma once

#define BATTERY_FULL_MV 5500
#define BaTTERY_HIGH_MV 4500
#define BATTERY_MID_MV 3700

class Battery : public Singleton<Battery> {
public:
  Battery();
  void setup();
  uint16_t millivolts();
private:
  uint16_t read();
  uint32_t last_reading_time = 0;
  uint16_t last_reading = 0;
};

