#include <Arduino.h>
#include "singleton.h"
#pragma once

class PCStream : public Singleton<PCStream>{
public:
  PCStream();
  Stream &current();
  enum Connection { USB, Bluetooth };
  Connection connection() const;
private:
};
