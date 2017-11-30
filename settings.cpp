#include "settings.h"
#include <EEPROM.h>
#include "logger.h"

static const char *default_version = "v1.0.1";

Settings::Settings(): Singleton<Settings>(this) {
#ifndef RTC_WITH_EEPROM
  EEPROM.init();
#endif
}

template<typename T> uint16_t struct_size() {
  return (sizeof(T) + (sizeof(T)%2))/2;
}

template<typename T> uint16_t *get_buffer(T &t) {
  return reinterpret_cast<uint16_t*>(&t);
}

void Settings::load() {
#ifdef RTC_WITH_EEPROM
  DEBUG() << F("Loading settings from EEPROM");
  uint8_t *buffer = reinterpret_cast<uint8_t*>(&data); 
  for(uint16_t i=0; i < sizeof(Data); i++)
    buffer[i] = RTC::instance()->read_ee(i);
#else
  DEBUG() << F("Loading settings from flash");
  auto data_eeprom = get_buffer(data);
  for(uint16_t i=0; i<struct_size<Data>(); i++) {
    data_eeprom[i] = EEPROM.read(i);
  }
#endif
  if(String(data.version) != default_version) {
    DEBUG() << F("Settings empty, loading defaults");
    strcpy(data.version, default_version);
    strcpy(data.bluetooth_pin, "0000");
    strcpy(data.bluetooth_name, "NexstarBT");
    data.timezone = 0;
    data.daylight_saving = 0;
    data.gps_timeout = 5 * 60;
  }
  DEBUG() << F("Settings loaded");
}

void Settings::save() {
#ifdef RTC_WITH_EEPROM
  uint8_t *buffer = reinterpret_cast<uint8_t*>(&data); 
  for(uint16_t i=0; i < sizeof(Data); i++)
    RTC::instance()->update_ee(i, buffer[i]);
#else
  auto data_eeprom = get_buffer(data);
  for(uint16_t i=0; i<struct_size<Data>(); i++) {
    EEPROM.update(i, data_eeprom[i]);
  }
#endif
  load();
}


void Settings::set_string(char *dest, const char *msg, int field_size) {
  memset(dest, 0, field_size);
  strcpy(dest, msg);
}

