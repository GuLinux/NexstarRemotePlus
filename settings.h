#include <Arduino.h>
#include "singleton.h"
#include "logger.h"
#include "gps.h"
#pragma once
#define BT_NAME_SIZE 50
#define BT_PIN_SIZE 4

class Settings: public Singleton<Settings> {
public:
  template<typename T> struct MenuSetting {
    T value;
    uint8_t uid;
  };
  Settings();
  void load();
  inline const char *version() const { return data.version; }
  inline const char *bluetooth_pin() const { return data.bluetooth_pin; }
  inline const char *bluetooth_name() const { return data.bluetooth_name; }
  inline MenuSetting<int8_t> timezone() const { return data.timezone; }
  inline MenuSetting<int8_t> daylight_saving() const { return data.daylight_saving; }
  inline MenuSetting<uint16_t> gps_timeout() const { return data.gps_timeout; }
  inline Logger::Level log_level() const { return data.log_level; }
  inline bool gps_timeout_enabled() const { return gps_timeout().value > 0; }
  void bluetooth_pin(const char *pin);
  void bluetooth_name(const char *name);
  inline void timezone(const MenuSetting<int8_t> &tz) { data.timezone = tz; save(); }
  inline void daylight_saving(const MenuSetting<int8_t> &daylight_saving) { data.daylight_saving = daylight_saving; save(); }
  inline void gps_timeout(const MenuSetting<uint16_t> &gps_timeout) { data.gps_timeout = gps_timeout; save(); }
  inline void log_level(Logger::Level level) { data.log_level = level; save(); }
  
  inline GPS::Fix last_fix() const { return data.last_fix; }
  inline void last_fix(const GPS::Fix &fix) { data.last_fix = fix; save(); }
private:
  struct Data {
    char bluetooth_pin[BT_PIN_SIZE + 1] = {0};
    char bluetooth_name[BT_NAME_SIZE + 1] = {0};
    MenuSetting<int8_t> timezone;
    MenuSetting<int8_t> daylight_saving;
    MenuSetting<uint16_t> gps_timeout;
    Logger::Level log_level;
    GPS::Fix last_fix;
    char version[10] = {0};
  };
  Data data;
  void save();
  void set_string(char *dest, const char *s, int field_size);
};
