#include "commands.h"
#include "gps.h"
#include "nexstar.h"
#include "bluetooth.h"
#include "settings.h"
#include "processor.h"
#include "pc_stream.h"
#include <TimeLib.h>
#include "logger.h"

struct Command {
  static const char *OK;
  Command(String message);
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

const char *Command::OK = "+OK";

Commands::Commands() : Singleton<Commands>(this) {
}

void Commands::read() {
  _buffer_len = 0;
  while(_buffer_len < 3) {
    if(PCStream::instance()->available())
      _buffer[_buffer_len++] = PCStream::instance()->read();
    if(_buffer[0] != 'A')
      return;
    if(_buffer_len > 1 && _buffer[1] != 'T')
      return;
    if(_buffer_len > 2 && _buffer[2] != '+')
      return;  
    if(_buffer_len > 2) {
      _buffer_len = 0;
      memset(_buffer, 0, 3);
      PCStream::instance()->setTimeout(10000);
      handle(PCStream::instance()->readStringUntil('\n'));
      return;
    }
  }
}

void Commands::handle(const String &command) {
  Command parsed{command};

  if(
      parsed.handle("PING", [this](const Command &) { INFO() << Command::OK << F("PONG");  }) ||
      parsed.handle("GPSWAKE", [this](const Command &c) { wake_gps(c); }) ||
      parsed.handle("LOGLEVEL", [this](const Command &c) { log_level(c); }) ||
      parsed.handle("BTNAME", [this](const Command &c) { bluetooth_name(c); }) ||
      parsed.handle("BTPIN", [this](const Command &c) { bluetooth_pin(c); }) ||
      parsed.handle("TZINFO", [this](const Command &c) { change_tz(c); }) ||
      parsed.handle("TIME", [this](const Command &c) { time(c); })
  )
    return;

  ERROR() << F("Unrecognized command: ") << parsed.name << '#';
  ERROR() << F("Available commands: PING, GPSWAKE, LOGLEVEL[=level], BTNAME[=name], BTPIN[=pin],TZINFO[=tz,dst]");
}

void Commands::bluetooth_settings_changed() {
  Bluetooth::instance()->reload_settings();
}

void Commands::bluetooth_name(const Command &command) {
  if(command.params[0].length() > 0) {
    Settings::instance()->bluetooth_name(command.params[0].c_str());
    bluetooth_settings_changed();
  }
  INFO() << Command::OK << Settings::instance()->bluetooth_name();
}

void Commands::bluetooth_pin(const Command &command) {
  if(command.params[0].length() > 0) {
    Settings::instance()->bluetooth_pin(command.params[0].c_str());
    bluetooth_settings_changed();
  }
  INFO() << Command::OK << Settings::instance()->bluetooth_pin();
}

void Commands::change_tz(const Command &command) {
  if(command.params[0].length() > 0) {
    Settings::instance()->timezone(atoi(command.params[0].c_str()));
    Settings::instance()->daylight_saving(atoi(command.params[1].c_str()));
  }
  INFO() << Command::OK << F(",tz=") << LOG_I(Settings::instance()->timezone()) << F(",dst=") << LOG_I(Settings::instance()->daylight_saving());
  
}

void Commands::wake_gps(const Command &command) {
  Processor::instance()->request_gps_wakeup();
  INFO() << Command::OK;
}

void Commands::time(const Command &command) {
  tmElements_t datetime;
  INFO() << Command::OK << year() << month() << day() << 'T' <<hour() << ':' << minute() << ':' << second();
}

void Commands::log_level(const Command &command) {
  if(command.params[0].length() > 0) {
    int level = atoi(command.params[0].c_str());
    if(level < Logger::Trace || level > Logger::Error) {
      ERROR() << F("+ERR: level ") << level << F(" not recognized");
      return;
    }
    Logger::instance()->set_level(static_cast<Logger::Level>(level));
    Settings::instance()->log_level(Logger::instance()->level());
  }
  INFO() << Command::OK << Logger::instance()->level();
}

Command::Command(String message) {
  message.trim();
  int start = 0;
  int end = message.length();
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

