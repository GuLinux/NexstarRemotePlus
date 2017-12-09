#include "Arduino.h"
#include "singleton.h"
#include "gps.h"
#pragma once

#define GPS_FIXES_AVERAGE_COUNT 20

class Nexstar;
class Settings;
class Bluetooth;
class Commands;
class Display;
class PCStream;
class Processor : public Singleton<Processor> {
public:
  Processor();
  void loop();
  inline void request_gps_wakeup() { _gps_wakeup_requested = true; }
  inline void request_nexstar_sync() { _sync_nexstar = true; }
  inline void record_button_press(bool longpress) {}
  void set_gps_expire();
  void setup();
private:
  void gps_power_management();
  void gps_fix_management();

  void nexstar_sync();
  void check_connection();

  bool _gps_wakeup_requested = false;
  bool _sync_nexstar = false;
  uint8_t _button_pressed = 0;
  uint32_t _gps_expire = 0;
  GPS::Fix last_fixes[GPS_FIXES_AVERAGE_COUNT];
  uint8_t last_fixes_cnt = 0;
  uint32_t _gps_last_fix = 0;
};

