#include "processor.h"
#include "nexstar.h"
#include "gps.h"
#include "bluetooth.h"
#include "commands.h"
#include "settings.h"

void Processor::loop() {
  if(_gps_fix_requested) {
    gps.resume();
    Serial.print(F("Looking for gps fix, timeout=")); Serial.print(settings.gps_timeout()); Serial.println(F(" secs"));
    auto got_fix = gps.wait_for_fix(settings.gps_timeout());
    Serial.print(F("got fix: ")); Serial.println(got_fix);
    gps.sleep();
    _gps_fix_requested = false;
  }
  if(_sync_nexstar) {
    nexstar.set_time();
    _sync_nexstar = false;
  }
  commands.read();
  nexstar.port().write(commands.buffer(), commands.buffer_len());
  nexstar.read_to(Serial);
}

