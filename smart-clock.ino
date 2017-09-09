#include <ESP8266WiFi.h>
#define RED_PIN 15
#define GREEN_PIN 12
#define BLUE_PIN 13
#define LDR_PIN A0
#define HTTP_PORT 443
#define TIME_OUT 30000

const char* ssid = "Microsoft";
const char* password = "password";
const char* host1 = "judgelinh.ngrok.io";
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
  readFromURL(host1, "/");
  delay(10000);
}

void readFromURL(const char* host, String url) {
  Serial.print("connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClientSecure client;

  if (!connectTo(client, host)) {
    return;
  }

  Serial.println("Requesting URL: " + url);

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");

  if (!waitForConnection(client)) {
    return;
  }

  // Read all the lines of the reply from server and print them to Serial
  while (client.available()) {
    String tempMsg = client.readStringUntil('\n');
    Serial.println(tempMsg);
  }

  Serial.println("============== Closing connection");
}


bool connectTo(WiFiClientSecure& client, const char* host) {
  if (!client.connect(host, HTTP_PORT)) {
    Serial.println("Connection FAILED");
    ledRed();
    yield();
    return false;
  }
  return true;
}

bool waitForConnection(WiFiClientSecure& client) {
  unsigned long timeout = millis();
  while (client.available() == 0) {
    yield();

    if (millis() - timeout > TIME_OUT) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      ledColor(255, 160, 176); // pink
      return false;
    }
  }
  return true;
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
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    ledColor(225, 0 , 225); // purple
    delay(500);
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}
