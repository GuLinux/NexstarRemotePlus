#include <Arduino.h>
#include "singleton.h"
#pragma once


class uRTCLib;
class RTC: public Singleton<RTC> {
public:
  RTC();
  ~RTC();
  void setup();
  time_t utc();
  time_t localtime();
  void set_time(time_t time);
  void write_ee(const uint32_t address, const uint8_t value);
  uint8_t read_ee(const uint32_t address);
  void update_ee(const uint32_t address, const uint8_t value);
private:
  uRTCLib *rtc;
};

