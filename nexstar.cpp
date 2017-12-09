#include "nexstar.h"
#include "settings.h"
#include "TimeLib.h"
#include "logger.h"

Nexstar::Nexstar(HardwareSerial &port) : Singleton<Nexstar>(this), _port(port)
{
}

void Nexstar::read_to(Stream &stream) {
  if(_port.available()) {
    stream.write(_port.read());
  }
}

void Nexstar::setup() {
  DEBUG() << F("Opening Nexstar serial port");
  _port.begin(9600);
  _port.setTimeout(2000);
}

struct LatLng {
  uint8_t degrees;
  uint8_t minutes;
  uint8_t seconds;
  uint8_t sign;
  LatLng(double number) {
    sign = 1;
    if(number < 0) {
      sign = -1;
      number = -number;
    }
    degrees = static_cast<uint8_t>(number);
    number = (number - static_cast<double>(degrees)) * 60;
    minutes = static_cast<uint8_t>(number);
    number = (number - static_cast<double>(minutes)) * 60;
    seconds = static_cast<uint8_t>(number);
  }
};

bool Nexstar::set_gps_info() {
  auto fix = Settings::instance()->last_fix();
  if(!fix.valid) {
    DEBUG() << F("Skipping nexstar position sync, as we don't have coordinates");
    return true;
  }
  if(fix == last_fix) {
    DEBUG() << F("Skipping nexstar position sync (same fix)");
    return true;
  }
  if(! check_connection())
    return false;
  DEBUG() << F("Setting position on nexstar: lat=") << fix.latitude << ", lng=" << fix.longitude;
  LatLng latitude(fix.latitude);
  LatLng longitude(fix.longitude);
  DEBUG() << F("Parsed position: ") << latitude.degrees << "° " << latitude.minutes << "' " << latitude.seconds << "\" " << (latitude.sign == 1 ? "N" : "S")
          << longitude.degrees << "° " << longitude.minutes << "' " << longitude.seconds << "\" " << (longitude.sign == 1 ? "E" : "W");
  _port.write('W');
  _port.write(latitude.degrees);
  _port.write(latitude.minutes);
  _port.write(latitude.seconds);
  _port.write(latitude.sign == 1 ? 0 : 1);
  _port.write(longitude.degrees);
  _port.write(longitude.minutes);
  _port.write(longitude.seconds);
  _port.write(longitude.sign == 1 ? 0 : 1);
  auto reply = get_reply();
  DEBUG() << F("Nexstar reply: ") << reply;
  if(reply != "#") {
    WARNING() << F("Error synchronizing position to Nexstar");
    return false;
  }
  return true;
}

struct NexstarTime {
  NexstarTime(time_t timestamp, uint8_t tz, uint8_t dst) {
    tmElements_t time;
    breakTime(timestamp, time);
    hour = time.Hour;
    minute = time.Minute;
    second = time.Second;
    month = time.Month;
    day = time.Day;
    year = time.Year - 30;
    this->tz = (tz > 0 ? tz : (256 + tz));
    this->dst = dst;
    sprintf(dbg, "h:%02x,m:%02x,s:%02x,M:%02x,D:%02x,Y:%02x,t:%02x,d:%01x", hour, minute, second, month, day, year, tz, dst);
  }
  char hour;
  char minute;
  char second;
  char month;
  char day;
  char year;
  char tz;
  char dst;
  char dbg[39]{0};
  void write(Stream &stream) {
    stream.write('H');
    stream.write(hour);
    stream.write(minute);
    stream.write(second);
    stream.write(month);
    stream.write(day);
    stream.write(year);
    stream.write(tz);
    stream.write(dst);
  }
};

bool Nexstar::set_time() {
  if(! check_connection())
    return false;
  // TODO: right now this is working either way, but we might use RTC::localtime instead
  NexstarTime time(now(), Settings::instance()->timezone().value, Settings::instance()->daylight_saving().value);
  DEBUG() << F("Setting time on nexstar: ") << now() << ": " << time.dbg;
  time.write(_port);
  

  auto reply = get_reply();
  DEBUG() << F("Nexstar reply: ") << reply;
  if(reply != "#") {
    WARNING() << F("Error synchronizing time to Nexstar");
    return false;
  }
  return true;
}

bool Nexstar::ping() {
  _port.write('K');
  _port.write('x');
  return get_reply() == "x#";
}

bool Nexstar::check_connection() {
  for(uint8_t i=1; i<=3; i++) {
    if(ping())
      return true;
    delay(500*i);
  }
  WARNING() << F("Nexstar seems unconnected");
  return false;
}
String Nexstar::get_reply() {
  uint32_t started = millis();
  char last_char;
  String output;
  while(millis() - started < 2000) {
    if(_port.available()) {
      last_char = _port.read();
      output += last_char;
      if(last_char == '#')
        break;
    }
  }
  return output;
}
