# NexstarRemote+

Nexstar Telescopes remote module, with GPS, Bluetooth and RTC support.

Initially built with Arduino, now using Maple Mini boards, since they have multiple advantages over the arduino nano modules:
 - multiple HardwareSerial ports (3 + 1 USB, as opposed to just the one in the Arduino Nano, that's also shared with the USB).
 - 3.3v logic levels (meaning we don't need the voltage dividers extra resistors)
