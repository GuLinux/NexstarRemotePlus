#include "gps.h"
//#include <DS1307RTC.h>
#include "processor.h"
#include "TimeLib.h"
#include "rtc.h"
#include "pc_stream.h"
#include "logger.h"

GPS::GPS(HardwareSerial &port) : Singleton(this), _port(port)
{
}

void debug_gps_info(TinyGPSPlus &gps);

void GPS::sleep() {
  static const char sleepMessage[] = {0xB5, 0x62, 0x02, 0x41, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x4D, 0x3B};
  for (uint8_t i = 0; i < sizeof(sleepMessage); i++)
    _port.write(sleepMessage[i]);
  delay(1000);
  _suspended = true;
}

void GPS::resume() {
  delay(500);
  for (int i = 0; i < 10; i++)
    _port.write("\xFF");
  delay(500);
  _suspended = false;
}

void GPS::open() {
  _port.begin(9600);
}

void GPS::close() {
  _port.end();
}

void GPS::process() {
  while (_port.available()) {
    auto c = _port.read();
    TRACE().no_newline() << c;
    if(gps.encode(c)) {
      debug_gps_info(gps);
    }
  }
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
  Serial.print("UNIX time: "); Serial.println(t);
  /*
  //Serial.print("UNIX time: "); Serial.println(t);
  RTC.set(t);
  setTime(t);
  */
  RTC::instance()->set_time(t);
}

void debug_gps_info(TinyGPSPlus &gps)
{
  auto dbg = DEBUG();
  dbg << F("Location: ");
  if (gps.location.isValid())
  {
    dbg << gps.location.lat() << F(", ") << gps.location.lng();
  }
  else
  {
    dbg << F("INVALID");
  }

  dbg << F(", date: ");
  if (gps.date.isValid())
  {
    char date[11];
    sprintf(date, "%02d/%02d/%04d", gps.date.day(), gps.date.month(), gps.date.year());
    dbg << date;
  }
  else
  {
    dbg << F("INVALID");
  }

  dbg << F(", time: ");
  if (gps.time.isValid())
  {
    char time[12];
    sprintf(time, "%02d:%02d:%02d.%02d", gps.time.hour(), gps.time.minute(), gps.time.second(), gps.time.centisecond());
    dbg << time;
  }
  else
  {
    dbg << F("INVALID");
  }
  dbg << F("; satellites: ") << gps.satellites.value();
}

