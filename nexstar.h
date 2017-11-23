#include "Arduino.h"
#include "singleton.h"
#pragma once

class Settings;
class Nexstar : public Singleton<Nexstar> {
public:
  Nexstar(HardwareSerial &port);
  inline HardwareSerial &port() { return _port; }
  void read_to(Stream &stream);
  void setup();
  void set_time();
  void set_gps_info(double latitude, double longitude);
private:
  HardwareSerial &_port;
};

