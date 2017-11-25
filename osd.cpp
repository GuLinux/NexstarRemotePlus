#include "osd.h"
#include "settings.h"
#include "processor.h"

#include "menu.h"

OSD::OSD() : Singleton<OSD>(this) {
}

void OSD::show_message(const String &message, int seconds) {
  this->message = message;
  this->message_expire = millis() + static_cast<uint32_t>(seconds * 1000);
}

void OSD::tick() {
  if(millis() > message_expire)
    message = String();
  if(millis() - last_button_press > 500 && button_presses > 0) {
      if(button_presses >=3) {
        current_entry = 0;
        button_presses = 0;
        return;
      }
      auto entry = entries[current_entry];
      auto code = button_presses == 2 ? entry.on_longpress : entry.on_shortpress;
      if(code >= ACTIONS_BASE) {
        action(code);
        code = 0;
      }
    current_entry = code;
    button_presses = 0;
  }
}

void OSD::render(Print &print) {
  if(message.length() > 0) {
    print.println(message);
  }
  print.print(entries[current_entry].label);
}

void OSD::button_pressed() {
  message = "";
  button_presses++;
  last_button_press = millis();
}

void OSD::action(uint8_t code) {
  auto set_gps_timeout = [this](int minutes) {
    Settings::instance()->gps_timeout(60 * minutes);
    show_message(String("GPS Timeout set to 1 minute"));
  };
  switch(code) {
    case(ACTION_GPS_FIX): Processor::instance()->request_gps_fix(); break;
    case(ACTION_SET_GPS_TIMEOUT_1M): set_gps_timeout(1); break;
    case(ACTION_SET_GPS_TIMEOUT_2M): set_gps_timeout(2); break;
    case(ACTION_SET_GPS_TIMEOUT_5M): set_gps_timeout(5); break;
    case(ACTION_SET_GPS_TIMEOUT_10M): set_gps_timeout(10); break;
    case(ACTION_SET_DAYLIGHT_SAVING_OFF): Settings::instance()->daylight_saving(0); show_message("Daylight saving set to OFF"); break;
    case(ACTION_NEXSTAR_SYNC): Processor::instance()->request_nexstar_sync(); break;
    case(ACTION_SET_DAYLIGHT_SAVING_ON): Settings::instance()->daylight_saving(1); show_message("Daylight saving set to ON"); break;
    case(ACTION_SET_TIMEZONE_GMT_M12): Settings::instance()->timezone(-12); show_message("Timezone set to GMT-12"); break;
    case(ACTION_SET_TIMEZONE_GMT_M11): Settings::instance()->timezone(-11); show_message("Timezone set to GMT-11"); break;
    case(ACTION_SET_TIMEZONE_GMT_M10): Settings::instance()->timezone(-10); show_message("Timezone set to GMT-10"); break;
    case(ACTION_SET_TIMEZONE_GMT_M09): Settings::instance()->timezone(-9); show_message("Timezone set to GMT-9"); break;
    case(ACTION_SET_TIMEZONE_GMT_M08): Settings::instance()->timezone(-8); show_message("Timezone set to GMT-8"); break;
    case(ACTION_SET_TIMEZONE_GMT_M07): Settings::instance()->timezone(-7); show_message("Timezone set to GMT-7"); break;
    case(ACTION_SET_TIMEZONE_GMT_M06): Settings::instance()->timezone(-6); show_message("Timezone set to GMT-6"); break;
    case(ACTION_SET_TIMEZONE_GMT_M05): Settings::instance()->timezone(-5); show_message("Timezone set to GMT-5"); break;
    case(ACTION_SET_TIMEZONE_GMT_M04): Settings::instance()->timezone(-4); show_message("Timezone set to GMT-4"); break;
    case(ACTION_SET_TIMEZONE_GMT_M03): Settings::instance()->timezone(-3); show_message("Timezone set to GMT-3"); break;
    case(ACTION_SET_TIMEZONE_GMT_M02): Settings::instance()->timezone(-2); show_message("Timezone set to GMT-2"); break;
    case(ACTION_SET_TIMEZONE_GMT_M01): Settings::instance()->timezone(-1); show_message("Timezone set to GMT-1"); break;
    case(ACTION_SET_TIMEZONE_GMT_P00): Settings::instance()->timezone(0); show_message("Timezone set to GMT+0"); break;
    case(ACTION_SET_TIMEZONE_GMT_P01): Settings::instance()->timezone(1); show_message("Timezone set to GMT+1"); break;
    case(ACTION_SET_TIMEZONE_GMT_P02): Settings::instance()->timezone(2); show_message("Timezone set to GMT+2"); break;
    case(ACTION_SET_TIMEZONE_GMT_P03): Settings::instance()->timezone(3); show_message("Timezone set to GMT+3"); break;
    case(ACTION_SET_TIMEZONE_GMT_P04): Settings::instance()->timezone(4); show_message("Timezone set to GMT+4"); break;
    case(ACTION_SET_TIMEZONE_GMT_P05): Settings::instance()->timezone(5); show_message("Timezone set to GMT+5"); break;
    case(ACTION_SET_TIMEZONE_GMT_P06): Settings::instance()->timezone(6); show_message("Timezone set to GMT+6"); break;
    case(ACTION_SET_TIMEZONE_GMT_P07): Settings::instance()->timezone(7); show_message("Timezone set to GMT+7"); break;
    case(ACTION_SET_TIMEZONE_GMT_P08): Settings::instance()->timezone(8); show_message("Timezone set to GMT+8"); break;
    case(ACTION_SET_TIMEZONE_GMT_P09): Settings::instance()->timezone(9); show_message("Timezone set to GMT+9"); break;
    case(ACTION_SET_TIMEZONE_GMT_P10): Settings::instance()->timezone(10); show_message("Timezone set to GMT+10"); break;
    case(ACTION_SET_TIMEZONE_GMT_P11): Settings::instance()->timezone(11); show_message("Timezone set to GMT+11"); break;
    case(ACTION_SET_TIMEZONE_GMT_P12): Settings::instance()->timezone(12); show_message("Timezone set to GMT+12"); break;
  }
}

