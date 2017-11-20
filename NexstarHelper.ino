#include "bluetooth.h"
#include "gps.h"
#include "nexstar.h"
#include "commands.h"
#include "settings.h"
#include "processor.h"
#include "display.h"
#include "pc_stream.h"
#include <DS1307RTC.h>

#define BUTTON_PIN 3

// Wiring: green -> TX, blue -> RX
// AltSoftSerial wiring: 9=TX, 8=RX

// D3 = BT RXD = SWSerial TXD
// D4 = BT TXT = SWSerial RXD
Display display;

Settings settings;
PCStream pc_stream{Serial};
Bluetooth bluetooth{Serial3, settings}; // RX, TX
Nexstar nexstar{Serial1, settings};

GPS gps{Serial2, nexstar};

Commands commands{gps, nexstar, bluetooth, settings, pc_stream};
Processor processor{nexstar, gps, bluetooth, display, commands, settings, pc_stream};


void setup() {
  Serial.begin(9600);
  delay(4000);
  Serial.println("Starting up NexStarRemote+");
  digitalWrite(LED_BUILTIN, LOW);
  setSyncProvider(RTC.get);   // the function to get the time from the RTC
  settings.load();
  bluetooth.setup();
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
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(BUTTON_PIN, buttonChanged, CHANGE);
}

void loop() {
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

