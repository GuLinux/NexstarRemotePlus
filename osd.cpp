#include "osd.h"
#include "settings.h"
#include "processor.h"
#include "buttons.h"
#include "menu.h"


namespace {
  void set_gps_timeout(uint32_t minutes) {
    Settings::instance()->gps_timeout(60 * minutes);
    if(minutes > 0) {
      OSD::instance()->show_message(String("GPS Timeout set to ") + minutes + " minutes");
    } else {
      OSD::instance()->show_message(String("GPS set to no timeout "));
    }
    Processor::instance()->set_gps_expire();
  }

  // We assume that GMT action codes are contiguous
  void set_timezone(uint8_t code) {
    int16_t delta = code - ACTION_SET_TIMEZONE_GMT_P00;
    char message[23];
    uint8_t sign = (delta >= 0 ? '+' : '-');
    sprintf(message, "Timezone set to GMT%c%02d", sign, delta);
    OSD::instance()->show_message(message);
    Settings::instance()->timezone(delta);
  }

  void set_daylight_saving(bool on) {
    Settings::instance()->daylight_saving(on ? 1 : 0);
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
}



OSD::OSD() : Singleton<OSD>(this) {
  Buttons::instance()->set_callback(::on_click, Buttons::SingleClick);
  Buttons::instance()->set_callback(::on_double_click, Buttons::DoubleClick);
  Buttons::instance()->set_callback(::on_triple_click, Buttons::TripleClick);
}

void OSD::show_message(const String &message, int seconds) {
  this->message = message;
  this->message_expire = millis() + static_cast<uint32_t>(seconds * 1000);
}

void OSD::tick() {
  if(millis() > message_expire)
    message = String();
}

void OSD::render(Print &print) {
/*
  static uint8_t last_menu = 0;
  if(last_menu != _menu_index) {
    Serial.print("Rendering menu: "); Serial.print(_menu_index); Serial.print(", `"); Serial.print(menu_entry().label); Serial.print("`, on_click=");
    Serial.print(menu_entry().on_click); Serial.print(", on_double_click="); Serial.println(menu_entry().on_double_click);
    last_menu = _menu_index;
  }
*/
  if(message.length() > 0) {
    print.println(message);
  }
  print.print(menu_entry().label);
}

void OSD::on_click() {
  _menu_index = menu_entry().on_click;
  clear_message();
}

void OSD::on_double_click() {
  clear_message();
  auto entry = menu_entry();
  _menu_index = 0;
  uint8_t code = entry.on_double_click;
  if(code >= ACTIONS_BASE) {
    action(code);
  } else {
    _menu_index = code;
  }
}

void OSD::on_triple_click() {
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

