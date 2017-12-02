#include "display.h"
#include "TimeLib.h"
#include "icons/icons.h"
#include "gps.h"
#include "osd.h"
#include "bluetooth.h"
#include "battery.h"
#include "pc_stream.h"

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif


#define DRAW_ICON_BORDER 0
#define ICON_TOP 0

#define ICONS_SIZE 16
#define ICONS_SPACING 2

#define ICONS_BORDER_X 111
#define ICONS_X ICONS_BORDER_X + 1

#define ICONS_BORDER_RECT ICONS_SIZE + 1
#define ICON_BORDER_Y(num) ICON_TOP + ((ICONS_SPACING + ICONS_BORDER_RECT) * num )

#if DRAW_ICON_BORDER == 1
  #define ICON_Y(num) ICON_BORDER_Y(num) + 1
  #define OSD_WIDTH ICONS_BORDER_X - 1
#else
  #define ICON_Y(num) ICON_TOP + ((ICONS_SPACING + ICONS_SIZE) * num)
  #define OSD_WIDTH ICONS_X - 1
#endif

#define CLOCK_TOP 56
#define OSD_HEIGHT CLOCK_TOP - 1

#define OLED_DC     3
#define OLED_CS     7
#define OLED_RESET  31

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
  if(OSD::instance()->render(osd_canvas)) {
    oled.drawBitmap(0, 0, osd_canvas.getBuffer(), osd_canvas.width(), osd_canvas.height(), WHITE);
    oled.display();
    return;
  }
  if(PCStream::instance()->connection() == PCStream::USB) {
    oled.drawBitmap(ICONS_X, ICON_Y(0), icon_usb, ICONS_SIZE, ICONS_SIZE, WHITE);
  } else {
    oled.drawBitmap(ICONS_X, ICON_Y(0), Bluetooth::instance()->isConnected() ? icon_bluetooth_connected : icon_bluetooth, ICONS_SIZE, ICONS_SIZE, WHITE);
    //oled.drawBitmap(ICONS_X, ICON_Y(0), icon_bluetooth, ICONS_SIZE, ICONS_SIZE, WHITE);
  }
  if(GPS::instance()->has_fix()) {
    oled.drawBitmap(ICONS_X, ICON_Y(1), icon_gps_on, ICONS_SIZE, ICONS_SIZE, WHITE);
  } else {
    oled.drawBitmap(ICONS_X, ICON_Y(1), icon_gps_off, ICONS_SIZE, ICONS_SIZE, WHITE);
  }

  // Battery
  if(Battery::instance()->millivolts() > BATTERY_FULL_MV)
    oled.drawBitmap(ICONS_X, ICON_Y(2), icon_battery_full, ICONS_SIZE, ICONS_SIZE/2, WHITE);
  else if(Battery::instance()->millivolts() > BATTERY_MID_MV)
    oled.drawBitmap(ICONS_X, ICON_Y(2), icon_battery_high, ICONS_SIZE, ICONS_SIZE/2, WHITE);
  else if(Battery::instance()->millivolts() > BATTERY_MID_MV)
    oled.drawBitmap(ICONS_X, ICON_Y(2), icon_battery_mid, ICONS_SIZE, ICONS_SIZE/2, WHITE);
  else
    oled.drawBitmap(ICONS_X, ICON_Y(2), icon_battery_low, ICONS_SIZE, ICONS_SIZE/2, WHITE);

#if DRAW_ICON_BORDER == 1
  oled.drawRect(ICONS_BORDER_X, ICON_BORDER_Y(0), ICONS_BORDER_RECT, ICONS_BORDER_RECT, WHITE);
  oled.drawRect(ICONS_BORDER_X, ICON_BORDER_Y(1), ICONS_BORDER_RECT, ICONS_BORDER_RECT, WHITE);
#endif
  oled.display();
}

