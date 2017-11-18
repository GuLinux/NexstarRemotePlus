#include "Arduino.h"
#include "SoftwareSerial.h"

#pragma once

class Settings;
class Nexstar {
public:
  Nexstar(Settings &settings, int rx, int tx);
  inline SoftwareSerial &port() { return _port; }
  void write(uint8_t *buffer, int size);
  void read_to(Stream &stream);
  void setup();
  void set_time();
  void set_gps_info(double latitude, double longitude);
private:
  Settings &settings;
  SoftwareSerial _port;
};

