#include "logger.h"

uint8_t Logger::Log::usages = 0;
Logger::Log::Log(Print &s, bool enable) : s(s), enabled(enable), newline(true) {
  ++usages;
}

Logger::Log::Log(const Log &other) : s(other.s), enabled(other.enabled), newline(other.newline) {
  ++usages;
}

Logger::Log &Logger::Log::no_newline() {
  this->newline = false;
  return *this;
}

Logger::Log::~Log() {
  if(enabled && newline && --usages == 0) {
    s.println();
  }
}

Logger::Log &Logger::Log::write(uint8_t c) {
  if(enabled)
    s.write(c);
  return *this;
}

