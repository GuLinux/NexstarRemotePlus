#include "Arduino.h"
#include "singleton.h"
#pragma once

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
  inline void request_gps_fix() { _gps_fix_requested = true; }
  inline void request_nexstar_sync() { _sync_nexstar = true; }
  inline void record_button_press(bool longpress) {}
private:
  void gps_fix();
  void nexstar_sync();

  bool _gps_fix_requested = false;
  bool _sync_nexstar = false;
  uint8_t _button_pressed = 0;
};

