#include <Arduino.h>
#include "singleton.h"
#include "logger.h"

#pragma once
#define BT_NAME_SIZE 50

class Settings: public Singleton<Settings> {
public:
  Settings();
  void load();
  inline const char *version() const { return data.version; }
  inline const char *bluetooth_pin() const { return data.bluetooth_pin; }
  inline const char *bluetooth_name() const { return data.bluetooth_name; }
  inline int8_t timezone() const { return data.timezone; }
  inline int8_t daylight_saving() const { return data.daylight_saving; }
  inline uint16_t gps_timeout() const { return data.gps_timeout; }
  inline Logger::Level log_level() const { return data.log_level; }
  inline bool gps_timeout_enabled() const { return gps_timeout() > 0; }
  inline void bluetooth_pin(const char *pin) { set_string(data.bluetooth_pin, pin, 5); save(); }
  inline void bluetooth_name(const char *name) { set_string(data.bluetooth_name, name, BT_NAME_SIZE); save(); }
  inline void timezone(int8_t tz) { data.timezone = tz; save(); }
  inline void daylight_saving(int8_t daylight_saving) { data.daylight_saving = daylight_saving; save(); }
  inline void gps_timeout(uint16_t gps_timeout) { data.gps_timeout = gps_timeout; save(); }
  inline void log_level(Logger::Level level) { data.log_level = level; save(); }
private:
  struct Data {
    char version[10];
    char bluetooth_pin[5];
    char bluetooth_name[BT_NAME_SIZE];
    int8_t timezone;
    int8_t daylight_saving;
    uint16_t gps_timeout;
    Logger::Level log_level;
  };
  Data data;
  void save();
  void set_string(char *dest, const char *s, int field_size);
};
