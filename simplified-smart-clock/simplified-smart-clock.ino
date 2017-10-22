#include <SoftwareSerial.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "RTClib.h"
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#include <TM1637Display.h>


#define OLED_DISPLAY_ADDRESS 0x3C
#define OLED_LINE_LEN 21
#define DISPLAY_QUOTE_INTERVAL 3000
#define TEMPERATURE_INTERVAL 1000

#define ONE_WIRE_BUS 7
#define CLK 11
#define DIO 10

char MONTH_NAMES[12][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
RTC_DS3231 rtc;
SSD1306AsciiWire oled;
TM1637Display timeDisplay(CLK, DIO);

unsigned long prevMillisTemp = 0;
unsigned long prevSecond = 0;


// ========== setup ==========

void setup() {
  Serial.begin(9600);

  sensors.begin();
  rtc.begin();
  setupOledDisplay();
  setupTimeDisplay();

  Serial.println("Ready\n");
}

void setupOledDisplay() {
  Wire.begin();
  oled.begin(&Adafruit128x64, OLED_DISPLAY_ADDRESS);
  oled.clear();
  oled.setFont(Verdana12);
}

void setupTimeDisplay() {
  timeDisplay.setBrightness(2);
  uint8_t data[] = { 0xff, 0xff, 0xff, 0xff };
  timeDisplay.setSegments(data);
}

// ========== LOOP ==========

void loop() {
  unsigned long currentMillis = millis();
  preProcessPrintingTempThread(currentMillis);

  processPrintTime();
  processPrintTimeLed();
  processPrintingTempThread(currentMillis);
}

// ========== Print Time Display ==========

void processPrintTime() {
  DateTime now = rtc.now();
  byte hour24 = now.hour();
  byte hour = hour24 % 12;
  byte second = now.second();

  if (hour24 > 6 && hour24 < 18) {
    timeDisplay.setBrightness(5);
  } else {
    timeDisplay.setBrightness(1);
  }

  int result = hour * 100 + now.minute();
  int dot = (0x80 >> (second % 2)); // odd seconds => show dot; even seconds => hide dot

  timeDisplay.showNumberDecEx(result, dot, true, 4, 0);
}

// ========== Print extra info ==========

void preProcessPrintingTempThread(unsigned long currentMillis) {
  if (currentMillis - prevMillisTemp > TEMPERATURE_INTERVAL) {
    sensors.setWaitForConversion(false);  // makes it async
    sensors.requestTemperatures();
    sensors.setWaitForConversion(true);
  }
}

void processPrintingTempThread(unsigned long currentMillis) {
  if (currentMillis - prevMillisTemp > TEMPERATURE_INTERVAL) {
    oled.set2X();
    oled.setCursor(32, 3);
    oledPrintTemperature();

    oled.clearToEOL();
    prevMillisTemp = currentMillis;
  }
}

// ========== Print DateTime ==========

void processPrintTimeLed() {
  DateTime now = rtc.now();
  byte currentSecond = now.second();

  if (currentSecond != prevSecond) {
    oled.set1X();
    oledPrintDateTime(now);
    prevSecond = currentSecond;
  }
}

void oledPrintDateTime(DateTime now) {
  oled.setCursor(10, 0);
  oledPrintTime(now);

  oled.setCursor(84, 0);
  oledPrintDate(now);
}

void oledPrintDate(DateTime& now) {
  char* monthStr = MONTH_NAMES[now.month() - 1];
  char dateStr[11];
  sprintf(dateStr, "%02d %s", now.day(), monthStr);

  oled.print(dateStr);
}

void oledPrintTime(DateTime& now) {
  byte hour = now.hour() % 12;
  char* amStr = hour == now.hour() ? "am" : "pm";
  char timeStr[11];
  sprintf(timeStr, "%02d:%02d:%02d%s", hour, now.minute(), now.second(), amStr);

  oled.print(timeStr);
}

// ========== Temperature ==========

void oledPrintTemperature() {
  float temp = sensors.getTempCByIndex(0);
  char tempStr[10];
  formatTempString(temp, tempStr);
  oled.print(tempStr);
}

void formatTempString(float temp, char* tempStr) {
  // char degreeChar = (char)223; // ° degreeSymbol
  char tempOneDecPlace[6];

  dtostrf(temp, 4, 1, tempOneDecPlace);
  sprintf(tempStr, "%s%cC", tempOneDecPlace, ' ');
}
