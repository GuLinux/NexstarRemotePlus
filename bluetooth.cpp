#include "bluetooth.h"
#include "settings.h";


Bluetooth::Bluetooth(Settings &settings) : settings{settings} {}

void Bluetooth::setup() {
  Serial.setTimeout(2000);
#if HC_MODEL == 5
    Serial.begin(38400);
    //Serial.println("Bluetooth settings--");
    for(int i = 0; i < 20; i++) {
      digitalWrite(LED_BUILTIN, i%2 ? HIGH : LOW);
      delay(200);
    }
  if(atCommand("AT").startsWith("OK")) {
    atCommand(String("AT+NAME=") + settings.bluetooth_name());
    atCommand(String("AT+PIN=") + settings.bluetooth_pin());
    atCommand("AT+RESET");
  }
  Serial.end();
  Serial.begin(9600);
  digitalWrite(LED_BUILTIN, LOW);
#else
  Serial.begin(9600);
  atCommand("AT");
  atCommand(String("AT+NAME") + settings.bluetooth_name());
  atCommand(String("AT+PIN") + settings.bluetooth_pin());
#endif
  //delay(500);
  //Serial.println("Bluetooth initialized");
}

String Bluetooth::atCommand(const String &msg) {
  Serial.write(msg.c_str());
#if HC_MODEL == 5
  Serial.write("\r\n");
#endif
  return Serial.readString();
}

