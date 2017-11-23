#include "processor.h"
#include "nexstar.h"
#include "gps.h"
#include "bluetooth.h"
#include "commands.h"
#include "settings.h"
#include "pc_stream.h"
#include "osd.h"

Processor::Processor() : Singleton<Processor>(this) {}

void Processor::loop() {
  gps_fix();
  nexstar_sync();
  Commands::instance()->read();
  Nexstar::instance()->port().write(Commands::instance()->buffer(), Commands::instance()->buffer_len());
  Nexstar::instance()->read_to(PCStream::instance()->current());
}

void Processor::gps_fix() {
  if(! _gps_fix_requested)
    return;
  OSD::instance()->show_message("Waiting for GPS", 999);
  GPS::instance()->resume();
  Serial.print(F("Looking for gps fix, timeout=")); Serial.print(Settings::instance()->gps_timeout()); Serial.println(F(" secs"));
  auto got_fix = GPS::instance()->wait_for_fix(Settings::instance()->gps_timeout());
  Serial.print(F("got fix: ")); Serial.println(got_fix);
  OSD::instance()->show_message(String("Fix: ") + (got_fix ? "OK" : "Failed"), 999);
  GPS::instance()->sleep();
  _gps_fix_requested = false;
}

void Processor::nexstar_sync() {
  if(! _sync_nexstar)
    return;
  OSD::instance()->show_message("Synchronizing Nexstar time/location", 10);
  Nexstar::instance()->set_time();
  _sync_nexstar = false;
}

