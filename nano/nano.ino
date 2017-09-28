#include <SoftwareSerial.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "RTClib.h"
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"


#define OLED_DISPLAY_ADDRESS 0x3C
#define OLED_LINE_LEN 21
#define DISPLAY_QUOTE_INTERVAL 3000
#define TEMPERATURE_INTERVAL 1000
#define ONE_WIRE_BUS 4

char MONTH_NAMES[12][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

SoftwareSerial ESPserial(2, 3); // RX | TX
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
RTC_DS3231 rtc;
SSD1306AsciiWire oled;

String wifiInput;
String nextWifiInput;
unsigned long prevMillisQuote = 0;
unsigned long prevMillisTemp = 0;
int oledIndexFrom = 0;


// ========== setup ==========

void setup() {
  Serial.begin(9600);     // communication with the host computer
  ESPserial.begin(9600);  // communication with the ESP8266

  oled.begin(&Adafruit128x64, OLED_DISPLAY_ADDRESS);
  oled.setFont(Verdana12);
  oled.clear();

  sensors.begin();
  rtc.begin();

  Serial.println("Ready\n");
}

void loop() {
  unsigned long currentMillis = millis();

  preProcessPrintingTempThread(currentMillis);

  if (ESPserial.available()) {
    nextWifiInput = ESPserial.readStringUntil('\n');
    Serial.println(nextWifiInput);
  }

  processPrintingQuotesThread(currentMillis);
  processPrintingTempThread(currentMillis);
}


// ========== Print quotes ===========

void processPrintingQuotesThread(unsigned long currentMillis) {
  if (currentMillis - prevMillisQuote > DISPLAY_QUOTE_INTERVAL) {
    oledPrintLongLine(0);
    oledPrintLongLine(1);
    prevMillisQuote = currentMillis;
  }
}

void oledPrintLongLine(int oledLine) {
  int strLen = wifiInput.length();
  int indexTo = min(oledIndexFrom + OLED_LINE_LEN, strLen);
  String subStr = wifiInput.substring(oledIndexFrom, indexTo);

  oledPrintLine(subStr, oledLine * 2);

  if (oledIndexFrom < strLen) {
    oledIndexFrom += OLED_LINE_LEN;
  } else {
    oledIndexFrom = 0;
    wifiInput = nextWifiInput;
  }
}

void oledPrintLine(String str, int oledLine) {
  oled.setCursor(0, oledLine);
  oled.print(str);
  oled.clearToEOL();
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
    oledPrintTemperature();
    oledPrintDateTime();
    oled.clearToEOL();
    prevMillisTemp = currentMillis;
  }
}

// ========== DateTime ==========

void oledPrintDateTime() {
  DateTime now = rtc.now();
  oled.setCursor(60, 4); // in the middle of row 2

  if (now.second() % 5) {
    oledPrintTime(now);
  } else {
    oledPrintDate(now);
  }
}

void oledPrintDate(DateTime& now) {
  char* monthStr = MONTH_NAMES[now.month()];
  char dateStr[11];
  sprintf(dateStr, "%02d %s   ", now.day(), monthStr);

  oled.print(dateStr);
}

void oledPrintTime(DateTime& now) {
  byte hour = now.hour() % 12;
  char* amStr = hour == now.hour() ? "am" : "pm";
  char timeStr[11];
  sprintf(timeStr, "%02d:%02d%s", hour, now.minute(), amStr);

  oled.print(timeStr);
}

// ========== Temperature ==========

void oledPrintTemperature() {
  float temp = sensors.getTempCByIndex(0);
  char tempStr[10];
  formatTempString(temp, tempStr);
  oled.setCursor(0, 4);
  oled.print(tempStr);
}

void formatTempString(float temp, char* tempStr) {
  char degreeChar = (char)223; // ° degreeSymbol
  char tempOneDecPlace[6];

  dtostrf(temp, 4, 1, tempOneDecPlace);
  sprintf(tempStr, "%s%cC", tempOneDecPlace, '*');
}
