#include "Arduino.h"
#include <TinyGPS++.h>
#include "singleton.h"

#pragma once

class Nexstar;
class RTC;
class GPS : public Singleton<GPS> {
public:

  struct Fix{
      double latitude; // North: positive, South: negative
      double longitude; // East: positive, West: negative
      bool valid = false;
      Fix(double lat, double lng) : latitude(lat), longitude(lng), valid(true) {}
      Fix() : valid(false) {}
      inline bool operator==(const Fix &o) const { return o.latitude == latitude && o.longitude == longitude && o.valid == valid; }
  };
  GPS(HardwareSerial &port);
  void open();
  void process();
  void sleep();
  void resume();
  void close();
  inline bool suspended() const { return _suspended; }
  inline bool has_time() { return gps.date.isValid() && gps.time.isValid() && gps.date.year() > 2016; } // hardcoded number, ok, but let's assume for a second we're not time traveling...
  inline bool has_location() const { return gps.location.isValid(); }
  inline bool has_fix() { return has_location() && has_time(); }
  Fix get_fix();
private:
  HardwareSerial &_port;
  TinyGPSPlus gps;
  bool _suspended = false;
  void send_nmea(const String &command);
  void syncDateTime();
  void sync_nexstar();
};

