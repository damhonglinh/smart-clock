#include <SoftwareSerial.h>
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

#define LCD_LEN 16
#define CHAR_COUNT_TO_SLIDE_PER_LCD_BLINK 4
#define LCD_BLINK_INTERVAL 550

SoftwareSerial ESPserial(2, 3); // RX | TX
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7); // 0x27 is the I2C bus address for an unmodified backpack
// String currentText;

void setup() {
  Serial.begin(9600);     // communication with the host computer
  ESPserial.begin(9600);  // communication with the ESP8266

  lcd.begin(16, 2); // for 16 x 2 LCD module
  lcd.setBacklightPin(3, POSITIVE);
  lcd.setBacklight(HIGH);

  Serial.println("Ready\n");
}

void loop() {
  String wifiInput;
  if (ESPserial.available()) {
    wifiInput = ESPserial.readStringUntil('\n');
    Serial.println(wifiInput);
  }


  lcdPrintLongLine(wifiInput, 0);
  delay(1000);
}


// ========== LCD ===========

void lcdPrintLongLine(String str, int lcdLine) {
  String subStr;
  int strLen = str.length();
  int lcdLen = strLen - LCD_LEN / 2;
  int indexFrom = 0;
  int indexTo = 0;

  while (indexFrom < lcdLen) {
    indexTo = min(indexFrom + LCD_LEN, strLen);
    subStr = str.substring(indexFrom, indexTo);
    lcdPrintLine(subStr, lcdLine);

    indexFrom += CHAR_COUNT_TO_SLIDE_PER_LCD_BLINK;
    delay(LCD_BLINK_INTERVAL);
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

void fillCharactersToArray(char* arr, int n, char character) {
  for (int i = 0; i < n; i++) {
    arr[i] = character;
  }
  arr[n] = 0;
}
