#include "nexstar.h"
#include "settings.h"
#include "TimeLib.h"
Nexstar::Nexstar(Settings &settings, int rx, int tx) : settings{settings}, _port{rx, tx} {}

void Nexstar::write(uint8_t *buffer, int size) {
  _port.listen();
  for(int i=0; i<size; i++)
    _port.write(buffer[i]);
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
  
  uint8_t command[9];
  command[0] = 'H';
  command[1] = time.Hour;
  command[2] = time.Minute;
  command[3] = time.Second;
  command[4] = time.Month;
  command[5] = time.Day;
  command[6] = 1970 + time.Year - 2000;
  command[7] = settings.timezone() > 0 ? settings.timezone() : (256 + settings.timezone());
  command[8] = settings.daylight_saving();
  write(command, 9);
}

