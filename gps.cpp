#include "gps.h"
//#include <DS1307RTC.h>
#include "processor.h"
#include "TimeLib.h"
#include "rtc.h"
#include "pc_stream.h"
#include "logger.h"

namespace {
  static const char sleepMessage[] = {0xB5, 0x62, 0x02, 0x41, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x4D, 0x3B};
}

GPS::GPS(HardwareSerial &port) : Singleton(this), _port(port)
{
}

void debug_gps_info(TinyGPSPlus &gps);

void GPS::sleep() {
  DEBUG() << F("Suspending GPS");
  for (uint8_t i = 0; i < sizeof(sleepMessage); i++)
    _port.write(sleepMessage[i]);
  delay(1000);
  _suspended = true;
}

void GPS::resume() {
  DEBUG() << F("Resuming GPS");
  delay(500);
  for (int i = 0; i < 10; i++)
    _port.write("\xFF");
  delay(500);
  _suspended = false;
}

void GPS::open() {
  DEBUG() << F("Opening GPS port");
  _port.begin(9600);
}

void GPS::close() {
  _port.end();
}

void GPS::process() {
  while (_port.available()) {
    auto c = _port.read();
    TRACE().no_newline().write(c);
    if(gps.encode(c)) {
      syncDateTime();
    }
  }
}

GPS::Fix GPS::get_fix() {
  if(! has_fix())
    return {}; 
  return {
    gps.location.lat(),
    gps.location.lng()
  };
}

void GPS::send_nmea(const String &command) {
  uint8_t cs = 0;
  for (auto c : command) {
    cs = cs ^ c;
  }
  _port.print("$");
  _port.print(command);
  _port.print("*");
  char lower = (cs & 0xF);
  char higher = (cs >> 4);

  _port.write(higher < 10 ? '0' + higher : 'A' + higher - 10);
  _port.write(lower < 10 ? '0' + lower : 'A' + lower - 10);
  _port.print("\r\n");
}

void GPS::syncDateTime() {
  
  if(! has_time() ) {
    return;
  }
  tmElements_t datetime {
    gps.time.second(),
    gps.time.minute(),
    gps.time.hour(),
    0, // TODO: weekday?
    gps.date.day(),
    gps.date.month(),
    static_cast<uint8_t>(gps.date.year() - 1970),
  };
  time_t t = makeTime(datetime);
  /*
  //Serial.print("UNIX time: "); Serial.println(t);
  RTC.set(t);
  setTime(t);
  */
  RTC::instance()->set_time(t);
}


