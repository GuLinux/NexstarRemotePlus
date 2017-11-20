#include "Arduino.h"

#pragma once

class GPS;
class Nexstar;
class Bluetooth;
class Settings;
struct Command;
class Processor;
typedef 
class Commands {
public:
  Commands(GPS &gps, Nexstar &nexstar, Bluetooth &bluetooth, Settings &settings);
  inline uint8_t *buffer() const { return _buffer; };
  inline size_t buffer_len() const { return _buffer_len; }
  void read();
  inline void set_processor(Processor *processor) { this->processor = processor; }

private:
  GPS &gps;
  Nexstar &nexstar;
  Bluetooth &bluetooth;
  Settings &settings;
  Processor *processor;
  uint8_t _buffer[3];
  uint8_t _buffer_len;
  void handle(const String &command);
  // Command handlers
  void gps_fix(const Command &command);
  void gps_debug(const Command &command);
  void bluetooth_name(const Command &command);
  void bluetooth_pin(const Command &command);
  void change_tz(const Command &command);
  void time(const Command &command);
  void bluetooth_settings_changed();
};

