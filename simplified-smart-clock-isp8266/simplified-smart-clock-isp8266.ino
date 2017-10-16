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

#define RED_PIN 15
#define GREEN_PIN 12
#define BLUE_PIN 13
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


// ========== setup ==========

void setup() {
  Serial.begin(9600);
  pinMode(RED_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);

  sensors.begin();
  yield();
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
  ledGreen();
  // preProcessPrintingTempThread(currentMillis);
  yield();

  processPrintTime(currentMillis);
  processPrintingTempThread(currentMillis);
}

// ========== Print Time Display ==========

void processPrintTime(unsigned long currentMillis) {
  DateTime now = rtc.now();
  byte hour = now.hour() % 12;
  byte minute = now.minute();

  timeDisplay.showNumberDec(hour, true, 2, 0);
  timeDisplay.showNumberDec(minute, true, 2, 2);
  Serial.print(hour);
  Serial.print(":");
  Serial.println(minute);
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
    oled.set1X();
    oledPrintDateTime();

    oled.set2X();
    oled.setCursor(28, 3);
    oledPrintTemperature();

    oled.clearToEOL();
    prevMillisTemp = currentMillis;
  }
}

// ========== Print DateTime ==========

void oledPrintDateTime() {
  DateTime now = rtc.now();
  oled.setCursor(12, 0);
  oledPrintTime(now);

  oled.setCursor(78, 0);
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
  sprintf(timeStr, "%02d : %02d%s", hour, now.minute(), amStr);

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
  sprintf(tempStr, "%s%cC", tempOneDecPlace, '*');
}



// ========== LED color ==========

void ledColor(int red, int green, int blue) {
  if (onOff) {
    analogWrite(RED_PIN, red);
    analogWrite(GREEN_PIN, green);
    analogWrite(BLUE_PIN, blue);
  } else {
    ledOff();
  }
}

void ledOff() {
  analogWrite(RED_PIN, 0);
  analogWrite(GREEN_PIN, 0);
  analogWrite(BLUE_PIN, 0);
}

void blinkLed(int red, int green, int blue, int timeout, int repeat) {
  for (int i = 0; i <= repeat; i++) {
    ledColor(red, green, blue);
    delay(timeout);
    ledOff();
    delay(timeout);
 }
}

void ledRed()   { ledColor(255, 0, 0); }
void ledGreen() { ledColor(0, 255, 0); }
void ledBlue()  { ledColor(0, 0, 255); }
