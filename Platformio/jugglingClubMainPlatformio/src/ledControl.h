#define COUNTER_WRAP_LIMIT = 65535 // 256 ^ 2
uint8_t patternId = 0; // Index number of which pattern is current
uint8_t hueCounter = 0; // rotating "base color" used by many of the patterns
uint8_t patternTimeCounter = 0; // defines the time step for animations
#include "patterns.h"


void fastLedSetup() {
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip); // tell FastLED about the LED strip configuration
  FastLED.setBrightness(BRIGHTNESS);   // set master brightness control
}

typedef void (*SimplePatternList[])(); // List of patterns to cycle through.  Each is defined as a separate function below.
// SimplePatternList gPatterns = { solid, rainbowWithGlitter, confetti, sinelon, juggle, bpm, rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm, rainbowWithGlitter, confetti, juggle};
SimplePatternList gPatterns = { pulse, solid, solid, rainbowWithGlitter, confetti, juggle, bpm, solid, solid, solid, solid, solid, solid, solid, solid };
SimplePatternList gAddins = { ring_solid, sparkle, flash};

void resetCounters() {
    patternTimeCounter = 0;
    hueCounter = 0;
    patternId = 0;
}

void incrementCounters() {
    unsigned short patternSpeed = jsonPacket[patternId]["patternSpeed"];
    patternTimeCounter += patternSpeed;
    unsigned short colorCycleSpeed = jsonPacket[patternId]["colorCycleSpeed"];
    hueCounter += colorCycleSpeed;
    unsigned short duration = jsonPacket[patternId]["duration"];
    if (patternTimeCounter > duration) {
        patternTimeCounter = 0;
        patternId = (patternId + 1) % jsonPacket[patternId].size();
    }
}

void updateLeds() {
    incrementCounters();
    // Call the current pattern function once, updating the 'leds' array
    String patternName = jsonPacket[patternId]["displayName"];
    if (patternName == "Vertical Wave") verticalWave();
    else if (patternName == "Pulsing Color") pulse();
    else if (patternName == "BPM") bpm();
    else solid();

    // then apply all of the addins. If they are not enabled, the function will handle that
    ring_solid();
    sparkle();
    flash();

    FastLED.show(); // send the 'leds' array out to the actual LED strip
}