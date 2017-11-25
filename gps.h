#include "Arduino.h"
#include <TinyGPS++.h>
#include "singleton.h"

#pragma once

class Nexstar;
class RTC;
class GPS : public Singleton<GPS> {
public:
  GPS(HardwareSerial &port);
  void open();
  void process();
  bool wait_for_fix(uint16_t timeout_sec);
  void sleep();
  void resume();
  void close();
  void debug(Stream &stream, bool raw=false);
  inline bool has_time() { return gps.date.isValid() && gps.time.isValid() && gps.date.year() > 2016; } // hardcoded number, ok, but let's assume for a second we're not time traveling...
  inline bool has_location() const { return gps.location.isValid(); }
  inline bool has_fix() { return has_location() && has_time(); }
private:
  HardwareSerial &_port;
  TinyGPSPlus gps;
  void send_nmea(const String &command);
  void syncDateTime();
  void sync_nexstar();
};

