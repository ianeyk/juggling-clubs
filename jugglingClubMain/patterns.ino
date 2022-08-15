#ifndef JUGGLING_CLUB_PATTERNS_LIBRARY
#define JUGGLING_CLUB_PATTERNS_LIBRARY

////////////////////////////
////      Patterns      ////
////////////////////////////

void solid() {
  fill_solid( leds, NUM_LEDS, packet.colors[0]);
}

void pulse() {
  CRGB myColor = CRGB(packet.colors[0]);
  int pulseLength = 128;
  int pulseAmount = abs((gHue % pulseLength) - (pulseLength / 2));
  int pulseScale = 1;

  for (int i = 0; i < pulseAmount; i += pulseScale) {
    myColor -= 3;
    // myColor /= 1.5;
  }
  for (int i = pulseLength / 2; i > pulseAmount; i -= pulseScale) {
    myColor += 3;
  }

  fill_solid( leds, NUM_LEDS, myColor);
}

void rainbow()
{
  // FastLED's built-in rainbow generator

  fill_rainbow( leds, NUM_LEDS, int(gHue / 50) * 50, 7);
}

void rainbowWithGlitter()
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter)
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti()
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

/*
void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS-1 );
  leds[pos] += CHSV( gHue, 255, 192);
}
*/

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
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
  fill_ring(packet.colors[3]);
}

const int sparkleMemoryLen = 5;
uint16_t sparkleMemory[sparkleMemoryLen];
int sparkleMemoryId = 0;
void sparkle() { // pass in a number between 0 and 255
  // almost an alias for FastLED's addGlitter(), but using a user-defined color
  fract8 chanceOfGlitter = 128;
  if( random8() < chanceOfGlitter) {
    sparkleMemory[sparkleMemoryId] = random16(NUM_LEDS);
    sparkleMemoryId = (sparkleMemoryId + 1) % sparkleMemoryLen;
  }
  for (int i = 0; i < sparkleMemoryLen; i++) {
    leds[sparkleMemory[i]] = packet.colors[5];
  }
}


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

#endif