#include "Arduino.h"
#include "singleton.h"
#include "gps.h"
#pragma once

class Settings;
class Nexstar : public Singleton<Nexstar> {
public:
  Nexstar(HardwareSerial &port);
  inline HardwareSerial &port() { return _port; }
  void read_to(Stream &stream);
  void write(const uint8_t *buffer, int size);
  void setup();
  bool set_time();
  bool set_gps_info();
  bool ping();
private:
  bool check_connection();
  String get_reply();
  HardwareSerial &_port;
  GPS::Fix last_fix;
};

