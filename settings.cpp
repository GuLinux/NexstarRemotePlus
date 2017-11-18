#include "settings.h"
#include <EEPROM.h>

static const char *default_version = "v1.0.0";

Settings::Settings() {
}

void Settings::load() {
  EEPROM.get(0, data);
  if(String(data.version) != default_version) {
    Serial.println("Settings empty, loading defaults");
    strcpy(data.version, default_version);
    strcpy(data.bluetooth_pin, "0000");
    strcpy(data.bluetooth_name, "NexstarBT");
    data.timezone = 0;
    data.daylight_saving = 0;
  }
}

void Settings::save() {
  EEPROM.put(0, data);
  load();
}


void Settings::set_string(char *dest, const char *msg, int field_size) {
  memset(dest, 0, field_size);
  strcpy(dest, msg);
}

