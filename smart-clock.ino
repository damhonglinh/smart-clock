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
const char* host = "https://judgelinh.ngrok.io/";
bool onOff = true;

void setup() {
  Serial.begin(115200);
  pinMode(RED_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  connectWifi();
}

void loop() {
  Serial.println("Looping...");
  ledGreen();
  yield();
  fetchWithHttp(host, "/");
  delay(10000);
}


void fetchWithHttp(const char* host, String url) {
  Serial.println("[HTTP] begin...");

  HTTPClient http;
  http.begin(host, "32 D1 EC 69 40 D3 4E 76 F8 32 89 4E 0C 4F 1C 62 2B AC 49 D9");

  Serial.println("[HTTP] GET...");

  int httpCode = http.GET();

  // httpCode will be negative on error
  if (httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);

    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
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

void ledRed()   { ledColor(255, 0, 0); }
void ledGreen() { ledColor(0, 255, 0); }
void ledBlue()  { ledColor(0, 0, 255); }

int readLDR() {
  return analogRead(LDR_PIN);
}


void connectWifi() {
  ESP8266WiFiMulti WiFiMulti;
  WiFiMulti.addAP(ssid, password);

  while (WiFiMulti.run() != WL_CONNECTED) {
    ledColor(225, 0 , 225); // purple
    delay(500);
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}
