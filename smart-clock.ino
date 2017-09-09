#include <ESP8266WiFi.h>
int redPin = 15;
int greenPin = 12;
int bluePin = 13;
int LDRPin = A0;
const char* ssid = "Microsoft";
const char* password = "tivitulanh";

const char* host1 = "judgelinh.ngrok.io";
boolean onOff = true;

void setup() {
  Serial.begin(115200);
  pinMode(redPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  connectWifi();
}

void loop() {
  Serial.println("Looping...");
  ledGreen();
  int LDRReading = 0;
  LDRReading = analogRead(LDRPin);
  yield();
  ReadFromURL(host1, 443, "/");
  delay(10000);
}

void ReadFromURL(const char* host, int httpPort, String url) {
  Serial.print("connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClientSecure client;

  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    ledRed();
    yield();
    return;
  }

  // We now create a URI for the request
  Serial.print("Requesting URL: ");
  Serial.println(url);

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }

  // Read all the lines of the reply from server and print them to Serial
  while (client.available()) {
    String tempMsg = client.readStringUntil('\n');
    if (tempMsg.length() == 1) { //ignores the headers
      if (tempMsg == "0") {
        onOff = false;
        Serial.println(onOff);
      }
      if (tempMsg == "1") {
        onOff = true;
        Serial.println(onOff);
      }
    }

    Serial.println(tempMsg);
  }

  Serial.println();
  Serial.println("closing connection");
}

void ledColor(int red, int green, int blue) {
  if (onOff == true) {
    analogWrite(redPin, red);
    analogWrite(greenPin, green);
    analogWrite(bluePin, blue);
  }
}
void ledRed()   { ledColor(255, 0, 0); }
void ledGreen() { ledColor(0, 255, 0); }
void ledBlue()  { ledColor(0, 0, 255); }



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
