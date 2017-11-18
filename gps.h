#include "Arduino.h"
#include "SoftwareSerial.h"
#include <TinyGPS++.h>

#pragma once

class Nexstar;

class GPS {
public:
  GPS(int rx, int tx, Nexstar &nexstar);
  void open();
  bool wait_for_fix(uint64_t timeout_msec = 2 * 60 * 1000);
  void sleep();
  void resume();
  void close();
  void debug(Stream &stream, bool raw=false);
private:
  SoftwareSerial port;
  Nexstar &nexstar;
  TinyGPSPlus gps;
  void send_nmea(const String &command);
  void syncDateTime();
};

