#define FASTLED_ESP8266_RAW_PIN_ORDER

// #include <TaskScheduler.h>
#include <FastLED.h>

FASTLED_USING_NAMESPACE

// FastLED "100-lines-of-code" demo reel, showing just a few
// of the kinds of animation patterns you can quickly and easily
// compose using FastLED.
//
// This example also shows one easy way to define multiple
// animations patterns and have them automatically rotate.
//
// -Mark Kriegsman, December 2014

#define PI 3.1415926535897932384626433832795
#define HALF_PI 1.5707963267948966192313216916398
#define TWO_PI 6.283185307179586476925286766559

#define DATA_PIN    4
//#define CLK_PIN   0
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    66
CRGB leds[NUM_LEDS];

#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120

int strip_len = 12;
int ring_len = 18;

void handle_wave(int h, int s, int v);
void start_wave();

// Task task_handle_wave(1000 / FRAMES_PER_SECOND, TASK_FOREVER, &handle_wave(255, 255, 255));
// Task updateLEDs(100, strip_len, &FastLED.show);
// Task task_start_wave(3000, TASK_FOREVER, &start_wave);
// Scheduler runner;

void setup() {
  Serial.begin(115200);
  delay(3000); // 3 second delay for recovery

  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
  // runner.addTask(updateLEDs);
  // runner.add_task(task_start_wave);
}


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

// void start_wave() {
//   runner.add_task(task_handle_wave);
// }

void loop()
{
  // Call the current pattern function once, updating the 'leds' array
  // gPatterns[gCurrentPatternNumber]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND);

  // do some periodic updates
  // EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
  // EVERY_N_SECONDS( 10 ) { nextPattern(); } // change patterns periodically
  // better_handle_wave(gHue, 255, 255);
  longitudinal_wave(0, 255, 255);
  // set_ring(gHue, 255, 255);
  // gHue++;
  // delay(100);

}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void rainbow()
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
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
