#include <FastLED.h>

#define NUM_LEDS 222  // Total number of LEDs in the strip

#define DATA_PIN 2    // Pin connected to the LED data line

CRGB leds[NUM_LEDS];

void setup() {
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);  // Configure LED strip
  FastLED.setBrightness(100);  // Set brightness (0-255)

  // Turn on LEDs 1, 111, 112, and 222
  leds[0] = CRGB::Red;    // LED 1
  leds[110] = CRGB::Green;  // LED 111
  leds[111] = CRGB::Blue;   // LED 112
  leds[221] = CRGB::Yellow; // LED 222

  FastLED.show();  // Update LEDs
}

void loop() {
  // Your code here
}