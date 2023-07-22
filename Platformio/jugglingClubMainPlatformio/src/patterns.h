#ifndef JUGGLING_CLUB_PATTERNS_LIBRARY
#define JUGGLING_CLUB_PATTERNS_LIBRARY

#include <FastLED.h>
#define FASTLED_ESP8266_RAW_PIN_ORDER
FASTLED_USING_NAMESPACE

#define DATA_PIN    D4
//#define CLK_PIN   0
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    30
#define BRIGHTNESS          94

CRGB leds[NUM_LEDS];

int strip_len = 3;
int ring_len = 5;

int patternId = 0;

void handle_wave(int h, int s, int v);
void start_wave();


////////////////////////////
////      Helpers       ////
////////////////////////////


void set_handle_by_height(int height, CRGB color) {
  leds[height]                     = color;
  leds[2 * strip_len - 1 - height] = color;
  leds[2 * strip_len - 1 + height] = color;
  leds[4 * strip_len - 1 - height] = color;
}

void fill_handle(CRGB color) {
  for (int i = 0; i < strip_len; i ++) {
    set_handle_by_height(i, color);
  }
}

int ring_start = 4 * strip_len;
void set_ring_by_longitude(int lon, CRGB color) {
  leds[ring_start + lon] = color;
}

void fill_ring(CRGB color) {
  for (int i = 0; i < ring_len; i++) {
    set_ring_by_longitude(i, color);
  }
}

void pulse_color(CRGB color) {
  int pulseLength = 128;
  int pulseAmount = abs((gHue % pulseLength) - (pulseLength / 2));
  int pulseScale = 1;

  for (int i = 0; i < pulseAmount; i += pulseScale) {
    color -= 3;
  }
  for (int i = pulseLength / 2; i > pulseAmount; i -= pulseScale) {
    color += 3;
  }
  fill_solid( leds, NUM_LEDS, color);
}

int multi_solid_colors[nClubs] = {0, 1, 2};
int get_unique_color_id() {
  if (packet.addons[5]) { // if the "Alternate Colors" add-on is selected:
    return myUniqueOrderNumber + int(int(gHue * nClubs) / 256);
  }
  else { // do not alternate colors
    return myUniqueOrderNumber;
  }
}

void addGlitter( fract8 chanceOfGlitter)
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

CRGB getLinkedColor(JsonObject colorLocation) {
  JsonObject colorHsv;
  if (colorLocation["linkColors"]) {
    colorObj = colorLocation["color"][MY_UNIQUE_CLUB_ID];
  }
  else {
    colorObj = colorLocation["color"][0];
  }
  CHSV colorHsv = CHSV(colorObj["h"], colorObj["s"], colorObj["v"]);
  CRGB colorRgb;
  hsv2rgb_rainbow(hsv, rgb);
  return colorRgb;
}

////////////////////////////
////      Patterns      ////
////////////////////////////

void solid() {
  fill_solid( leds, NUM_LEDS, getLinkedColor(jsonPacket[patternId]["color"]));
}

void pulse() {
  pulse_color(jsonPacket[patternId]["color"]);
}

void rainbow() // FastLED's built-in rainbow generator
{
  fill_rainbow( leds, NUM_LEDS, int(gHue / 50) * 50, 7);
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
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void bpm() // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
{
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
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
  flashCounter += jsonPacket[patternId]["flashColor"]["speed"];
  // check if it is over the flash duty cycle limit
  if (flashCounter > FRAMES_PER_SECOND * jsonPacket[patternId]["flashColor"]["duration"]) {
    // during the flash time, make sure the club is a solid color by setting it every frame rate
    fill_solid(getLinkedColor(jsonPacket[patternId]["flashColor"]));
  }
  // then, check if it is over the total period limit
  if (flashCounter > FRAMES_PER_SECOND * 200) {
    flashCounter = 0;
    // don't do anything to the pattern; let the base pattern shine through
  }
}

////////////////////////////
////  LED Driver code   ////
////////////////////////////

void fastLedSetup() {
  // tell FastLED about the LED strip configuration
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
}

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
// SimplePatternList gPatterns = { solid, rainbowWithGlitter, confetti, sinelon, juggle, bpm, rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm, rainbowWithGlitter, confetti, juggle};
SimplePatternList gPatterns = { pulse, solid, solid, rainbowWithGlitter, confetti, juggle, bpm, solid, solid, solid, solid, solid, solid, solid, solid };
SimplePatternList gAddins = { ring_solid, sparkle, flash};

void updateLeds() {
    // Call the current pattern function once, updating the 'leds' array
    String patternName = jsonPacket[patternId]["displayName"];
    switch(patternName) {
      case "Vertical Wave":
        verticalWave();
        break;
      case "Pulsing Color":
        pulse();
        break;
      case "BPM":
        bpm();
        break;
      default:
        solid();
    }
  }

  // then apply all of the addins. If they are not enabled, the function will handle that
  ring_solid();
  sparkle();
  flash();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();
}





#endif