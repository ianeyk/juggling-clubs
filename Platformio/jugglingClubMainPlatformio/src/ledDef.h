#pragma once

#include <FastLED.h>
#define FASTLED_ESP8266_RAW_PIN_ORDER
#define FASTLED_ALLOW_INTERRUPTS 0
// #define FASTLED_INTERRUPT_RETRY_COUNT 5
FASTLED_USING_NAMESPACE

#define DATA_PIN D2
// #define CLK_PIN   0
#define LED_TYPE WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS 100
#define BRIGHTNESS 94

CRGB leds[NUM_LEDS];
