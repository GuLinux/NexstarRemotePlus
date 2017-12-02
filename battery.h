#include "Arduino.h"
#include "singleton.h"
#pragma once

// Using values suitable for the bluetooth adapter,
// the only device, other than the maple mini itself, powered directly by the batteries.
#define BATTERY_MAX_MV 6000.
#define BATTERY_MIN_MV 3600.

class Battery : public Singleton<Battery> {
public:
  Battery();
  void setup();
  uint16_t millivolts();
  uint8_t percentage();
private:
  uint16_t read();
  uint32_t last_reading_time = 0;
  uint16_t last_reading = 0;
};

