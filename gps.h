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
  bool wait_for_fix(uint16_t timeout_sec);
  void sleep();
  void resume();
  void close();
  void debug(Stream &stream, bool raw=false);
  inline bool has_time() const { return gps.date.isValid() && gps.time.isValid(); }
  inline bool has_location() const { return gps.location.isValid(); }
  inline bool has_fix() const { return has_location() && has_time(); }
private:
  HardwareSerial &_port;
  TinyGPSPlus gps;
  void send_nmea(const String &command);
  void syncDateTime();
};

