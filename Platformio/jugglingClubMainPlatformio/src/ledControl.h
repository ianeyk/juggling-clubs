#ifndef LED_CONTROL_LIBRARY
#define LED_CONTROL_LIBRARY

#define COUNTER_WRAP_LIMIT = 65535 // 256 ^ 2
uint8_t patternId = 0; // Index number of which pattern is current
uint8_t hueCounter = 0; // rotating "base color" used by many of the patterns
uint8_t patternTimeCounter = 0; // defines the time step for animations
#include "patterns.h"


void fastLedSetup() {
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip); // tell FastLED about the LED strip configuration
  FastLED.setBrightness(BRIGHTNESS);   // set master brightness control
  readJsonDocument("");
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
        patternId = (patternId + 1) % jsonPacket.size();
    }
    Serial.println("Json packet size is " + String(jsonPacket.size()));
    Serial.println("Incrementing counters! New counter is " + String(patternId));
    Serial.println("Pattern Speed is " + String(patternSpeed) + "and duration is " + String(duration) + "and counter is " + String(patternTimeCounter));
}

void updateLeds() {
    // incrementCounters();
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

    // Serial.println("Pattern Name: " + patternName);

    FastLED.show(); // send the 'leds' array out to the actual LED strip
}

std::vector<unsigned long> cumDurations;
std::vector<unsigned long> cumHueDurations;

void assignDurations() {
  // assign cumulative duration values for calculating pattern time from global clock
  unsigned long duration = 0;
  unsigned long hueDuration = 0;
  cumDurations.clear();
  cumHueDurations.clear();

  for (int i = 0; i < jsonPacket.size(); i++) {
    duration += jsonPacket[i]["duration"] * 1000; // seconds to milliseconds
    hueDuration += jsonPacket[i]["duration"] * 1000 * jsonPacket[i]["colorCycleSpeed"];
    cumDurations.push_back(duration);
    cumHueDurations.push_back(hueDuration);
  }
}

unsigned long patternTime = 0; // in milliseconds
unsigned long hueTime = 0; // in milliseconds
unsigned long patternFrame = 0; // in frames, 20fps
unsigned long hueFrame = 0; // in frames, 20fps
const unsigned int millisecondsPerFrame = 50; // for use later in patterns themselves

void updateCounters() { // replacement for incrementCounters() above
    unsigned long nodeTime = mesh.getNodeTime() / 1000; // microseconds to milliseconds

    unsigned long periodTime = nodeTime % cumDurations.back();
    for (int i = 0; i < cumDurations.size(); i++) {
        if (periodTime <= cumDurations[i]) {
            patternId = i;
            break; // out of the for loop
        }
    }
    if (patternId == 0) {
        patternTime = periodTime;
        hueTime = periodTime * jsonPacket[patternId]["colorCycleSpeed"];
    } else {
        patternTime = periodTime - cumDurations[patternId - 1];
        hueTime = periodTime * jsonPacket[patternId]["colorCycleSpeed"] - cumHueDurations[patternId - 1];
    }
    patternFrame = patternTime / millisecondsPerFrame;
    hueFrame = (hueTime / millisecondsPerFrame) % 256;
}

#endif