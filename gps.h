#include "Arduino.h"
#include "SoftwareSerial.h"
#include <TinyGPS++.h>

#pragma once

class Nexstar;

class GPS {
public:
  GPS(int rx, int tx, Nexstar &nexstar);
  void open();
  bool wait_for_fix(uint16_t timeout_sec);
  void sleep();
  void resume();
  void close();
  void debug(Stream &stream, bool raw=false);
  inline bool has_time() const { return gps.date.isValid() && gps.time.isValid(); }
  inline bool has_location() const { return gps.location.isValid(); }
  inline bool has_fix() const { return has_location() && has_time(); }
private:
  SoftwareSerial port;
  Nexstar &nexstar;
  TinyGPSPlus gps;
  void send_nmea(const String &command);
  void syncDateTime();
};

