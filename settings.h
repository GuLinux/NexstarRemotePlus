#include "Arduino.h"

#pragma once
#define BT_NAME_SIZE 50

class Settings {
public:
  Settings();
  void load();
  inline const char *version() const { return data.version; }
  inline const char *bluetooth_pin() const { return data.bluetooth_pin; }
  inline const char *bluetooth_name() const { return data.bluetooth_name; }
  inline int8_t timezone() const { return data.timezone; }
  inline int8_t daylight_saving() const { return data.daylight_saving; }
  inline int16_t gps_timeout() const { return data.gps_timeout; }
  inline void bluetooth_pin(const char *pin) { set_string(data.bluetooth_pin, pin, 5); save(); }
  inline void bluetooth_name(const char *name) { set_string(data.bluetooth_name, name, BT_NAME_SIZE); save(); }
  inline void timezone(int8_t tz) { data.timezone = tz; save(); }
  inline void daylight_saving(int8_t daylight_saving) { data.daylight_saving = daylight_saving; save(); }
  inline void gps_timeout(int16_t gps_timeout) { data.gps_timeout = gps_timeout; save(); }
private:
  struct Data {
    char version[10];
    char bluetooth_pin[5];
    char bluetooth_name[BT_NAME_SIZE];
    int8_t timezone;
    int8_t daylight_saving;
    int16_t gps_timeout;
  };
  Data data;
  void save();
  void set_string(char *dest, const char *s, int field_size);
};
