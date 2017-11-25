#include <Arduino.h>
#include "singleton.h"
#pragma once

class PCStream : public Singleton<PCStream>{
public:
  PCStream(Stream *current) : Singleton<PCStream>(this), _current(current) {}
  inline Stream &current() { return *_current; }
  void set_current(Stream *stream) { this->_current = stream; }
private:
  Stream *_current;
};
