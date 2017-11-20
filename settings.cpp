#include "settings.h"
#include <EEPROM.h>

static const char *default_version = "v1.0.1";

Settings::Settings() {
  EEPROM.init();
}

template<typename T> uint16_t struct_size() {
  return (sizeof(T) + (sizeof(T)%2))/2;
}

template<typename T> uint16_t *get_buffer(T &t) {
  return reinterpret_cast<uint16_t*>(&t);
}

void Settings::load() {
  auto data_eeprom = get_buffer(data);
  for(uint16_t i=0; i<struct_size<Data>(); i++) {
    data_eeprom[i] = EEPROM.read(i);
  }
  //EEPROM.get(0, data);
  if(String(data.version) != default_version) {
    Serial.println(F("Settings empty, loading defaults"));
    strcpy(data.version, default_version);
    strcpy(data.bluetooth_pin, "0000");
    strcpy(data.bluetooth_name, "NexstarBT");
    data.timezone = 0;
    data.daylight_saving = 0;
    data.gps_timeout = 5 * 60;
  }
}

void Settings::save() {
  auto data_eeprom = get_buffer(data);
  for(uint16_t i=0; i<struct_size<Data>(); i++) {
    EEPROM.update(i, data_eeprom[i]);
  }
  load();
}


void Settings::set_string(char *dest, const char *msg, int field_size) {
  memset(dest, 0, field_size);
  strcpy(dest, msg);
}

