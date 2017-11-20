#include <Arduino.h>
#pragma once

class PCStream {
public:
  PCStream(Stream &current) : _current(current) {}
  inline Stream &current() { return _current; }
  void set_current(Stream &stream) { this->_current = stream; }
private:
  Stream &_current;
};
