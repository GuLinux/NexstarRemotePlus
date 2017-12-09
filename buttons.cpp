#include "buttons.h"
#include "logger.h"

#define TIMEOUT 500
#define LONGPRESS_TIMEOUT 1000

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
  this->pin = pin;
  DEBUG() << F("Using pin ") << pin << F(" for buttons handling");
  pinMode(pin, INPUT_PULLDOWN);
  attachInterrupt(pin, buttonChanged, CHANGE);
}

void Buttons::click() {
  auto state = digitalRead(pin);
  if(state == 1) {
    _last_down_millis = millis();
  } else {
    _last_millis = millis();
    _clicks++;
  }
}

void Buttons::tick() {
  if(_last_millis == 0 || millis() - _last_millis < TIMEOUT) {
    _status = NoButton;
    return;
  }

  if(_last_millis - _last_down_millis > LONGPRESS_TIMEOUT) {
    run_callback(on_longpress);
  } else if(_clicks == 1) {
    run_callback(on_click);
  } else if(_clicks == 2) {
    run_callback(on_double_click);
  } else {
    run_callback(on_triple_click);
  }
  _last_down_millis = 0;
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
  else if(type == LongClick)
    on_longpress = callback;
}

