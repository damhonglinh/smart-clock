#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

#define RED_PIN 15
#define GREEN_PIN 12
#define BLUE_PIN 13
#define LDR_PIN A0
#define HTTP_PORT 443
#define TIME_OUT 30000

const char* ssid = "Microsoft";
const char* password = "password";
bool onOff = true;
int lightLevel;


void setup() {
  Serial.begin(9600);
  pinMode(RED_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);

  pinMode(LDR_PIN, INPUT_PULLUP);
  connectWifi();
}

void loop() {
  sensorLight();

  delay(1000);
}



// ==========================================================================

void sensorLight() {
  lightLevel = analogRead(LDR_PIN);

  Serial.print("lightLevel: "); Serial.println(lightLevel);

  yield();

  if (lightLevel < 50) {
    ledGreen();
  } else {
    ledOff();
  }
}

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




void connectWifi() {
  ESP8266WiFiMulti WiFiMulti;
  WiFiMulti.addAP(ssid, password);
  while (WiFiMulti.run() != WL_CONNECTED) {
    ledColor(225, 165 , 0); // orange
    delay(500);
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}
