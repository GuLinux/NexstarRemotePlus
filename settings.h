#include "Arduino.h"

#pragma once

class Settings {
public:
  Settings();
  void load();
  inline char *version() const { return data.version; }
  inline char *bluetooth_pin() const { return data.bluetooth_pin; }
  inline char *bluetooth_name() const { return data.bluetooth_name; }
  inline int8_t timezone() const { return data.timezone; }
  inline int8_t daylight_saving() const { return data.daylight_saving; }
  inline void bluetooth_pin(const char *pin) { set_string(data.bluetooth_pin, pin, 5); save(); }
  inline void bluetooth_name(const char *name) { set_string(data.bluetooth_name, name, 100); save(); }
  inline void timezone(int8_t tz) { data.timezone = tz; save(); }
  inline void daylight_saving(int8_t daylight_saving) { data.daylight_saving = daylight_saving; save(); }
private:
  struct Data {
    char version[10];
    char bluetooth_pin[5];
    char bluetooth_name[100];
    int8_t timezone;
    int8_t daylight_saving;
  };
  Data data;
  void save();
  void set_string(char *dest, const char *s, int field_size);
};
