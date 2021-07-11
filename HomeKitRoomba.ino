// HomeKit
#include "HomeSpan.h"
#include "RoombaAccessory.h"

// NTP
#include <NTPClient.h>
#include <WiFiUdp.h>

// Watchdog
#include <esp_task_wdt.h>

// Display
#include <TFT_eSPI.h> // Hardware-specific library
#include <SPI.h>

// RTC
#include <ESP32Time.h>
ESP32Time rtc;

// Roomba Comms
// Try 25, 26 if 32, 33 not working
const int tx_pin = 32;
const int rx_pin = 33;
const int baud_rate_change_pin = 22;

// NTP Client
WiFiUDP ntp_udp;
NTPClient time_client(ntp_udp);
boolean ntp_setup_complete = false;

// Watchdog
const int watchdog_timeout = 90;

// Display
TFT_eSPI tft = TFT_eSPI();       // Invoke custom library
long long last_refresh = 0;

float sx = 0, sy = 1, mx = 1, my = 0, hx = -1, hy = 0;    // Saved H, M, S x & y multipliers
float sdeg=0, mdeg=0, hdeg=0;
uint16_t osx=64, osy=64, omx=64, omy=64, ohx=64, ohy=64;  // Saved H, M, S x & y coords
uint16_t x0=0, x1=0, yy0=0, yy1=0;
uint32_t targetTime = 0;                    // for next 1 second timeout

static uint8_t conv2d(const char* p) {
  uint8_t v = 0;
  if ('0' <= *p && *p <= '9')
    v = *p - '0';
  return 10 * v + *++p - '0';
}

uint8_t hh=conv2d(__TIME__), mm=conv2d(__TIME__+3), ss=conv2d(__TIME__+6);  // Get H, M, S from compile time

boolean initial = 1;

void setup() {
  Serial.begin(115200);
  Serial.println("Booting");

  esp_task_wdt_init(watchdog_timeout, true);
  esp_task_wdt_add(NULL);

  roomba_start();
  setup_display();

  homeSpan.enableOTA();
  homeSpan.begin(Category::Lighting, "Roomba");
  new SpanAccessory();
  new Service::AccessoryInformation();
  new Characteristic::Name("Roomba");
  new Characteristic::Manufacturer("Josh Billions");
  new Characteristic::SerialNumber("123-ABC");
  new Characteristic::Model("Roomba");
  new Characteristic::FirmwareRevision("0.91");
  new Characteristic::Identify();

  new Service::HAPProtocolInformation();
  new Characteristic::Version("1.1.0");

  new Roomba(time_client);
}

void loop() {
  esp_task_wdt_reset();
  homeSpan.poll();

  if (homeSpan.connected) {
    if (!ntp_setup_complete) {
      time_client.begin();
      time_client.setTimeOffset(-25200);
      Serial.println("Started NTP Client");
      sync_rtc();
      ntp_setup_complete = true;
    }
  }

  if (millis() - last_refresh > 1000) {
    draw_clock();
    last_refresh = millis();
  }
}

void sync_rtc() {
  Serial.println("Syncing RTC...");
  time_client.update();

  Serial.print("NTP Time: ");
  Serial.println(time_client.getFormattedTime());

  rtc.setTime(time_client.getEpochTime());

  Serial.print("RTC Time: ");
  Serial.println(rtc.getTime("%A, %B %d %Y %H:%M:%S"));

  roomba_sync_time();
}

void setup_display() {
  tft.init();
  tft.setRotation(2);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);  // Adding a black background colour erases previous text automatically
  
  // Draw clock face
  tft.fillCircle(64, 64, 61, TFT_BLUE);
  tft.fillCircle(64, 64, 57, TFT_BLACK);

  // Draw 12 lines
  for(int i = 0; i<360; i+= 30) {
    sx = cos((i-90)*0.0174532925);
    sy = sin((i-90)*0.0174532925);
    x0 = sx*57+64;
    yy0 = sy*57+64;
    x1 = sx*50+64;
    yy1 = sy*50+64;

    tft.drawLine(x0, yy0, x1, yy1, TFT_BLUE);
  }

  // Draw 60 dots
  for(int i = 0; i<360; i+= 6) {
    sx = cos((i-90)*0.0174532925);
    sy = sin((i-90)*0.0174532925);
    x0 = sx*53+64;
    yy0 = sy*53+64;
    
    tft.drawPixel(x0, yy0, TFT_BLUE);
    if(i==0 || i==180) tft.fillCircle(x0, yy0, 1, TFT_CYAN);
    if(i==0 || i==180) tft.fillCircle(x0+1, yy0, 1, TFT_CYAN);
    if(i==90 || i==270) tft.fillCircle(x0, yy0, 1, TFT_CYAN);
    if(i==90 || i==270) tft.fillCircle(x0+1, yy0, 1, TFT_CYAN);
  }

  tft.fillCircle(65, 65, 3, TFT_RED);

  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.drawCentreString("Offline",64,140,4);
}

void draw_clock() {
  ss = rtc.getSecond();
  mm = rtc.getMinute();
  hh = rtc.getHour();

  // Pre-compute hand degrees, x & y coords for a fast screen update
  sdeg = ss * 6;                // 0-59 -> 0-354
  mdeg = mm * 6 + sdeg * 0.01666667; // 0-59 -> 0-360 - includes seconds
  hdeg = hh * 30 + mdeg * 0.0833333; // 0-11 -> 0-360 - includes minutes and seconds
  hx = cos((hdeg - 90) * 0.0174532925);
  hy = sin((hdeg - 90) * 0.0174532925);
  mx = cos((mdeg - 90) * 0.0174532925);
  my = sin((mdeg - 90) * 0.0174532925);
  sx = cos((sdeg - 90) * 0.0174532925);
  sy = sin((sdeg - 90) * 0.0174532925);

  if (ss == 0 || initial) {
    initial = 0;
    // Erase hour and minute hand positions every minute
    tft.drawLine(ohx, ohy, 65, 65, TFT_BLACK);
    ohx = hx * 33 + 65;
    ohy = hy * 33 + 65;
    tft.drawLine(omx, omy, 65, 65, TFT_BLACK);
    omx = mx * 44 + 65;
    omy = my * 44 + 65;
  }

  // Redraw new hand positions, hour and minute hands not erased here to avoid flicker
  tft.drawLine(osx, osy, 65, 65, TFT_BLACK);
  tft.drawLine(ohx, ohy, 65, 65, TFT_WHITE);
  tft.drawLine(omx, omy, 65, 65, TFT_WHITE);
  osx = sx * 47 + 65;
  osy = sy * 47 + 65;
  tft.drawLine(osx, osy, 65, 65, TFT_RED);

  tft.fillCircle(65, 65, 3, TFT_RED);

  if (homeSpan.connected) {
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.drawCentreString("Online",64,140,4);
  } else {
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.drawCentreString("Offline",64,140,4);
  }
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawCentreString(time_client.getFormattedTime(),64,180,4);
}
