#ifndef JUGGLING_CLUB_PATTERNS_LIBRARY
#define JUGGLING_CLUB_PATTERNS_LIBRARY

#include "jsonPacket.h"
#include <FastLED.h>
#define FASTLED_ESP8266_RAW_PIN_ORDER
FASTLED_USING_NAMESPACE

#define DATA_PIN    D4
//#define CLK_PIN   0
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    100
#define BRIGHTNESS  94

CRGB leds[NUM_LEDS];

#define STRIP_LEN 3
#define RING_LEN 5

void handle_wave(int h, int s, int v);
void start_wave();


////////////////////////////
////      Helpers       ////
////////////////////////////


void set_handle_by_height(int height, CRGB color) {
  leds[height]                     = color;
  leds[2 * STRIP_LEN - 1 - height] = color;
  leds[2 * STRIP_LEN - 1 + height] = color;
  leds[4 * STRIP_LEN - 1 - height] = color;
}

void fill_handle(CRGB color) {
  for (int i = 0; i < STRIP_LEN; i ++) {
    set_handle_by_height(i, color);
  }
}

int ring_start = 4 * STRIP_LEN;
void set_ring_by_longitude(int lon, CRGB color) {
  leds[ring_start + lon] = color;
}

void fill_ring(CRGB color) {
  for (int i = 0; i < RING_LEN; i++) {
    set_ring_by_longitude(i, color);
  }
}

const int pulseLength = 128;
const int pulseScale = 1;
void pulse_color(CRGB color) {
  int pulseAmount = abs((hueCounter % pulseLength) - (pulseLength / 2));

  for (int i = 0; i < pulseAmount; i += pulseScale) {
    color -= 3;
  }
  for (int i = pulseLength / 2; i > pulseAmount; i -= pulseScale) {
    color += 3;
  }
  fill_solid( leds, NUM_LEDS, color);
}

int multi_solid_colors[N_CLUBS] = {0, 1, 2};
int get_unique_color_id() {
  // if (packet.addons[5]) { // if the "Alternate Colors" add-on is selected:
  //   return myUniqueOrderNumber + int(int(hueCounter * N_CLUBS) / 256);
  // }
  // else { // do not alternate colors
  //   return myUniqueOrderNumber;
  // }
  return MY_UNIQUE_CLUB_ID;
}

void addGlitter( fract8 chanceOfGlitter)
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

CRGB getLinkedColor(JsonObject colorLocation) {
  JsonObject colorObj;
  if (!colorLocation["linkColors"]) {
    colorObj = colorLocation["color"][MY_UNIQUE_CLUB_ID];
  }
  else {
    colorObj = colorLocation["color"][0];
  }
  CHSV colorHsv = CHSV(colorObj["h"], colorObj["s"], colorObj["l"]);
  CRGB colorRgb;
  hsv2rgb_rainbow(colorHsv, colorRgb);
  return colorRgb;
}

////////////////////////////
////      Patterns      ////
////////////////////////////

void solid() {
  // CRGB myColor = getLinkedColor(jsonPacket[patternId]["color"]);
  // Serial.print("filling solid wtih color "); Serial.print(myColor.red); Serial.print(myColor.green); Serial.print(myColor.blue);
  fill_solid( leds, NUM_LEDS, getLinkedColor(jsonPacket[patternId]["color"]));
}

void pulse() {
  pulse_color(getLinkedColor(jsonPacket[patternId]["color"]));
}

void rainbow() // FastLED's built-in rainbow generator
{
  fill_rainbow( leds, NUM_LEDS, int(hueCounter / 50) * 50, 7);
}

void rainbowWithGlitter() // built-in FastLED rainbow, plus some random sparkly glitter
{
  rainbow();
  addGlitter(80);
}

void confetti() // random colored speckles that blink in and fade smoothly
{
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( hueCounter + random8(64), 200, 255);
}

void bpm() // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
{
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, hueCounter+(i*2), beat-hueCounter+(i*10));
  }
}

void juggle() { // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  uint8_t dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

void verticalWave() {
  solid();
  Serial.println("Inside the verticalWave function!");
}



////////////////////////////
////      Add-ins       ////
////////////////////////////


void ring_solid() {
  if (jsonPacket[patternId]["ringColor"]["on"] == false) {
    return;
  }
  fill_ring(getLinkedColor(jsonPacket[patternId]["ringColor"]));
}

const int sparkleMemoryLen = 5;
uint16_t sparkleMemory[sparkleMemoryLen];
int sparkleMemoryId = 0;

void sparkle() { // pass in a number between 0 and 255
  if (jsonPacket[patternId]["sparkleColor"]["on"] == false) {
    return;
  }
  // almost an alias for FastLED's addGlitter(), but using a user-defined color
  fract8 chanceOfGlitter = 128;
  if( random8() < chanceOfGlitter) {
    sparkleMemory[sparkleMemoryId] = random16(NUM_LEDS);
    sparkleMemoryId = (sparkleMemoryId + 1) % sparkleMemoryLen;
  }
  for (int i = 0; i < sparkleMemoryLen; i++) {
    leds[sparkleMemory[i]] = getLinkedColor(jsonPacket[patternId]["sparkleColor"]);
  }
}

int flashCounter = 0;
void flash() {
  if (jsonPacket[patternId]["flashColor"]["on"] == false) {
    return;
  }
  // first, increment the counter
  flashCounter += int(jsonPacket[patternId]["flashColor"]["speed"]);
  // check if it is over the flash duty cycle limit
  if (flashCounter > FRAMES_PER_SECOND * jsonPacket[patternId]["flashColor"]["duration"]) {
    // during the flash time, make sure the club is a solid color by setting it every frame rate
    fill_solid(leds, NUM_LEDS, getLinkedColor(jsonPacket[patternId]["flashColor"]));
  }
  // then, check if it is over the total period limit
  if (flashCounter > FRAMES_PER_SECOND * 200) {
    flashCounter = 0;
    // don't do anything to the pattern; let the base pattern shine through
  }
}

#endif