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

#include <TimeLib.h>
//#define BUTTON_PIN 3
#define BUTTON_PIN 32

// Wiring: green -> TX, blue -> RX
// AltSoftSerial wiring: 9=TX, 8=RX

// D3 = BT RXD = SWSerial TXD
// D4 = BT TXT = SWSerial RXD

RTC rtc;
Display display;
Settings settings;
PCStream pc_stream{&Serial};
Bluetooth bluetooth{Serial3}; // RX, TX
Nexstar nexstar{Serial1};

GPS gps{Serial2};
HardwareTimer display_timer(1);


Commands commands;
Processor processor;
OSD osd;


void setup() {
  Serial.begin(9600);
  rtc.setup();
  display.begin();
  Serial.println("Starting up NexStarRemote+");
  
  digitalWrite(LED_BUILTIN, LOW);
  settings.load();
  bluetooth.setup();
 // if(timeStatus()!= timeSet)
 //   Serial.println(F("Error setting time from RTC"));
  gps.open();
  gps.resume();
  //gps.sleep();
  nexstar.setup();
  processor.setup();
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.setTimeout(2000);
  //processor.gps_getfix();
  //processor.sync_nexstar();
  pinMode(BUTTON_PIN, INPUT);
  attachInterrupt(BUTTON_PIN, buttonChanged, FALLING);
/*
  // Pause the timer while we're configuring it
  display_timer.pause();
  // Set up period
  display_timer.setPeriod(200000); // in microseconds

  // Set up an interrupt on channel 1
  display_timer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
  display_timer.setCompare(TIMER_CH1, 1);  // Interrupt 1 count after each update
  display_timer.attachCompare1Interrupt(on_timer);

  // Refresh the timer's count, prescale, and overflow
  display_timer.refresh();

  // Start the timer counting
  display_timer.resume();
  */
}

void loop() {
  processor.loop();
}

void buttonChanged() {
  osd.button_pressed();
}

void on_timer() {
  osd.tick();
  display.update();
}

