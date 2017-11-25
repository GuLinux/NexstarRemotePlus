#include "rtc.h"
#include <uRTCLib.h>

// Function defined before importing Timelib, to avoid clashing with the dayOfWeek macro
uint8_t get_day_of_week(uRTCLib *rtc) {
  return rtc->dayOfWeek();
}

#include <TimeLib.h>


namespace {
  time_t syncProvider() {
    if(RTC::instance())
      return RTC::instance()->get_time_t();
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
  rtc->set_rtc_address(0x68);
  setSyncProvider(syncProvider);
  setSyncInterval(2);
//     rtc->set(0, 10, 20, 6, 10, 12, 10);

}

time_t RTC::get_time_t() {
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
  /*
  Serial.print(rtc->day()); Serial.print("/");
  Serial.print(rtc->month()); Serial.print("/");
  Serial.print(rtc->year()); Serial.print(" ");
  Serial.print(rtc->hour()); Serial.print(":");
  Serial.print(rtc->minute()); Serial.print(":");
  Serial.print(rtc->second()); Serial.print("\n");
  */
  return makeTime(time);
}

void RTC::set_time(time_t time) {
  Serial.print("Called set_time with "); Serial.println(time);
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

