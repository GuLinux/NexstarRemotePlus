#include "processor.h"
#include "nexstar.h"
#include "gps.h"
#include "bluetooth.h"
#include "commands.h"
#include "settings.h"
#include "pc_stream.h"
#include "osd.h"
#include "display.h"

Processor::Processor() : Singleton<Processor>(this) {
}

void Processor::setup() {
  set_gps_expire();
}

Stream *get_stream(Processor::Connection connection) {
  if(connection == Processor::USB)
    return &Serial;
  return &Bluetooth::instance()->port();
}

void Processor::loop() {
  check_connection();

  GPS::instance()->process();
  gps_power_management();

  nexstar_sync();
  Commands::instance()->read();
  Nexstar::instance()->port().write(Commands::instance()->buffer(), Commands::instance()->buffer_len());
  Nexstar::instance()->read_to(PCStream::instance()->current());
  OSD::instance()->tick();
  Display::instance()->update();
}

void Processor::check_connection() {
  Connection connection = static_cast<bool>(Serial) ? USB : Bluetooth;

  this->_connection = connection;
  Display::instance()->set_connection(connection);
  PCStream::instance()->set_current(get_stream(connection));
}

void Processor::gps_power_management() {
  if(_gps_wakeup_requested && GPS::instance()->suspended() ) {
    OSD::instance()->show_message("Wake up GPS");
    GPS::instance()->resume();
    set_gps_expire();
    _gps_wakeup_requested = false;
  }

  if(_gps_expire > 0 && millis() > _gps_expire && ! GPS::instance()->suspended() ) {
    GPS::instance()->sleep();
  }
}

void Processor::set_gps_expire() {
  auto gps_timeout = Settings::instance()->gps_timeout();
  if(gps_timeout > 0) {
    _gps_expire = static_cast<uint32_t>(gps_timeout) * 1000 + millis();
  } else {
    _gps_expire = 0;
  }
}

void Processor::nexstar_sync() {
  if(! _sync_nexstar)
    return;
  OSD::instance()->show_message("Synchronizing Nexstar time/location", 10);
  Nexstar::instance()->set_time();
  _sync_nexstar = false;
}

