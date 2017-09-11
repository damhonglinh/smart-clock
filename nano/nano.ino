#include <SoftwareSerial.h>
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

SoftwareSerial ESPserial(2, 3); // RX | TX
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7); // 0x27 is the I2C bus address for an unmodified backpack

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

  lcd.setCursor(0,0);
  lcd.print(wifiInput);

  lcd.setCursor(0,1);
  lcd.print("Hi back!");
  delay(2000);
}
