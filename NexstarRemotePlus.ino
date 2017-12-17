#include "rtc.h"

#include "bluetooth.h"
#include "gps.h"
#include "nexstar.h"
#include "commands.h"
#include "settings.h"
#include "processor.h"
#include "display.h"
#include "pc_stream.h"
#include "osd.h"
#include "buttons.h"
#include "battery.h"
#include "blink_led.h"

#include "logger.h"
//#define BUTTON_PIN 3
#define BUTTON_PIN 32

// Wiring: green -> TX, blue -> RX

PCStream pc_stream;
Logger logger(pc_stream);


Battery battery;
RTC rtc;
Display display;
Settings settings;
Bluetooth bluetooth{Serial2}; // RX, TX
Nexstar nexstar{Serial1};

GPS gps{Serial3};
HardwareTimer display_timer(1);


Commands commands;
Processor processor;
Buttons buttons;
OSD osd;


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  BlinkLed blink_led;
  Serial.begin(9600);
  rtc.setup();
  settings.load();
  logger.set_level(static_cast<Logger::Level>(settings.log_level()));
  DEBUG() << F("Starting setup sequence");



  bluetooth.setup();
  buttons.setup(BUTTON_PIN);

  display.begin();
  DEBUG() << F("Starting up NexStarRemote+");
  battery.setup();
  



 // if(timeStatus()!= timeSet)
 //   Serial.println(F("Error setting time from RTC"));
  gps.open();
  gps.resume();
  nexstar.setup();
  processor.setup();
  DEBUG() << F("Setup finished");
}

void loop() {
  buttons.tick();
  processor.loop();
}


