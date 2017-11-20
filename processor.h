#include "Arduino.h"

#pragma once

class GPS;
class Nexstar;
class Settings;
class Bluetooth;
class Commands;
class Display;
class PCStream;
class Processor {
public:
  Processor(Nexstar &nexstar, GPS &gps, Bluetooth &bluetooth, Display &display, Commands &commands, Settings &settings, PCStream &pc_stream);
  void loop();
  inline void gps_getfix() { _gps_fix_requested = true; }
  inline void sync_nexstar() { _sync_nexstar = true; }
  inline void button_pressed(bool longpress) {}
private:
  Nexstar &nexstar;
  GPS &gps;
  Bluetooth &bluetooth;
  Display &display;
  Commands &commands;
  Settings &settings;
  PCStream &pc_stream;

  bool _gps_fix_requested = false;
  bool _sync_nexstar = false;
  uint8_t _button_pressed = 0;
};

