#include "buttons.h"

#define TIMEOUT 500

namespace {
  void run_callback(Buttons::Callback callback) {
    if(callback)
      callback();
  }
}

void buttonChanged() {
  Buttons::instance()->click();
}

Buttons::Buttons() : Singleton<Buttons>(this) {
}

void Buttons::setup(int pin) {
  pinMode(pin, INPUT);
  attachInterrupt(pin, buttonChanged, FALLING);
}

void Buttons::click() {
  _clicks++;
  _last_millis = millis();
}

void Buttons::tick() {
  if(_last_millis == 0 || millis() - _last_millis < TIMEOUT) {
    _status = NoButton;
    return;
  }

  if(_clicks == 1) {
    run_callback(on_click);
  } else if(_clicks == 2) {
    run_callback(on_double_click);
  } else {
    run_callback(on_triple_click);
  }
  _last_millis = 0;
  _clicks = 0;
}

void Buttons::set_callback(Callback callback, Clicks type) {
  if(type == SingleClick)
    on_click = callback;
  else if(type == DoubleClick)
    on_double_click = callback;
  else if(type == TripleClick)
    on_triple_click = callback;
}

