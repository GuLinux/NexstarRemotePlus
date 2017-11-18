//#include <AltSoftSerial.h>
#include <SoftwareSerial.h>
#include "bluetooth.h"
#include "gps.h"
#include "nexstar.h"
#include "commands.h"
#include "settings.h"
#include <DS1307RTC.h>

// Wiring: green -> TX, blue -> RX
// AltSoftSerial wiring: 9=TX, 8=RX

// D3 = BT RXD = SWSerial TXD
// D4 = BT TXT = SWSerial RXD
Settings settings;

Bluetooth bluetooth{settings, 10, 3}; // RX, TX
Nexstar nexstar{settings, 8, 9};

GPS gps{12, 4, nexstar};

Commands commands{gps, nexstar, bluetooth, settings};

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
    Serial.println("Error setting time from RTC");
  gps.open();
  auto got_fix = gps.wait_for_fix(1*1000);
  Serial.print("Got GPS fix: "); Serial.println(got_fix);
  gps.sleep();
  //gps.close();
  //Serial.println("Bluetooth setup completed");
  nexstar.setup();
  digitalWrite(LED_BUILTIN, HIGH);
  //gps.open();
  //gps.resume();
  //gps.debug(false, Serial);

  //pcPorts.write("#");
  Serial.setTimeout(2000);
  nexstar.set_time();
}

void loop() {
#ifdef BT_TEST
  while(Serial.available()) bt.write(Serial.read());
  while(bt.available()) Serial.write(bt.read());
  return;
#endif
  commands.read();
  nexstar.write(commands.buffer(), commands.buffer_len());
  nexstar.read_to(Serial);
}
