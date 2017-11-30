#include "bluetooth.h"
#include "logger.h"
#include "settings.h"

#define STATE_PIN 13
#define KEY_PIN 11
#define EN_PIN 12

Bluetooth::Bluetooth(HardwareSerial &port) : Singleton<Bluetooth>(this), _port(port) {}

void Bluetooth::setup() {
  _port.setTimeout(2000);
  // Set pin modes
  pinMode(STATE_PIN, INPUT);
  pinMode(KEY_PIN, OUTPUT);
  pinMode(EN_PIN, OUTPUT);
  // Turn off bluetooth
  digitalWrite(EN_PIN, LOW);
  // Set AT mode
  digitalWrite(KEY_PIN, HIGH);
  delay(200);
  // Turn on bluetooth again
  digitalWrite(EN_PIN, HIGH);
  _port.begin(38400);
  //_port.println("Bluetooth settings--");
  DEBUG() << F("--- Bluetooth command mode ---");
  while(! atCommand("AT").startsWith("OK"))
    delay(100);
  atCommand(String("AT+NAME=") + Settings::instance()->bluetooth_name());
  atCommand(String("AT+PSWD=\"") + Settings::instance()->bluetooth_pin() + "\"");
  atCommand("AT+NAME?");
  atCommand("AT+PSWD?");
  atCommand("AT+VERSION?");
  // Turn off AT mode, and reset bluetooth adapter
  digitalWrite(KEY_PIN, LOW);
  atCommand("AT+RESET");
  DEBUG() << F("Bluetooth setup complete");
  _port.end();
  // Start normal serial terminal
  _port.begin(9600);
  _booted = true;
}

String Bluetooth::atCommand(const String &msg) {
  DEBUG() << '>' << msg;
  _port.write(msg.c_str());
  _port.write("\r\n");
  auto result = _port.readStringUntil('\n');
  DEBUG() << '<' << result;
  return result;
}

bool Bluetooth::isConnected() const {
  return digitalRead(STATE_PIN) == HIGH;
}
