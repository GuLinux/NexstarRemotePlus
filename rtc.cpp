#include "rtc.h"
#include <uRTCLib.h>
#include "logger.h"
#include "settings.h"

// Function defined before importing Timelib, to avoid clashing with the dayOfWeek macro
uint8_t get_day_of_week(uRTCLib *rtc) {
  return rtc->dayOfWeek();
}

#include <TimeLib.h>


namespace {
  time_t syncProvider() {
    if(RTC::instance()) {
      return RTC::instance()->localtime();
    }
    return 0;
  }
}

RTC::RTC() : Singleton<RTC>(this) {
  rtc = new uRTCLib();
}

RTC::~RTC() {
  delete rtc;
}


void RTC::setup() {
  TRACE() << F("Setting up RTC time provider");
  rtc->set_rtc_address(0x68);
  rtc->set_ee_address(0x50);
  setSyncProvider(syncProvider);
  setSyncInterval(2);
}

time_t RTC::utc() {
  rtc->refresh();
  TimeElements time{
    rtc->second(),
    rtc->minute(),
    rtc->hour(),
    get_day_of_week(rtc),
    rtc->day(),
    rtc->month(),
    static_cast<uint8_t>(y2kYearToTm(rtc->year())),
  };
  return makeTime(time);
}

time_t RTC::localtime() {
  return utc() + (Settings::instance()->timezone().value + Settings::instance()->daylight_saving().value) * 3600;
}

void RTC::set_time(time_t time) {
  TRACE() << F("Called set_time with ") << time;
  TimeElements time_elements;
  breakTime(time, time_elements);
  rtc->set(time_elements.Second, time_elements.Minute, time_elements.Hour, time_elements.Wday, time_elements.Day, time_elements.Month, tmYearToY2k(time_elements.Year));
}

void RTC::write_ee(const uint32_t address, const uint8_t value) {
  rtc->eeprom_write(address, value);
}

uint8_t RTC::read_ee(const uint32_t address) {
  return rtc->eeprom_read(address);
}

void RTC::update_ee(const uint32_t address, const uint8_t value) {
  if(read_ee(address) != value)
    write_ee(address, value);
}

