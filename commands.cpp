#include "commands.h"
#include "gps.h"
#include "nexstar.h"
#include "bluetooth.h"
#include "settings.h"
#include "processor.h"
#include "pc_stream.h"
//#include <DS1307RTC.h>
#include <TimeLib.h>


struct Command {
  Command(const String &message);
  String name;
  String params[4];
  bool matches(const String &name) const;
  template<typename T> bool handle(const char *name, T t) {
    if(this->name == name) {
      t(*this);
      return true;
    }
    return false;
  }
};

Commands::Commands(GPS &gps, Nexstar &nexstar, Bluetooth &bluetooth, Settings &settings, PCStream &pc_stream)
        : gps(gps), nexstar(nexstar), bluetooth(bluetooth), settings(settings), pc_stream(pc_stream)
{
}

void Commands::read() {
  _buffer_len = 0;
  while(_buffer_len < 3) {
    if(pc_stream.current().available())
      _buffer[_buffer_len++] = pc_stream.current().read();
    if(_buffer[0] != 'A')
      return;
    if(_buffer_len > 1 && _buffer[1] != 'T')
      return;
    if(_buffer_len > 2 && _buffer[2] != '+')
      return;  
    if(_buffer_len > 2) {
      _buffer_len = 0;

      handle(pc_stream.current().readStringUntil('#'));
      return;
    }
  }
}

void Commands::handle(const String &command) {
  Command parsed{command};
  /*
  pc_stream.current().print("command name: "); pc_stream.current().println(parsed.name);
  pc_stream.current().print("arg0: '"); pc_stream.current().print(parsed.params[0]); pc_stream.current().println("'");
  pc_stream.current().print("arg1: '"); pc_stream.current().print(parsed.params[1]); pc_stream.current().println("'");
  pc_stream.current().print("arg2: '"); pc_stream.current().print(parsed.params[2]); pc_stream.current().println("'");
  pc_stream.current().print("arg3: '"); pc_stream.current().print(parsed.params[3]); pc_stream.current().println("'");
  */
  if(
      parsed.handle("PING", [this](const Command &) { pc_stream.current().println("PONG"); }) ||
      parsed.handle("GPSFIX", [this](const Command &c) { gps_fix(c); }) ||
      parsed.handle("GPSDEBUG", [this](const Command &c) { gps_debug(c); }) ||
      parsed.handle("BTNAME", [this](const Command &c) { bluetooth_name(c); }) ||
      parsed.handle("BTPIN", [this](const Command &c) { bluetooth_pin(c); }) ||
      parsed.handle("TZINFO", [this](const Command &c) { change_tz(c); }) ||
      parsed.handle("TIME", [this](const Command &c) { time(c); })
  )
    return;
  pc_stream.current().print(F("Unrecognized command: ")); pc_stream.current().print(parsed.name); pc_stream.current().println("#");
  pc_stream.current().println(F("Available commands: PING, GPSFIX, GPSDEBUG[=raw], BTNAME[=name], BTPIN[=pin],TZINFO[=tz,dst]"));
}

void Commands::bluetooth_settings_changed() {
#if HC_MODEL == 5
  pc_stream.current().println(F("With bluetooth module HC-05 you need to manually reboot into AT mode (pushing the setup button)."));
#else
  pc_stream.current().println(F("Bluetooth settings will be applied at next reboot"));
#endif
}

void Commands::bluetooth_name(const Command &command) {
  if(command.params[0].length() == 0) {
    pc_stream.current().println(settings.bluetooth_name());
  } else {
    settings.bluetooth_name(command.params[0].c_str());
    pc_stream.current().print("+OK"); pc_stream.current().println(settings.bluetooth_name());    
    bluetooth_settings_changed();
  }
}

void Commands::bluetooth_pin(const Command &command) {
  if(command.params[0].length() == 0) {
    pc_stream.current().println(settings.bluetooth_pin());
  } else {
    settings.bluetooth_pin(command.params[0].c_str());
    pc_stream.current().print(F("+OK")); pc_stream.current().println(settings.bluetooth_pin());
    bluetooth_settings_changed();
  }
}

void Commands::change_tz(const Command &command) {
  if(command.params[0].length() == 0) {
    pc_stream.current().print(F("timezone: ")); pc_stream.current().print(static_cast<int>(settings.timezone()));
    pc_stream.current().print(F(", dst: ")); pc_stream.current().println(static_cast<int>(settings.daylight_saving()));
  } else {
    settings.timezone(atoi(command.params[0].c_str()));
    settings.daylight_saving(atoi(command.params[1].c_str()));
    pc_stream.current().print(F("+OK")); pc_stream.current().print(settings.timezone()); pc_stream.current().print(","); pc_stream.current().println(settings.daylight_saving());
  }
}



void Commands::gps_fix(const Command &command) {
  processor->gps_getfix();
}

void Commands::time(const Command &command) {
  tmElements_t datetime;
  pc_stream.current().print(hour()); pc_stream.current().print(":"); pc_stream.current().print(minute()); pc_stream.current().print(":"); pc_stream.current().print(second()); pc_stream.current().print(" ");
  pc_stream.current().print(day()); pc_stream.current().print("/"); pc_stream.current().print(month()); pc_stream.current().print("/"); pc_stream.current().println(year());
}

void Commands::gps_debug(const Command &command) {
    gps.resume();
    gps.debug(pc_stream.current(), command.params[0] == "raw");
    gps.sleep();
}

Command::Command(const String &message) {
  int start = 0;
  int end = message.indexOf('#');
  int eq = message.indexOf('=');
  name = message.substring(0, eq != -1 ? eq : end);
  start=eq+1;
   {
    for(int i=0; i<4; i++) {
      if(! start > 0)
        break;
      int comma = message.indexOf(',', start);
      params[i] = message.substring(start, comma != -1 ? comma : end);
      start = comma + 1;
    }
  }
}

