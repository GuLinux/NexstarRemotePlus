#include <Arduino.h>
#include "singleton.h"
#include "menu.h"

#pragma once

class OSD : public Singleton<OSD> {
public:
  OSD();
  void show_message(const String &message, int seconds=10);
  void tick();
  bool render(Print &print);
  void on_click();
  void on_double_click();
  void on_triple_click();
  inline void clear_message() { message = String(); message_expire = 0; }
private:
  uint8_t _menu_index = 0;
  String message;
  uint32_t message_expire = 0;
  void action(uint8_t code);
  MenuEntry menu_entry() const;
};

