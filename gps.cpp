#include "gps.h"
#include <DS1307RTC.h>
#include "nexstar.h"

GPS::GPS(int rx, int tx, Nexstar &nexstar) : port{rx, tx}, nexstar{nexstar} {}

void displayGPSInfo(TinyGPSPlus &gps, Stream &stream);

void GPS::sleep() {
  static const char sleepMessage[] = {0xB5, 0x62, 0x02, 0x41, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x4D, 0x3B};
  for (uint8_t i = 0; i < sizeof(sleepMessage); i++)
    port.write(sleepMessage[i]);
  delay(1000);
}

void GPS::resume() {
  delay(500);
  for (int i = 0; i < 10; i++)
    port.write("\xFF");
  delay(500);
}

void GPS::open() {
  port.begin(9600);
//  send_nmea("PUBX,41,1,3,3,115200,0");
//  port.end();
//  port.begin(57600);

}

void GPS::close() {
  port.end();
}

bool GPS::wait_for_fix(uint16_t timeout_sec) {
  auto start = millis();
  port.listen();
  auto expired = [&start, &timeout_sec] { return static_cast<unsigned long>(millis() - start) >= static_cast<unsigned long>(timeout_sec) * 1000l; };

  while ( ! expired() && ! has_fix() ) {
    while (port.available() && ! expired()) {
      gps.encode(port.read());
      //if ();
        //displayGPSInfo(gps, Serial);
    }
  }
  //displayGPSInfo(gps, Serial);
  syncDateTime();
  if(has_time()) {
    nexstar.set_time();
  }
  return has_fix();
}


void GPS::send_nmea(const String &command) {
  uint8_t cs = 0;
  for (auto c : command) {
    cs = cs ^ c;
  }
  port.print("$");
  port.print(command);
  port.print("*");
  char lower = (cs & 0xF);
  char higher = (cs >> 4);

  port.write(higher < 10 ? '0' + higher : 'A' + higher - 10);
  port.write(lower < 10 ? '0' + lower : 'A' + lower - 10);
  port.print("\r\n");
}

void GPS::debug(Stream &stream, bool raw) {
  port.listen();
  while(true) {
      if(stream.available()) {
        if(stream.readString() == "stop")
          return;
      }
      while (port.available()) {
        int c = port.read();
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
  //Serial.print("UNIX time: "); Serial.println(t);
  RTC.set(t);
  setTime(t);
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
