#ifndef JUGGLING_CLUB_PATTERNS_LIBRARY
#define JUGGLING_CLUB_PATTERNS_LIBRARY

void solid() {
  fill_solid( leds, NUM_LEDS, packet.colors[0]);
}

void sparkle() {
  fill_solid( leds, NUM_LEDS, packet.colors[1]);
  addGlitter(80);
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

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS-1 );
  leds[pos] += CHSV( gHue, 255, 192);
}

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

int handle_wave_pos = 0;
void handle_wave(int h, int s, int v) {
  set_handle(handle_wave_pos, 0, 0, 0); // set the previous LED to off
  handle_wave_pos = (handle_wave_pos + 1) % strip_len;
  set_handle(handle_wave_pos, h, s, v); // set the next LED to on
}

float better_handle_wave_pos = 0;
int handle_wave_val(int x) {
  // return floor(pow(2, -abs(better_handle_wave_pos - float(x))) * 255);
  float d = better_handle_wave_pos - float(x) + 0.005;
  // return floor(((sin(d) / d) + 0.25) * 1 / 1.25 * 255);
  return floor(max(cos(d), double(0)) * 255);
}

void better_handle_wave(int h, int s, int v) {
  for (int i = 0; i < strip_len; i++) {
    if (abs(better_handle_wave_pos - i) < 2) {
      set_handle(i, h, s, handle_wave_val(i));
    }
    else {
      set_handle(i, h, s, 0);
    }
  }
  better_handle_wave_pos += 0.05;
  if (better_handle_wave_pos >= strip_len + 2) {
    better_handle_wave_pos = -2;
  }
}

void set_handle(int height, int h, int s, int v) {
  leds[height] = CHSV(h, s, v);
  leds[2 * strip_len - 1 - height] = CHSV(h, s, v);
  leds[2 * strip_len - 1 + height] = CHSV(h, s, v);
  leds[4 * strip_len - 1 - height] = CHSV(h, s, v);
}

void set_ring(int h, int s, int v) {
  for (int i = 0; i < ring_len; i++) {
    leds[4 * strip_len + i] = CHSV(h, s, v);
  }
}

float longit_wave_pos = 0;
int longit_wave_val(float angle) {
  float d = fmod(longit_wave_pos - angle, TWO_PI);
  if ((0 < d && d < HALF_PI) || 3 * HALF_PI < d && d < TWO_PI) {
    return floor(cos(d) * 255);
  } //else
  return 0;
}

void longitudinal_wave(int h, int s, int v) {
  // set the vertical strips
  for (int handle_strip = 0; handle_strip < 4; handle_strip++) {
    for (int i = 0; i < strip_len; i++) {
      leds[handle_strip * strip_len + i] = CHSV(h, s, longit_wave_val(handle_strip * HALF_PI));
    }
  }
  // set the ring
  for (int i = 0; i < ring_len; i++) {
    leds[4 * strip_len + i] = CHSV(h, s, longit_wave_val(i * TWO_PI / ring_len));
  }
  longit_wave_pos += 0.08;
  if (longit_wave_pos > TWO_PI) {
    longit_wave_pos = 0;
  }
}

#endif