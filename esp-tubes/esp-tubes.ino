#include <NeoPixelBus.h>
#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>

WebSocketsClient webSocket;

const uint16_t PixelCount = 38;
const uint8_t PixelPin = 2;

NeoPixelBus<NeoGrbFeature, NeoWs2812xMethod> strip(PixelCount, PixelPin);

#ifndef STASSID
#define STASSID "NETGEAR48"
#define STAPSK "widerabbit310"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

const char* host = "djxmmx.net";
const uint16_t port = 17;

void printPayload(uint8_t* payload, size_t length) {
  Serial.println("Length...");
  Serial.println(length);
  for (size_t i = 0; i < length; i += 3) {
    Serial.print(payload[i], HEX);
    Serial.print(' ');
    strip.SetPixelColor(i / 3, RgbColor(payload[i], payload[i + 1], payload[i + 2]));
  }
  strip.Show();
  Serial.println();
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
        // connected = true;

        // send message to server when Connected
        Serial.println("[WSc] SENT: Connected");
        webSocket.sendTXT("Connected");
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


  Serial.println("Attempting websocket connect...");
  webSocket.begin("192.168.1.3", 8080);
  webSocket.onEvent(webSocketEvent);
}



void loop() {

  webSocket.loop();

  // static bool wait = false;

  // Serial.print("connecting to ");
  // Serial.print(host);
  // Serial.print(':');
  // Serial.println(port);

  // // Use WiFiClient class to create TCP connections
  // WiFiClient client;
  // if (!client.connect(host, port)) {
  //   Serial.println("connection failed");
  //   delay(5000);
  //   return;
  // }

  // // This will send a string to the server
  // Serial.println("sending data to server");
  // if (client.connected()) { client.println("hello from ESP8266"); }

  // // wait for data to be available
  // unsigned long timeout = millis();
  // while (client.available() == 0) {
  //   if (millis() - timeout > 5000) {
  //     Serial.println(">>> Client Timeout !");
  //     client.stop();
  //     delay(60000);
  //     return;
  //   }
  // }

  // // Read all the lines of the reply from server and print them to Serial
  // Serial.println("receiving from remote server");
  // // not testing 'client.connected()' since we do not need to send data here
  // while (client.available()) {
  //   char ch = static_cast<char>(client.read());
  //   Serial.print(ch);
  // }

  // // Close the connection
  // Serial.println();
  // Serial.println("closing connection");
  // client.stop();

  // if (wait) {
  //   delay(300000);  // execute once every 5 minutes, don't flood remote service
  // }
  // wait = true;
}
