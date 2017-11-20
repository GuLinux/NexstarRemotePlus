#include "Arduino.h"

#pragma once

#define HC_MODEL 5

class Settings;
class Bluetooth {
public:
  Bluetooth(HardwareSerial &port, Settings &settings);
  void setup();
  String atCommand(const String &msg);
  inline HardwareSerial &port() { return _port; }
private:
  Settings &settings;
  HardwareSerial &_port;
};

