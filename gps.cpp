#include "gps.h"
//#include <DS1307RTC.h>
#include "processor.h"
#include "TimeLib.h"
#include "rtc.h"

GPS::GPS(HardwareSerial &port) : Singleton(this), _port(port)
{
}

void displayGPSInfo(TinyGPSPlus &gps, Stream &stream);

void GPS::sleep() {
  static const char sleepMessage[] = {0xB5, 0x62, 0x02, 0x41, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x4D, 0x3B};
  for (uint8_t i = 0; i < sizeof(sleepMessage); i++)
    _port.write(sleepMessage[i]);
  delay(1000);
}

void GPS::resume() {
  delay(500);
  for (int i = 0; i < 10; i++)
    _port.write("\xFF");
  delay(500);
}

void GPS::open() {
  _port.begin(9600);
//  send_nmea("PUBX,41,1,3,3,115200,0");
//  _port.end();
//  _port.begin(57600);

}

void GPS::close() {
  _port.end();
}

bool GPS::wait_for_fix(uint16_t timeout_sec) {
  auto start = millis();
  auto expired = [&start, &timeout_sec] { return static_cast<unsigned long>(millis() - start) >= static_cast<unsigned long>(timeout_sec) * 1000l; };

  while ( ! expired() && ! has_fix() ) {
    while (_port.available() && ! expired()) {
      gps.encode(_port.read());
      //if ();
        //displayGPSInfo(gps, Serial);
    }
  }
  //displayGPSInfo(gps, Serial);
  syncDateTime();
  if(has_time()) {
    Processor::instance()->request_nexstar_sync();
  }
  return has_fix();
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

void GPS::debug(Stream &stream, bool raw) {
  while(true) {
      if(stream.available()) {
        if(stream.readString() == "stop")
          return;
      }
      while (_port.available()) {
        int c = _port.read();
        if(raw)
          stream.write(c);
        if ( ! raw && gps.encode(c)) {
          displayGPSInfo(gps, stream);
        }
        syncDateTime();
    }
  }
}

void GPS::syncDateTime() {
  
  if(! gps.date.isValid() || ! gps.time.isValid() || gps.date.year() < 2017) { // hardcoded number, ok, but let's assume for a second we're not time traveling...
    return;
  }
  tmElements_t datetime {
    gps.time.second(),
    gps.time.minute(),
    gps.time.hour(),
    0, // TODO: weekday?
    gps.date.day(),
    gps.date.month(),
    gps.date.year() - 1970,
  };
  time_t t = makeTime(datetime);
  /*
  //Serial.print("UNIX time: "); Serial.println(t);
  RTC.set(t);
  setTime(t);
  */
  RTC::instance()->set_time(t);
}

void displayGPSInfo(TinyGPSPlus &gps, Stream &stream)
{
  stream.print(F("Location: "));
  if (gps.location.isValid())
  {
    stream.print(gps.location.lat(), 6);
    stream.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }
  else
  {
    stream.print(F("INVALID"));
  }

  stream.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    stream.print(gps.date.month());
    stream.print(F("/"));
    stream.print(gps.date.day());
    stream.print(F("/"));
    stream.print(gps.date.year());
    stream.print(" value="); stream.print(gps.date.value());
  }
  else
  {
    stream.print(F("INVALID"));
  }

  stream.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) stream.print(F("0"));
    stream.print(gps.time.hour());
    stream.print(F(":"));
    if (gps.time.minute() < 10) stream.print(F("0"));
    stream.print(gps.time.minute());
    stream.print(F(":"));
    if (gps.time.second() < 10) stream.print(F("0"));
    stream.print(gps.time.second());
    stream.print(F("."));
    if (gps.time.centisecond() < 10) stream.print(F("0"));
    stream.print(gps.time.centisecond());
    stream.print(" time="); stream.print(gps.time.value());
  }
  else
  {
    stream.print(F("INVALID"));
  }
  stream.print(F("; sats: ")); stream.println(gps.satellites.value());
  stream.println();
}
