#include "Arduino.h"
#include <SoftwareSerial.h>

#pragma once

#define HC_MODEL 5

class Settings;
class Bluetooth {
public:
  Bluetooth(Settings &settings);
  void setup();
  String atCommand(const String &msg);
private:
  Settings &settings;
};

