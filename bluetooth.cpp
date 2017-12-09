#include "bluetooth.h"
#include "logger.h"
#include "settings.h"

#define STATE_PIN 13
#define KEY_PIN 11
#define EN_PIN 12

Bluetooth::Bluetooth(HardwareSerial &port) : Singleton<Bluetooth>(this), _port(port) {}

void Bluetooth::setup() {
  // Set pin modes
  pinMode(STATE_PIN, INPUT);
  pinMode(KEY_PIN, OUTPUT);
  pinMode(EN_PIN, OUTPUT);
  reload_settings();
}

String Bluetooth::atCommand(const char *msg) {
  DEBUG() << '>' << msg;
  _port.write(msg);
  _port.write("\r\n");
  auto result = _port.readStringUntil('\n');
  DEBUG() << '<' << result;
  return result;
}

bool Bluetooth::isConnected() const {
  return digitalRead(STATE_PIN) == HIGH;
}

void Bluetooth::reload_settings() {
  char buffer[BT_NAME_SIZE + 12]{0};
  _booted = false;
  _port.end();
  _port.setTimeout(200);
  // Turn off bluetooth
  digitalWrite(EN_PIN, LOW);
  // Set AT mode
  digitalWrite(KEY_PIN, HIGH);
  delay(100);
  // Turn on bluetooth again
  digitalWrite(EN_PIN, HIGH);
  _port.begin(38400);
  //_port.println("Bluetooth settings--");
  DEBUG() << F("--- Bluetooth command mode ---");
  while(! atCommand("AT").startsWith("OK"))
    delay(100);
  sprintf(buffer, "AT+NAME=%s", Settings::instance()->bluetooth_name());
  atCommand(buffer);
  sprintf(buffer, "AT+PSWD=\"%s\"", Settings::instance()->bluetooth_pin());
  atCommand(buffer);
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

