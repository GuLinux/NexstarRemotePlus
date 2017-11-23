#include <Arduino.h>
#include "singleton.h"
#pragma once

class uRTCLib;
class RTC: public Singleton<RTC> {
public:
  RTC();
  ~RTC();
  void setup();
  time_t get_time_t();
  void set_time(time_t time);
private:
  uRTCLib *rtc;
};

