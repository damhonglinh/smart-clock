#include <SoftwareSerial.h>
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "RTClib.h"


#define LCD_LEN 16
#define CHAR_COUNT_TO_SLIDE_PER_LCD_BLINK 4
#define LCD_BLINK_INTERVAL 600
#define ONE_WIRE_BUS 7

SoftwareSerial ESPserial(2, 3); // RX | TX
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7); // 0x27 is the I2C bus address for an unmodified backpack
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
RTC_DS3231 rtc;

String wifiInput;
String nextWifiInput;
unsigned long prevMillisLcd = 0;
int lcdIndexFrom = 0;


// ========== setup ==========

void setup() {
  Serial.begin(9600);     // communication with the host computer
  ESPserial.begin(9600);  // communication with the ESP8266

  lcd.begin(16, 2); // for 16 x 2 LCD module
  lcd.setBacklightPin(3, POSITIVE);
  lcd.setBacklight(HIGH);

  sensors.begin();
  rtc.begin();

  Serial.println("Ready\n");
}

void loop() {
  unsigned long currentMillis = millis();

  if (ESPserial.available()) {
    nextWifiInput = ESPserial.readStringUntil('\n');
    Serial.println(nextWifiInput);
  }

  processLcdThread(currentMillis);
}


// ========== LCD ===========

void processLcdThread(unsigned long currentMillis) {
  if (currentMillis - prevMillisLcd > LCD_BLINK_INTERVAL) {
    lcdPrintLongLine();
    lcdPrintTemperature();
    lcdPrintTime();
    prevMillisLcd = currentMillis;
  }
}

void lcdPrintLongLine() {
  int strLen = wifiInput.length();
  int lcdLen = strLen - LCD_LEN / 2;
  int indexTo = min(lcdIndexFrom + LCD_LEN, strLen);
  String subStr = wifiInput.substring(lcdIndexFrom, indexTo);

  lcdPrintLine(subStr, 0);

  if (lcdIndexFrom < lcdLen) {
    lcdIndexFrom += CHAR_COUNT_TO_SLIDE_PER_LCD_BLINK;
  } else {
    lcdIndexFrom = 0;
    wifiInput = nextWifiInput;
  }
}

void lcdPrintLine(String str, int lcdLine) {
  int padding = LCD_LEN - str.length();
  if (padding > 0) {
    char padStr[padding + 1];
    fillCharactersToArray(padStr, padding, ' ');
    str.concat(padStr);
  }

  lcd.setCursor(0, lcdLine);
  lcd.print(str);
}

void lcdPrintTemperature() {
  float temp = getTemperature();
  char tempStr[10];
  formatTempString(temp, tempStr);
  lcd.setCursor(0, 1);
  lcd.print(tempStr);
}

void lcdPrintTime() {
  DateTime now = rtc.now();
  byte hour = now.hour() % 12;
  bool showColon = now.second() % 2 == 0;
  char colon = showColon ? ':' : ' ';

  char timeStr[11];
  sprintf(timeStr, "%02d %02d%c%02d", now.day(), hour, colon, now.minute());

  lcd.setCursor(8, 1);
  lcd.print(timeStr);
}

void fillCharactersToArray(char* arr, int n, char character) {
  for (int i = 0; i < n; i++) {
    arr[i] = character;
  }
  arr[n] = 0;
}

// ========== Temperature ==========

float getTemperature() {
  sensors.requestTemperatures();
  return sensors.getTempCByIndex(0);
}

void formatTempString(float temp, char* tempStr) {
  char degreeChar = (char)223; // ° degreeSymbol
  char tempOneDecPlace[6];

  dtostrf(temp, 4, 1, tempOneDecPlace);
  sprintf(tempStr, "%s%cC", tempOneDecPlace, degreeChar);
}
