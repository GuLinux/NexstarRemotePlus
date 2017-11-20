#include "Arduino.h"

#pragma once

class Settings;
class Nexstar {
public:
  Nexstar(HardwareSerial &port, Settings &settings);
  inline HardwareSerial &port() { return _port; }
  void read_to(Stream &stream);
  void setup();
  void set_time();
  void set_gps_info(double latitude, double longitude);
private:
  Settings &settings;
  HardwareSerial &_port;
};

