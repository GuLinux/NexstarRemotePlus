#include "osd.h"
#include "settings.h"
#include "processor.h"
#include "buttons.h"
#include "menu.h"


namespace {
  void set_gps_timeout(uint32_t minutes) {
    Settings::MenuSetting<uint16_t> setting;
    switch(minutes) {
      case 5:
        setting.value = 5 * 60;
        setting.uid = MENU_GPS_TIMEOUT_5M;
        break;
      case 10:
        setting.value = 10 * 60;
        setting.uid = MENU_GPS_TIMEOUT_10M;
        break;
      case 20:
        setting.value = 2 * 60;
        setting.uid = MENU_GPS_TIMEOUT_20M;
        break;
      case 40:
        setting.value = 4 * 60;
        setting.uid = MENU_GPS_TIMEOUT_40M;
        break;
      default:
        setting.value = 0;
        setting.uid = MENU_GPS_TIMEOUT_INFINITE;
        break;

    }
    TRACE() << F("set_gps_timeout: minutes=") << minutes << F(", setting.value=") << setting.value << F(", setting.uid=") << setting.uid;
    Settings::instance()->gps_timeout(setting);
    if(setting.value> 0) {
      OSD::instance()->show_message(String("GPS Timeout set to ") + setting.value/60 + " minutes");
    } else {
      OSD::instance()->show_message(String("GPS set to no timeout "));
    }
    Processor::instance()->set_gps_expire();
  }

  // We assume that GMT action codes are contiguous
  void set_timezone(uint8_t code) {
    int8_t delta = static_cast<uint8_t>(code) - static_cast<uint8_t>(ACTION_SET_TIMEZONE_GMT_P00);
    DEBUG() << "Got code: " << code << ", delta: " << delta << ", P00=" << ACTION_SET_TIMEZONE_GMT_P00;
    char message[23];
    uint8_t sign = (delta >= 0 ? '+' : '-');
    sprintf(message, "Timezone set to GMT%c%02d", sign, delta);
    OSD::instance()->show_message(message);
    uint8_t menu_uid = MENU_SET_TIMEZONE_GMT_P00 + delta;
    Settings::instance()->timezone({delta, menu_uid});
  }

  void set_daylight_saving(bool on) {
    Settings::MenuSetting<int8_t> dst {0, MENU_SET_DAYLIGHT_SAVING_OFF};
    if(on) {
      dst.value = 1;
      dst.uid = MENU_SET_DAYLIGHT_SAVING_ON;
    }
    Settings::instance()->daylight_saving(dst);
    String message("Daylight saving: ");
    message += on ? "ON" : "OFF";
    OSD::instance()->show_message(message);
  }
  
  void on_click() {
    OSD::instance()->on_click();
  }

  void on_double_click() {
    OSD::instance()->on_double_click();
  }

  void on_triple_click() {
    OSD::instance()->on_triple_click();
  }

  void on_long_click() {
    OSD::instance()->on_long_click();
  }

}



OSD::OSD() : Singleton<OSD>(this) {
  Buttons::instance()->set_callback(::on_click, Buttons::SingleClick);
  Buttons::instance()->set_callback(::on_double_click, Buttons::DoubleClick);
  Buttons::instance()->set_callback(::on_triple_click, Buttons::TripleClick);
  Buttons::instance()->set_callback(::on_long_click, Buttons::LongClick);
}

void OSD::show_message(const String &message, int seconds) {
  this->message = message;
  this->message_expire = millis() + static_cast<uint32_t>(seconds * 1000);
}

void OSD::tick() {
  if(millis() > message_expire)
    message = String();
}

String OSD::render() {
  if(message.length() > 0) {
    return message;
  }
  auto entry = menu_entry();
  String label = entry.label;
  if( (entry.parent == MENU_GPS_TIMEOUT && Settings::instance()->gps_timeout().uid == entry.id) ||
      (entry.parent == MENU_SET_DAYLIGHT_SAVING && Settings::instance()->daylight_saving().uid == entry.id) ||
      (entry.parent == MENU_SET_TIMEZONE && Settings::instance()->timezone().uid == entry.id) )
    label += " *";
  return label;
}

void OSD::on_click() {
  TRACE() << F("on_click: entry=") << menu_entry().id << F(", on_next=") << menu_entry().on_next;
  _menu_index = menu_entry().on_next;
  clear_message();
}

void OSD::on_triple_click() {
  TRACE() << F("on_triple_click: entry=") << menu_entry().id << F(", on_back=") << menu_entry().on_back;
  _menu_index = menu_entry().on_back;
  clear_message();
}

void OSD::on_double_click() {
  clear_message();
  auto entry = menu_entry();
  _menu_index = 0;
  uint8_t code = entry.on_enter;
  TRACE() << F("on_double_click: entry=") << entry.id << F(", on_enter=") << static_cast<int>(code);
  if(code >= ACTIONS_BASE) {
    action(code);
  } else {
    if(entry.id == MENU_GPS_TIMEOUT)
      code = Settings::instance()->gps_timeout().uid;
    if(entry.id == MENU_SET_DAYLIGHT_SAVING)
      code = Settings::instance()->daylight_saving().uid;
    if(entry.id == MENU_SET_TIMEZONE)
      code = Settings::instance()->timezone().uid;
    _menu_index = code;
  }
}

void OSD::on_long_click() {
  TRACE() << F("on_long_click");
  _menu_index = 0;
  clear_message();
}

MenuEntry OSD::menu_entry() const {
  if(_menu_index < len_entries)
    return entries[_menu_index];
  return entries[0];
}

void OSD::action(uint8_t code) {
  switch(code) {
    case(ACTION_GPS_WAKEUP): Processor::instance()->request_gps_wakeup(); break;
    case(ACTION_SET_GPS_TIMEOUT_5M): set_gps_timeout(5); break;
    case(ACTION_SET_GPS_TIMEOUT_10M): set_gps_timeout(10); break;
    case(ACTION_SET_GPS_TIMEOUT_20M): set_gps_timeout(20); break;
    case(ACTION_SET_GPS_TIMEOUT_40M): set_gps_timeout(40); break;
    case(ACTION_SET_GPS_TIMEOUT_INFINITE): set_gps_timeout(0); break;
    case(ACTION_NEXSTAR_SYNC): Processor::instance()->request_nexstar_sync(); break;
    case(ACTION_SET_DAYLIGHT_SAVING_OFF):
    case(ACTION_SET_DAYLIGHT_SAVING_ON): set_daylight_saving(code == ACTION_SET_DAYLIGHT_SAVING_ON); break;
    case(ACTION_SET_TIMEZONE_GMT_M12):
    case(ACTION_SET_TIMEZONE_GMT_M11):
    case(ACTION_SET_TIMEZONE_GMT_M10):
    case(ACTION_SET_TIMEZONE_GMT_M09):
    case(ACTION_SET_TIMEZONE_GMT_M08):
    case(ACTION_SET_TIMEZONE_GMT_M07):
    case(ACTION_SET_TIMEZONE_GMT_M06):
    case(ACTION_SET_TIMEZONE_GMT_M05):
    case(ACTION_SET_TIMEZONE_GMT_M04):
    case(ACTION_SET_TIMEZONE_GMT_M03):
    case(ACTION_SET_TIMEZONE_GMT_M02):
    case(ACTION_SET_TIMEZONE_GMT_M01):
    case(ACTION_SET_TIMEZONE_GMT_P00):
    case(ACTION_SET_TIMEZONE_GMT_P01):
    case(ACTION_SET_TIMEZONE_GMT_P02):
    case(ACTION_SET_TIMEZONE_GMT_P03):
    case(ACTION_SET_TIMEZONE_GMT_P04):
    case(ACTION_SET_TIMEZONE_GMT_P05):
    case(ACTION_SET_TIMEZONE_GMT_P06):
    case(ACTION_SET_TIMEZONE_GMT_P07):
    case(ACTION_SET_TIMEZONE_GMT_P08):
    case(ACTION_SET_TIMEZONE_GMT_P09):
    case(ACTION_SET_TIMEZONE_GMT_P10):
    case(ACTION_SET_TIMEZONE_GMT_P11):
    case(ACTION_SET_TIMEZONE_GMT_P12):
      set_timezone(code); break;
  }
}

