#include "processor.h"
#include "nexstar.h"
#include "gps.h"
#include "bluetooth.h"
#include "commands.h"
#include "settings.h"

void Processor::loop() {
  if(_gps_fix_requested) {
    gps.resume();
    int secs = _gps_fix_timeout;
    if(secs == 0)
      secs = settings.gps_timeout();
    Serial.print(F("Looking for gps fix, timeout=")); Serial.print(secs); Serial.println(F(" secs"));
    auto got_fix = gps.wait_for_fix(secs*1000);
    Serial.print(F("got fix: ")); Serial.println(got_fix);
    gps.sleep();
  }
  if(_sync_nexstar) {
    nexstar.set_time();
  }
  commands.read();
  nexstar.write(commands.buffer(), commands.buffer_len());
  nexstar.read_to(Serial);
}

