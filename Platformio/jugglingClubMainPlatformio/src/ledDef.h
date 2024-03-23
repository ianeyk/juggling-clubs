#pragma once

#include <FastLED.h>
#define FASTLED_ESP8266_RAW_PIN_ORDER
FASTLED_USING_NAMESPACE

#define DATA_PIN D4
// #define CLK_PIN   0
#define LED_TYPE WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS 50
#define BRIGHTNESS 94

CRGB leds[NUM_LEDS];
