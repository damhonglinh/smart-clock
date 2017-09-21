#include <SoftwareSerial.h>
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "RTClib.h"


#define LCD_LEN 16
#define CHAR_COUNT_TO_SLIDE_PER_LCD_BLINK 4
#define LCD_BLINK_INTERVAL 750
#define TEMPERATURE_INTERVAL 1000
#define ONE_WIRE_BUS 4

char MONTH_NAMES[12][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

SoftwareSerial ESPserial(2, 3); // RX | TX
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7); // 0x27 is the I2C bus address for an unmodified backpack
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
RTC_DS3231 rtc;

String wifiInput;
String nextWifiInput;
unsigned long prevMillisLcd = 0;
unsigned long prevMillisTemp = 0;
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

  preProcessTemperatureThread(currentMillis);

  if (ESPserial.available()) {
    nextWifiInput = ESPserial.readStringUntil('\n');
    Serial.println(nextWifiInput);
  }

  processLcdThread(currentMillis);
  processTemperatureThread(currentMillis);
}


// ========== LCD ===========

void processLcdThread(unsigned long currentMillis) {
  if (currentMillis - prevMillisLcd > LCD_BLINK_INTERVAL) {
    lcdPrintLongLine();
    lcdPrintDateTime();
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

void lcdPrintDateTime() {
  DateTime now = rtc.now();
  lcd.setCursor(9, 1);

  if (now.second() % 5) {
    lcdPrintTime(now);
  } else {
    lcdPrintDate(now);
  }
}

void lcdPrintDate(DateTime& now) {
  char* monthStr = MONTH_NAMES[now.month()];
  char dateStr[11];
  sprintf(dateStr, "%02d %s   ", now.day(), monthStr);

  lcd.print(dateStr);
}

void lcdPrintTime(DateTime& now) {
  byte hour = now.hour() % 12;
  char* amStr = hour == now.hour() ? "am" : "pm";
  char timeStr[11];
  sprintf(timeStr, "%02d:%02d%s", hour, now.minute(), amStr);

  lcd.print(timeStr);
}

void fillCharactersToArray(char* arr, int n, char character) {
  for (int i = 0; i < n; i++) {
    arr[i] = character;
  }
  arr[n] = 0;
}

// ========== Temperature ==========

void preProcessTemperatureThread(unsigned long currentMillis) {
  if (currentMillis - prevMillisTemp > TEMPERATURE_INTERVAL) {
    sensors.setWaitForConversion(false);  // makes it async
    sensors.requestTemperatures();
    sensors.setWaitForConversion(true);
  }
}

void processTemperatureThread(unsigned long currentMillis) {
  if (currentMillis - prevMillisTemp > TEMPERATURE_INTERVAL) {
    lcdPrintTemperature();
    prevMillisTemp = currentMillis;
  }
}

void lcdPrintTemperature() {
  float temp = sensors.getTempCByIndex(0);
  char tempStr[10];
  formatTempString(temp, tempStr);
  lcd.setCursor(0, 1);
  lcd.print(tempStr);
}

void formatTempString(float temp, char* tempStr) {
  char degreeChar = (char)223; // ° degreeSymbol
  char tempOneDecPlace[6];

  dtostrf(temp, 4, 1, tempOneDecPlace);
  sprintf(tempStr, "%s%cC", tempOneDecPlace, degreeChar);
}
