#include <Arduino.h>
#include "singleton.h"
#pragma once

class PCStream : public Singleton<PCStream>, public Stream {
public:
  PCStream();
  Stream &current();
  enum Connection { USB, Bluetooth };
  Connection connection() const;

  virtual size_t write(uint8 ch) { return current().write(ch); }
  virtual size_t write(const char *str) { return current().write(str); }
  virtual size_t write(const void *buf, uint32 len) { return current().write(buf, len); }
  virtual int available() { return current().available(); }
  virtual int read() { return current().read(); }
  virtual int peek() { return current().peek(); }
  virtual void flush() { current().flush(); }


	
private:
};
