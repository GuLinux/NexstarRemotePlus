#include "Arduino.h"
#include "singleton.h"
#pragma once

class Nexstar;
class Bluetooth;
class Settings;
struct Command;
class PCStream;
class Processor;

class Commands : public Singleton<Commands> {
public:
  Commands();
  inline const uint8_t *buffer() const { return _buffer; };
  inline size_t buffer_len() const { return _buffer_len; }
  void read();

private:
  uint8_t _buffer[3];
  uint8_t _buffer_len;
  void handle(const String &command);
  // Command handlers
  void wake_gps(const Command &command);
  void gps_debug(const Command &command);
  void bluetooth_name(const Command &command);
  void bluetooth_pin(const Command &command);
  void change_tz(const Command &command);
  void time(const Command &command);
  void bluetooth_settings_changed();
};

