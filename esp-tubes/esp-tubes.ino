#include <NeoPixelBus.h>
#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>

WebSocketsClient webSocket;

// length of 6 foot pixels is 226 units (113 on each side)
const uint16_t PixelCount = 222;

// units to ignore at the beginning and end
// we ignore X pixels, meaning our actual length is 111 on each side
const uint8_t offsetStart = 0;

// we further ignore the address as the first character
const uint8_t addressCharacters = 1;

const uint8_t PixelPin = 2;

NeoPixelBus<NeoGrbFeature, NeoWs2812xMethod> strip(PixelCount, PixelPin);

#ifndef STASSID
#define STASSID "NETGEAR48"
#define STAPSK "widerabbit310"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;
IPAddress assignedIpAddress;
String assignedIpAddressString;

void printPayload(uint8_t* payload, size_t length) {

  uint8_t firstCharacter = payload[0];
  if (firstCharacter == assignedIpAddress[3]) {
    for (size_t i = 0; i < 111 * 3; i += 3) {
      int unitNumber = i / 3 + offsetStart;
      strip.SetPixelColor(unitNumber, RgbColor(payload[i + 1 + addressCharacters], payload[i + 1 + addressCharacters], payload[i + 2 + addressCharacters]));
      strip.SetPixelColor(PixelCount - unitNumber - 1, RgbColor(payload[i + addressCharacters], payload[i + 1 + addressCharacters], payload[i + 2 + addressCharacters]));
    }
  }
  strip.Show();
}

void webSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.println("[WSc] Disconnected!\n");
      for (int i = 0; i < PixelCount; i++) {
        strip.SetPixelColor(i, RgbColor(50, 0, 0));
      }
      strip.Show();
      // connected = false;
      break;
    case WStype_CONNECTED:
      {
        Serial.println("Connected!");
        for (int i = 0; i < PixelCount; i++) {
          strip.SetPixelColor(i, RgbColor(0, 50, 0));
        }
        strip.Show();
        delay(500);

        // Turn all LED strips off
        for (int i = 0; i < PixelCount; i++) {
          strip.SetPixelColor(i, RgbColor(0, 0, 0));
        }
        strip.Show();

        // send message to server when Connected
        Serial.println("[WSc] SENT: Connected");
        webSocket.sendTXT("Test");
        webSocket.sendTXT("Connected");
        webSocket.sendTXT(assignedIpAddressString);
      }
      break;
    case WStype_TEXT:
      Serial.println("got text");
      printPayload(payload, length);
      break;
    case WStype_BIN:
      Serial.println("got bin");
      printPayload(payload, length);
      break;
      // Serial.println(length);
      // hexdump(payload, length);
      break;
    case WStype_PING:
      // pong will be send automatically
      Serial.println("[WSc] get ping\n");
      break;
    case WStype_PONG:
      // answer to a ping we send
      Serial.println("[WSc] get pong\n");
      break;
  }
}

void setup() {
  Serial.begin(115200);

  // this resets all the neopixels to an off state
  strip.Begin();
  strip.Show();

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");

    for (int i = 0; i < PixelCount; i++) {
      strip.SetPixelColor(i, RgbColor(80, 30, 40));
    }
    strip.Show();

    delay(200);
    for (int i = 0; i < PixelCount; i++) {
      strip.SetPixelColor(i, RgbColor(0, 0, 0));
    }
    strip.Show();
  }

  for (int i = 0; i < PixelCount; i++) {
    strip.SetPixelColor(i, RgbColor(50, 50, 0));
  }
  strip.Show();

  Serial.println("");
  Serial.println("WiFi connected");

  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  assignedIpAddress = WiFi.localIP();
  assignedIpAddressString = assignedIpAddress.toString();

  Serial.println("Attempting websocket connect...");
  webSocket.begin("192.168.1.3", 8080);
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  webSocket.loop();
}
