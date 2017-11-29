#include <Arduino.h>
#include "singleton.h"

#pragma once

class Logger : public Singleton<Logger> {
public:
  struct Log {
      Log(Print &s, bool enable) : s(s), enabled(enable) {}
      Log(Log &other) : s(other.s), enabled(other.enabled), newline(other.newline) { other.enabled = false; }
      Log(Log &&other) : s(other.s), enabled(other.enabled), newline(other.newline)  { other.enabled = false; }
      Log &no_newline() { this->newline = false; }
      ~Log() { if(enabled && newline) s.println(); }
      Print &s;
      bool enabled;
      bool newline=true;
  };

  inline Logger(Print &s) : Singleton<Logger>(this), s(s) {}
  enum Level { Trace=0, Debug=1, Info=2, Warning=3, Error=4 };
  inline void set_level(Level level) { this->_level = level; }
  Log log(Level level) { return Log(s, level >= this->_level); }
  Level level() const { return _level; }
private:
  Print &s;
  Level _level = Info;
};

#define TRACE() Logger::instance()->log(Logger::Trace)
#define DEBUG() Logger::instance()->log(Logger::Debug)
#define INFO() Logger::instance()->log(Logger::Info)
#define WARNING() Logger::instance()->log(Logger::Warning)
#define ERROR() Logger::instance()->log(Logger::Error)
#define LOG_I(i) static_cast<int>(i)

template<typename T>
Logger::Log operator<<(Logger::Log l, const T &msg) {
  if(l.enabled)
    l.s.print(msg);
  return l;
}
