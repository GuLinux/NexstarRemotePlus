//#include <AltSoftSerial.h>
#include <SoftwareSerial.h>
#include "bluetooth.h"
#include "gps.h"
#include "nexstar.h"
#include "commands.h"
#include "settings.h"
#include "processor.h"
#include "display.h"
#include <DS1307RTC.h>

#define BUTTON_PIN 2

// Wiring: green -> TX, blue -> RX
// AltSoftSerial wiring: 9=TX, 8=RX

// D3 = BT RXD = SWSerial TXD
// D4 = BT TXT = SWSerial RXD
Settings settings;

Bluetooth bluetooth{settings}; // RX, TX
Nexstar nexstar{settings, 8, 9};

GPS gps{12, 4, nexstar};

Commands commands{gps, nexstar, bluetooth, settings};
Display display;
Processor processor{nexstar, gps, bluetooth, display, commands, settings};

//AltSoftSerial nexstar; //{8, 9, false};
//#define BT_TEST 1
#ifdef BT_TEST
SoftwareSerial bt(11, 10);
#endif

void setup() {
#ifdef BT_TEST
  bt.begin(38400);
  Serial.begin(9600);
  return;
#endif
  digitalWrite(LED_BUILTIN, LOW);
  setSyncProvider(RTC.get);   // the function to get the time from the RTC
  settings.load();
  bluetooth.setup();
  setSyncProvider(RTC.get);   // the function to get the time from the RTC
  if(timeStatus()!= timeSet)
    Serial.println(F("Error setting time from RTC"));
  gps.open();
  gps.sleep();
  nexstar.setup();
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.setTimeout(2000);
  commands.set_processor(&processor);
  processor.gps_getfix();
  processor.sync_nexstar();
  pinMode(BUTTON_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonChanged, CHANGE);
}

void loop() {
#ifdef BT_TEST
  while(Serial.available()) bt.write(Serial.read());
  while(bt.available()) Serial.write(bt.read());
  return;
#endif
  processor.loop();
}

volatile unsigned long btn_high;
void buttonChanged() {
  int state = digitalRead(BUTTON_PIN);
  if(state == 1) {
    btn_high = millis();
  } else {
    processor.button_pressed(millis() - btn_high > 2000l);
  }
}

