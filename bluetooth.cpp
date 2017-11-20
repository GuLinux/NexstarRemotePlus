#include "bluetooth.h"
#include "settings.h";


Bluetooth::Bluetooth(HardwareSerial &port, Settings &settings) : _port(port), settings(settings) {}

void Bluetooth::setup() {
  _port.setTimeout(2000);
#if HC_MODEL == 5
    _port.begin(38400);
    //_port.println("Bluetooth settings--");
    for(int i = 0; i < 20; i++) {
      digitalWrite(LED_BUILTIN, i%2 ? HIGH : LOW);
      delay(200);
    }
  if(atCommand("AT").startsWith("OK")) {
    atCommand(String("AT+NAME=") + settings.bluetooth_name());
    atCommand(String("AT+PIN=") + settings.bluetooth_pin());
    atCommand("AT+RESET");
  }
  _port.end();
  _port.begin(9600);
  digitalWrite(LED_BUILTIN, LOW);
#else
  _port.begin(9600);
  atCommand("AT");
  atCommand(String("AT+NAME") + settings.bluetooth_name());
  atCommand(String("AT+PIN") + settings.bluetooth_pin());
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

