#include "display.h"
#include "TimeLib.h"
#include "icons/icons.h"
#include "gps.h"
#include "osd.h"
#include "bluetooth.h"
#include "battery.h"
#include "pc_stream.h"
#include "logger.h"

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif


#define ICON_TOP 0

#define ICONS_SIZE 32
#define ICONS_SPACING 2

#define ICON_X(n) ICONS_SPACING * n + ICONS_SIZE * n
#define ICON_Y(n) ICONS_SPACING * n + ICONS_SIZE * n


#define CLOCK_TOP 56

#define OSD_WIDTH 128
#define OSD_HEIGHT CLOCK_TOP - 1

#define OLED_DC     3
#define OLED_CS     7
#define OLED_RESET  31

const unsigned char * get_battery_icon() {
  uint8_t percentage = Battery::instance()->percentage();
   if(percentage <= 20)
    return icon_battery_20;
  if(percentage <= 30)
    return icon_battery_30;
  if(percentage <= 50)
    return icon_battery_50;
  if(percentage <= 60)
    return icon_battery_60;
  if(percentage <= 80)
    return icon_battery_80;
  if(percentage <= 90)
    return icon_battery_90;
  return icon_battery_100; 
}
const unsigned char * get_connection_icon() {
  if(PCStream::instance()->connection() == PCStream::USB)
    return icon_usb;
  if(Bluetooth::instance()->isConnected())
    return icon_bluetooth_connected;
  return icon_bluetooth;
}

const unsigned char * get_gps_icon() {
  if(GPS::instance()->has_fix())
    return icon_gps_on;
  return icon_gps_off;
}

Display::Display(): Singleton<Display>(this), oled(OLED_DC, OLED_RESET, OLED_CS), osd_canvas(OSD_WIDTH, OSD_HEIGHT) {
}

void Display::begin() {
  oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  oled.setTextSize(2);
  oled.setTextColor(WHITE);
  oled.clearDisplay();
  oled.print("Nexstar\nRemote+");
  oled.display();
}


void Display::update() {
  char datetime[20];
  sprintf(datetime, "%02d/%02d/%04d %02d:%02d:%02d", day(), month(), year(), hour(), minute(), second());

  oled.setTextSize(1);
  oled.clearDisplay();
  oled.setCursor(0,CLOCK_TOP);
  oled.print(datetime);
  osd_canvas.setCursor(0,0);
  osd_canvas.fillScreen(BLACK);
  osd_canvas.setTextSize(2);
  if(OSD::instance()->render(osd_canvas)) {
    oled.drawBitmap(0, 0, osd_canvas.getBuffer(), osd_canvas.width(), osd_canvas.height(), WHITE);
    oled.display();
    return;
  }


  oled.drawBitmap(ICON_X(0), ICON_Y(0), get_connection_icon(), ICONS_SIZE, ICONS_SIZE, WHITE);
  oled.drawBitmap(ICON_X(1), ICON_Y(0), get_gps_icon(), ICONS_SIZE, ICONS_SIZE, WHITE);
  oled.drawBitmap(ICON_X(2), ICON_Y(0), get_battery_icon(), ICONS_SIZE, ICONS_SIZE, WHITE);

  oled.display();
}

