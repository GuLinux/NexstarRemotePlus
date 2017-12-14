#include "settings.h"
#include "logger.h"
#include "rtc.h"
#include "menu.h"

static const char *default_version = "v1.0.8";

Settings::Settings(): Singleton<Settings>(this) {
}


void Settings::load() {
  delay(100);
  TRACE() << F("Loading settings from EEPROM");
  uint8_t *buffer = reinterpret_cast<uint8_t*>(&data); 
  for(uint16_t i=0; i < sizeof(Data); i++)
    buffer[i] = RTC::instance()->read_ee(i);
  if(strcmp(data.version, default_version) != 0) {
    DEBUG() << F("Settings empty, loading defaults");
    strcpy(data.version, default_version);
    strcpy(data.bluetooth_pin, "0000");
    strcpy(data.bluetooth_name, "NexstarBT");
    data.timezone = {0, MENU_SET_TIMEZONE_GMT_P00};
    data.daylight_saving = {0, MENU_SET_DAYLIGHT_SAVING_OFF};
    data.gps_timeout = {0, MENU_GPS_TIMEOUT_INFINITE};
    data.log_level = Logger::Info;
  }
  TRACE() << F("Settings loaded");
}

void Settings::save() {
  uint8_t *buffer = reinterpret_cast<uint8_t*>(&data); 
  for(uint16_t i=0; i < sizeof(Data); i++)
    RTC::instance()->update_ee(i, buffer[i]);
  delay(100);
  load();
}

void Settings::bluetooth_pin(const char *pin) {
  strncpy(data.bluetooth_pin, pin, BT_PIN_SIZE);
  save();
}

void Settings::bluetooth_name(const char *name) {
  strncpy(data.bluetooth_name, name, BT_NAME_SIZE);
  save();
}

void Settings::set_string(char *dest, const char *msg, int field_size) {
  strncpy(dest, msg, field_size);
  dest[field_size] = 0;
  DEBUG() << "got string: " << msg << ", dest: " << dest << ", field_size: " << field_size;
}

