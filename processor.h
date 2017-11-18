#include "Arduino.h"

#pragma once

class GPS;
class Nexstar;
class Settings;
class Bluetooth;
class Commands;
class Processor {
public:
  inline Processor(Nexstar &nexstar, GPS &gps, Bluetooth &bluetooth, Commands &commands, Settings &settings) : nexstar{nexstar}, gps{gps}, bluetooth{bluetooth}, commands{commands}, settings{settings} {}
  void loop();
  inline void gps_getfix(int timeout=0) { _gps_fix_requested = true; _gps_fix_timeout=timeout; }
  inline void sync_nexstar() { _sync_nexstar = true; }
private:
  Nexstar &nexstar;
  GPS &gps;
  Bluetooth &bluetooth;
  Commands &commands;
  Settings &settings;

  bool _gps_fix_requested;
  int _gps_fix_timeout;
  bool _sync_nexstar;
};

