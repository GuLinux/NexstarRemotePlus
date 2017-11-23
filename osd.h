#include <Arduino.h>
#include "singleton.h"

#pragma once

struct MenuEntry;
class OSD : public Singleton<OSD> {
public:
  OSD();
  void show_message(const String &message, int seconds=10);
  void tick();
  void render(Print &print);
  void button_pressed();

private:
  uint8_t current_entry = 0;
  String message;
  uint32_t message_expire = 0;
  uint32_t last_button_press = 0;
  uint8_t button_presses = 0;
  void action(uint8_t code);
};

