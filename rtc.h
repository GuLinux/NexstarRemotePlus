#include <Arduino.h>
#include "singleton.h"
#pragma once

#define RTC_WITH_EEPROM

class uRTCLib;
class RTC: public Singleton<RTC> {
public:
  RTC();
  ~RTC();
  void setup();
  time_t get_time_t();
  void set_time(time_t time);
#ifdef RTC_WITH_EEPROM
  void write_ee(const uint32_t address, const uint8_t value);
  uint8_t read_ee(const uint32_t address);
  void update_ee(const uint32_t address, const uint8_t value);
#endif
private:
  uRTCLib *rtc;
};

