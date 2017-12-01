#include "Arduino.h"
#include "singleton.h"
#pragma once

#define HC_MODEL 5

class Settings;
class Bluetooth : public Singleton<Bluetooth> {
public:
  Bluetooth(HardwareSerial &port);
  void setup();
  String atCommand(const String &msg);
  inline HardwareSerial &port() { return _port; }
  bool isConnected() const;
  bool booted() const { return _booted; }
  void reload_settings();
private:
  HardwareSerial &_port;
  bool _booted = false;
};

