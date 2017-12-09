#include "singleton.h"
#include <Arduino.h>

#pragma once

class Buttons : public Singleton<Buttons> {
public:
  typedef void (*Callback)(void);
  Buttons();
  void setup(int pin);
  void click();
  void tick();
  enum Clicks { NoButton, SingleClick, DoubleClick, TripleClick, LongClick};
  void set_callback(Callback callback, Clicks type);
private:
  uint8_t pin;
  uint8_t _clicks = 0;
  uint32_t _last_down_millis = 0;
  uint32_t _last_millis = 0;
  Clicks _status = NoButton;
  Callback on_click = nullptr;
  Callback on_double_click = nullptr;
  Callback on_triple_click = nullptr;
  Callback on_longpress = nullptr;
};
