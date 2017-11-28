#include "commands.h"
#include "gps.h"
#include "nexstar.h"
#include "bluetooth.h"
#include "settings.h"
#include "processor.h"
#include "pc_stream.h"
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

Commands::Commands() : Singleton<Commands>(this) {
}

void Commands::read() {
  _buffer_len = 0;
  while(_buffer_len < 3) {
    if(PCStream::instance()->current().available())
      _buffer[_buffer_len++] = PCStream::instance()->current().read();
    if(_buffer[0] != 'A')
      return;
    if(_buffer_len > 1 && _buffer[1] != 'T')
      return;
    if(_buffer_len > 2 && _buffer[2] != '+')
      return;  
    if(_buffer_len > 2) {
      _buffer_len = 0;

      handle(PCStream::instance()->current().readStringUntil('#'));
      return;
    }
  }
}

void Commands::handle(const String &command) {
  Command parsed{command};
  /*
  PCStream::instance()->current().print("command name: "); PCStream::instance()->current().println(parsed.name);
  PCStream::instance()->current().print("arg0: '"); PCStream::instance()->current().print(parsed.params[0]); PCStream::instance()->current().println("'");
  PCStream::instance()->current().print("arg1: '"); PCStream::instance()->current().print(parsed.params[1]); PCStream::instance()->current().println("'");
  PCStream::instance()->current().print("arg2: '"); PCStream::instance()->current().print(parsed.params[2]); PCStream::instance()->current().println("'");
  PCStream::instance()->current().print("arg3: '"); PCStream::instance()->current().print(parsed.params[3]); PCStream::instance()->current().println("'");
  */
  if(
      parsed.handle("PING", [this](const Command &) { PCStream::instance()->current().println("PONG"); }) ||
      parsed.handle("GPSWAKE", [this](const Command &c) { wake_gps(c); }) ||
      parsed.handle("GPSDEBUG", [this](const Command &c) { gps_debug(c); }) ||
      parsed.handle("BTNAME", [this](const Command &c) { bluetooth_name(c); }) ||
      parsed.handle("BTPIN", [this](const Command &c) { bluetooth_pin(c); }) ||
      parsed.handle("TZINFO", [this](const Command &c) { change_tz(c); }) ||
      parsed.handle("TIME", [this](const Command &c) { time(c); })
  )
    return;
  PCStream::instance()->current().print(F("Unrecognized command: ")); PCStream::instance()->current().print(parsed.name); PCStream::instance()->current().println("#");
  PCStream::instance()->current().println(F("Available commands: PING, GPSWAKE, GPSDEBUG[=raw], BTNAME[=name], BTPIN[=pin],TZINFO[=tz,dst]"));
}

void Commands::bluetooth_settings_changed() {
  PCStream::instance()->current().println(F("Bluetooth settings will be applied at next reboot"));
}

void Commands::bluetooth_name(const Command &command) {
  if(command.params[0].length() == 0) {
    PCStream::instance()->current().println(Settings::instance()->bluetooth_name());
  } else {
    Settings::instance()->bluetooth_name(command.params[0].c_str());
    PCStream::instance()->current().print("+OK"); PCStream::instance()->current().println(Settings::instance()->bluetooth_name());    
    bluetooth_settings_changed();
  }
}

void Commands::bluetooth_pin(const Command &command) {
  if(command.params[0].length() == 0) {
    PCStream::instance()->current().println(Settings::instance()->bluetooth_pin());
  } else {
    Settings::instance()->bluetooth_pin(command.params[0].c_str());
    PCStream::instance()->current().print(F("+OK")); PCStream::instance()->current().println(Settings::instance()->bluetooth_pin());
    bluetooth_settings_changed();
  }
}

void Commands::change_tz(const Command &command) {
  if(command.params[0].length() == 0) {
    PCStream::instance()->current().print(F("timezone: ")); PCStream::instance()->current().print(static_cast<int>(Settings::instance()->timezone()));
    PCStream::instance()->current().print(F(", dst: ")); PCStream::instance()->current().println(static_cast<int>(Settings::instance()->daylight_saving()));
  } else {
    Settings::instance()->timezone(atoi(command.params[0].c_str()));
    Settings::instance()->daylight_saving(atoi(command.params[1].c_str()));
    PCStream::instance()->current().print(F("+OK")); PCStream::instance()->current().print(Settings::instance()->timezone()); PCStream::instance()->current().print(","); PCStream::instance()->current().println(Settings::instance()->daylight_saving());
  }
}



void Commands::wake_gps(const Command &command) {
  Processor::instance()->request_gps_wakeup();
}

void Commands::time(const Command &command) {
  tmElements_t datetime;
  PCStream::instance()->current().print(hour()); PCStream::instance()->current().print(":"); PCStream::instance()->current().print(minute()); PCStream::instance()->current().print(":"); PCStream::instance()->current().print(second()); PCStream::instance()->current().print(" ");
  PCStream::instance()->current().print(day()); PCStream::instance()->current().print("/"); PCStream::instance()->current().print(month()); PCStream::instance()->current().print("/"); PCStream::instance()->current().println(year());
}

void Commands::gps_debug(const Command &command) {
    GPS::instance()->resume();
    GPS::instance()->debug(PCStream::instance()->current(), command.params[0] == "raw");
    GPS::instance()->sleep();
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

