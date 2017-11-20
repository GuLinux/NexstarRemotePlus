#include "nexstar.h"
#include "settings.h"
#include "TimeLib.h"
Nexstar::Nexstar(HardwareSerial &port, Settings &settings) : _port(port), settings(settings)
{
}



void Nexstar::read_to(Stream &stream) {
  if(_port.available()) {
    stream.write(_port.read());
  }
}

void Nexstar::setup() {
  _port.begin(9600);
}

void Nexstar::set_gps_info(double latitude, double longitude) {
  
}

void Nexstar::set_time() {
  tmElements_t time;
  int32_t offset = static_cast<int32_t>(settings.timezone() + settings.daylight_saving()) * 60 * 60;
  breakTime(now() + offset, time);
  
  _port.write('H');
  _port.write(time.Hour);
  _port.write(time.Minute);
  _port.write(time.Second);
  _port.write(time.Month);
  _port.write(time.Day);
  _port.write(1970 + time.Year - 2000);
  _port.write(settings.timezone() > 0 ? settings.timezone() : (256 + settings.timezone()));
  _port.write(settings.daylight_saving());
}

