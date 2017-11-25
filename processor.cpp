#include "processor.h"
#include "nexstar.h"
#include "gps.h"
#include "bluetooth.h"
#include "commands.h"
#include "settings.h"
#include "pc_stream.h"
#include "osd.h"
#include "display.h"

Processor::Processor() : Singleton<Processor>(this) {}

Stream *get_stream(Processor::Connection connection) {
  if(connection == Processor::USB)
    return &Serial;
  return &Bluetooth::instance()->port();
}
//uint32_t last_loop_run = 0;
void Processor::loop() {
//  Serial.print("Elapsed: "); Serial.println(millis() - last_loop_run);
//  last_loop_run = millis();
  check_connection();
  //gps_fix();
  GPS::instance()->process();
  nexstar_sync();
  Commands::instance()->read();
  Nexstar::instance()->port().write(Commands::instance()->buffer(), Commands::instance()->buffer_len());
  Nexstar::instance()->read_to(PCStream::instance()->current());
  OSD::instance()->tick();
  Display::instance()->update();
}

void Processor::check_connection() {
  Connection connection = static_cast<bool>(Serial) ? USB : Bluetooth;

//  if(connection != this->_connection) {
    this->_connection = connection;
    Display::instance()->set_connection(connection);
    PCStream::instance()->set_current(get_stream(connection));
//  }
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

