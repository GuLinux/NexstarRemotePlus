#include "processor.h"
#include "nexstar.h"
#include "gps.h"
#include "bluetooth.h"
#include "commands.h"
#include "settings.h"
#include "pc_stream.h"
#include "osd.h"
#include "display.h"
#include "logger.h"
#include "battery.h"
#include "rtc.h"

#define GPS_FIXES_EVERY 10000

Processor::Processor() : Singleton<Processor>(this) {
}

void Processor::setup() {
  set_gps_expire();
  request_nexstar_sync();
}

void debug_status() {
  static uint32_t last_dbg = 0;
  if(millis() - last_dbg < 2000)
    return;
  last_dbg = millis();
  DEBUG().no_newline() << F("\r\n* STATUS: GPS: ");
  if(GPS::instance()->has_fix()) {
    auto fix = GPS::instance()->get_fix();
    DEBUG().no_newline() << F("lat=") << fix.latitude << F(", lng=") << fix.longitude;
  } else {
    DEBUG().no_newline() << F("N/A");
  }
  DEBUG().no_newline() << F(", GPS time: ") << (GPS::instance()->has_time() ? 'Y' : 'N');
  DEBUG().no_newline() << F(", last fix: lat=") << Settings::instance()->last_fix().latitude << F(", lng=") << Settings::instance()->last_fix().longitude ;
  DEBUG().no_newline() << F(", vbat=") << Battery::instance()->millivolts() << F(", ") << Battery::instance()->percentage()
    << F(", RTC: UTC=") << RTC::instance()->utc() << F(", local=") << RTC::instance()->localtime();
  DEBUG().no_newline().write('\r').write('\n');
}


void Processor::loop() {
  GPS::instance()->process();
  gps_fix_management();
  gps_power_management();

  nexstar_sync();
  Commands::instance()->read();
  Nexstar::instance()->write(Commands::instance()->buffer(), Commands::instance()->buffer_len());
  Nexstar::instance()->read_to(*PCStream::instance());
  OSD::instance()->tick();
  Display::instance()->update();
  debug_status();
}


void Processor::gps_power_management() {
  if(_gps_wakeup_requested && GPS::instance()->suspended() ) {
    last_fixes_cnt = 0;
    OSD::instance()->show_message("Wake up\nGPS");
    GPS::instance()->resume();
    set_gps_expire();
    _gps_wakeup_requested = false;
    return;
  }
  bool has_enough_fixes = last_fixes_cnt >= GPS_FIXES_AVERAGE_COUNT;
  bool gps_timeout_expired = _gps_expire > 0 && millis() > _gps_expire;

  if(! GPS::instance()->suspended() && ( has_enough_fixes || gps_timeout_expired )  )  {
    TRACE() << F("Suspending GPS: has_enough_fixes=") << has_enough_fixes << F(", gps_timeout_expired=") << gps_timeout_expired;
    GPS::instance()->sleep();
  }
}

GPS::Fix get_average_fix(GPS::Fix *fixes, uint8_t count) {
  if(count == 0) {
    return {};
  }
  GPS::Fix average{0, 0};
  for(uint8_t i=0; i<count; i++) {
    TRACE() << F("get_average_fix(count=") << count << F("): last_fixes[") << i << F("] = ") << fixes[i].latitude << F(", ") << fixes[i].longitude;
    average += fixes[i];
  }
  average /= count;
  DEBUG() << "Average of " << count << " fixes: " << average.latitude << ", " << average.longitude;
  return average;
}

void Processor::gps_fix_management() {
  if(! GPS::instance()->has_fix())
    return;
  if(last_fixes_cnt >= GPS_FIXES_AVERAGE_COUNT)
    return; // We got enough gps fixes already
  if(millis() - _gps_last_fix < GPS_FIXES_EVERY)
    return; // Collect fixes every GPS_FIXES_EVERY
  //if(last_fixes_cnt == 0 && _gps_expire != 0)
  //  _gps_expire += ((GPS_FIXES_AVERAGE_COUNT+2) * GPS_FIXES_EVERY); // give GPS enough time to collect GPS_FIXES_AVERAGE_COUNT samples
  last_fixes[last_fixes_cnt++] = GPS::instance()->get_fix();
  _gps_last_fix = millis();
  auto fix = get_average_fix(last_fixes, last_fixes_cnt);
  Settings::instance()->last_fix(fix);
  Nexstar::instance()->set_gps_info();
}

void Processor::set_gps_expire() {
  auto gps_timeout = Settings::instance()->gps_timeout().value;
  if(gps_timeout > 0) {
    _gps_expire = static_cast<uint32_t>(gps_timeout) * 1000 + millis();
  } else {
    _gps_expire = 0;
  }
  TRACE() << millis() << F("- GPS timeout=") << gps_timeout << F(", expire set to ") << _gps_expire;
}

void Processor::nexstar_sync() {
  if(! _sync_nexstar)
    return;
  if(! Nexstar::instance()->set_time() || ! Nexstar::instance()->set_gps_info() ) {
    OSD::instance()->show_message("Error on\nNexstar Sync");
  } else {
    OSD::instance()->show_message("Nexstar\nSync: OK", 10);
  }
  _sync_nexstar = false;
}

