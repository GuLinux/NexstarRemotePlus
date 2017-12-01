#include <Arduino.h>
#include "singleton.h"

#pragma once
//#define DISABLE_LOGGER

class Logger : public Singleton<Logger> {
public:
#ifndef DISABLE_LOGGER
  struct Log {
      Log(Print &s, bool enable);
      Log(const Log &other);
      Log &no_newline();
      Log &write(uint8_t c);
      ~Log();
      Print &s;
      bool enabled;
      bool newline;
  private:
      static uint8_t usages;
  };
#else
  struct Log {
    Log &no_newline() { return *this; }
  };
#endif

  inline Logger(Print &s) : Singleton<Logger>(this), s(s) {}
  enum Level { Trace=0, Debug=1, Info=2, Warning=3, Error=4 };
  inline void set_level(Level level) { this->_level = level; }
  Log log(Level level) {
#ifndef DISABLE_LOGGER
    return Log(s, level >= this->_level);
#else
    return Log();
#endif
  }
  Level level() const { return _level; }
private:
  Print &s;
  Level _level = Trace;
};



#define TRACE() Logger::instance()->log(Logger::Trace)
#define DEBUG() Logger::instance()->log(Logger::Debug)
#define INFO() Logger::instance()->log(Logger::Info)
#define WARNING() Logger::instance()->log(Logger::Warning)
#define ERROR() Logger::instance()->log(Logger::Error)
#define LOG_I(i) static_cast<int>(i)

template<typename T>
Logger::Log operator<<(const Logger::Log &l, const T &msg) {
#ifndef DISABLE_LOGGER
  if(l.enabled) {
    l.s.print(msg);
  }
#endif
  return l;
}
