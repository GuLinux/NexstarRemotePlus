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

#include <TimeLib.h>
//#define BUTTON_PIN 3
#define BUTTON_PIN 32

// Wiring: green -> TX, blue -> RX

// Voltmeter: AnalogPin 10, R1=, R2=8180


RTC rtc;
Display display;
Settings settings;
PCStream pc_stream{&Serial};
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
  Serial.begin(9600);
  settings.load();
  bluetooth.setup();
  buttons.setup(BUTTON_PIN);
  rtc.setup();
  display.begin();
  Serial.println("Starting up NexStarRemote+");
  
  digitalWrite(LED_BUILTIN, LOW);


 // if(timeStatus()!= timeSet)
 //   Serial.println(F("Error setting time from RTC"));
  gps.open();
  gps.resume();
  nexstar.setup();
  processor.setup();
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.setTimeout(2000);
}

void loop() {
  buttons.tick();
  processor.loop();
}


