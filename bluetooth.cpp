#include "bluetooth.h"
#include "settings.h"

#define STATE_PIN 13
#define KEY_PIN 11
#define EN_PIN 12

Bluetooth::Bluetooth(HardwareSerial &port) : Singleton<Bluetooth>(this), _port(port) {}

void Bluetooth::setup() {
  _port.setTimeout(2000);
#if HC_MODEL == 5
    pinMode(STATE_PIN, INPUT);
    pinMode(KEY_PIN, OUTPUT);
    pinMode(EN_PIN, OUTPUT);
    digitalWrite(EN_PIN, LOW);
    digitalWrite(KEY_PIN, HIGH);
    delay(500);
    digitalWrite(EN_PIN, HIGH);
    _port.begin(38400);
    //_port.println("Bluetooth settings--");
    for(int i = 0; i < 20; i++) {
      digitalWrite(LED_BUILTIN, i%2 ? HIGH : LOW);
      delay(200);
    }
  if(atCommand("AT").startsWith("OK")) {
    atCommand(String("AT+NAME=") + Settings::instance()->bluetooth_name());
    atCommand(String("AT+PIN=") + Settings::instance()->bluetooth_pin());
    atCommand("AT+RESET");
    digitalWrite(KEY_PIN, LOW);
  }
  _port.end();
  _port.begin(9600);
  digitalWrite(LED_BUILTIN, LOW);
#else
  _port.begin(9600);
  atCommand("AT");
  atCommand(String("AT+NAME") + Settings::instance()->bluetooth_name());
  atCommand(String("AT+PIN") + Settings::instance()->bluetooth_pin());
#endif
  //delay(500);
  //_port.println("Bluetooth initialized");
}

String Bluetooth::atCommand(const String &msg) {
  _port.write(msg.c_str());
#if HC_MODEL == 5
  _port.write("\r\n");
#endif
  return _port.readString();
}

