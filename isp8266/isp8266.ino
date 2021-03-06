#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#define RED_PIN 15
#define GREEN_PIN 12
#define BLUE_PIN 13
#define LDR_PIN A0
#define HTTP_PORT 443
#define TIME_OUT 30000

const char* ssid = "Microsoft";
const char* password = "password";
const char* host = "http://api.forismatic.com/api/1.0/?method=getQuote&format=json&lang=en";
const char* fingerPrint = "32 D1 EC 69 40 D3 4E 76 F8 32 89 4E 0C 4F 1C 62 2B AC 49 D9";
bool onOff = true;

void setup() {
  Serial.begin(9600);
  pinMode(RED_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  connectWifi();
}

void loop() {
  ledGreen();
  yield();
  httpGet(host);
  blinkLed(0, 0, 255, 500, 7); // blue
}

// ==========================================================================

void httpGet(const char* url) {
  HTTPClient http;
  http.begin(url);
  int httpCode = http.GET();

  if (httpCode > 0) {
    String payload = http.getString();
    yield();
    parseJson(payload, "quoteText");
    blinkLed(0, 255, 0, 100, 7);
  } else {
    Serial.printf("Failed, ERROR: %s\n", http.errorToString(httpCode).c_str());
    blinkLed(255, 0, 0, 200, 10);
  }

  http.end();
}

// void httpPost(const char* url) {
//   HTTPClient http;
//   http.begin(url, fingerPrint);
//   http.addHeader("Content-Type", "application/x-www-form-urlencoded");
//   http.POST("title=foo&body=bar&userId=1");
//   http.writeToStream(&Serial);
//   http.end();
// }


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

const char* parseJson(String& json, const char* key) {
  StaticJsonBuffer<512> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  if (!root.success()) {
    Serial.println("parseJson() failed");
  }
  const char* result = root[key];
  Serial.println(result);
  return result; // NOTE: chars returned here don't work
}


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



// TODO
// + Integrate Wunderlist
// + https://theysaidso.com/api/
