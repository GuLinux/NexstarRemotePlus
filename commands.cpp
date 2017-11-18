#include "commands.h"
#include "gps.h"
#include "nexstar.h"
#include "bluetooth.h"
#include "settings.h"
#include "processor.h"
#include <DS1307RTC.h>


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

Commands::Commands(GPS &gps, Nexstar &nexstar, Bluetooth &bluetooth, Settings &settings) : gps{gps}, nexstar{nexstar}, bluetooth{bluetooth}, settings{settings} {}

void Commands::read() {
  _buffer_len = 0;
  while(_buffer_len < 3) {
    if(Serial.available())
      _buffer[_buffer_len++] = Serial.read();
    if(_buffer[0] != 'A')
      return;
    if(_buffer_len > 1 && _buffer[1] != 'T')
      return;
    if(_buffer_len > 2 && _buffer[2] != '+')
      return;  
    if(_buffer_len > 2) {
      _buffer_len = 0;

      handle(Serial.readStringUntil('#'));
      return;
    }
  }
}

void Commands::handle(const String &command) {
  Command parsed{command};
  /*
  Serial.print("command name: "); Serial.println(parsed.name);
  Serial.print("arg0: '"); Serial.print(parsed.params[0]); Serial.println("'");
  Serial.print("arg1: '"); Serial.print(parsed.params[1]); Serial.println("'");
  Serial.print("arg2: '"); Serial.print(parsed.params[2]); Serial.println("'");
  Serial.print("arg3: '"); Serial.print(parsed.params[3]); Serial.println("'");
  */
  if(
      parsed.handle("PING", [](const Command &) { Serial.println("PONG"); }) ||
      parsed.handle("GPSFIX", [this](const Command &c) { gps_fix(c); }) ||
      parsed.handle("GPSDEBUG", [this](const Command &c) { gps_debug(c); }) ||
      parsed.handle("BTNAME", [this](const Command &c) { bluetooth_name(c); }) ||
      parsed.handle("BTPIN", [this](const Command &c) { bluetooth_pin(c); }) ||
      parsed.handle("TZINFO", [this](const Command &c) { change_tz(c); }) ||
      parsed.handle("TIME", [this](const Command &c) { time(c); })
  )
    return;
  Serial.print(F("Unrecognized command: ")); Serial.print(parsed.name); Serial.println("#");
  Serial.println(F("Available commands: PING, GPSFIX=<secs>, GPSDEBUG[=raw], BTNAME[=name], BTPIN[=pin],TZINFO[=tz,dst]"));
}

void Commands::bluetooth_settings_changed() {
#if HC_MODEL == 5
  Serial.println(F("With bluetooth module HC-05 you need to manually reboot into AT mode (pushing the setup button)."));
#else
  Serial.println(F("Bluetooth settings will be applied at next reboot"));
#endif
}

void Commands::bluetooth_name(const Command &command) {
  if(command.params[0].length() == 0) {
    Serial.println(settings.bluetooth_name());
  } else {
    settings.bluetooth_name(command.params[0].c_str());
    Serial.print("+OK"); Serial.println(settings.bluetooth_name());    
    bluetooth_settings_changed();
  }
}

void Commands::bluetooth_pin(const Command &command) {
  if(command.params[0].length() == 0) {
    Serial.println(settings.bluetooth_pin());
  } else {
    settings.bluetooth_pin(command.params[0].c_str());
    Serial.print(F("+OK")); Serial.println(settings.bluetooth_pin());
    bluetooth_settings_changed();
  }
}

void Commands::change_tz(const Command &command) {
  if(command.params[0].length() == 0) {
    Serial.print(F("timezone: ")); Serial.print(static_cast<int>(settings.timezone()));
    Serial.print(F(", dst: ")); Serial.println(static_cast<int>(settings.daylight_saving()));
  } else {
    settings.timezone(atoi(command.params[0].c_str()));
    settings.daylight_saving(atoi(command.params[1].c_str()));
    Serial.print(F("+OK")); Serial.print(settings.timezone()); Serial.print(","); Serial.println(settings.daylight_saving());
  }
}



void Commands::gps_fix(const Command &command) {
  processor->gps_getfix(atoi(command.params[0].c_str()));
}

void Commands::time(const Command &command) {
  tmElements_t datetime;
  if(RTC.chipPresent()) {
    Serial.print(hour()); Serial.print(":"); Serial.print(minute()); Serial.print(":"); Serial.print(second()); Serial.print(" ");
    Serial.print(day()); Serial.print("/"); Serial.print(month()); Serial.print("/"); Serial.println(year());
    
  } else {
    Serial.println(F("ERROR: no RTC chip detected"));
  }
}

void Commands::gps_debug(const Command &command) {
    gps.resume();
    gps.debug(Serial, command.params[0] == "raw");
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

