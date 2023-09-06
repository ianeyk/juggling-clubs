#ifndef LED_CONTROL_LIBRARY
#define LED_CONTROL_LIBRARY

#include "jsonPacket.h"

uint8_t patternId = 0; // Index number of which pattern is current
unsigned long patternTime = 0; // in milliseconds
unsigned long hueTime = 0; // in milliseconds
unsigned long patternFrame = 0; // in frames, 20fps
unsigned long hueFrame = 0; // in frames, 20fps // rotating "base color" used by many of the patterns
const unsigned int millisecondsPerFrame = 50; // for use later in patterns themselves // defines the time step for animations
std::vector<unsigned long> cumDurations;
std::vector<unsigned long> cumHueDurations;

#include "patterns.h"

void assignDurations();
void updateCounters();

void fastLedSetup() {
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip); // tell FastLED about the LED strip configuration
  FastLED.setBrightness(BRIGHTNESS);   // set master brightness control
  readJsonDocument("");
}

typedef void (*SimplePatternList[])(); // List of patterns to cycle through.  Each is defined as a separate function below.
// SimplePatternList gPatterns = { solid, rainbowWithGlitter, confetti, sinelon, juggle, bpm, rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm, rainbowWithGlitter, confetti, juggle};
SimplePatternList gPatterns = { pulse, solid, solid, rainbowWithGlitter, confetti, juggle, bpm, solid, solid, solid, solid, solid, solid, solid, solid };
SimplePatternList gAddins = { ring_solid, sparkle, flash};

void updateLeds() {
    updateCounters(); // get the current pattern id and pattern frame
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

void updateCounters() { // replacement for incrementCounters() above
    unsigned long nodeTime = mesh.getNodeTime(); // microseconds to milliseconds
    // Serial.print("Node Time was " + String(nodeTime));
    nodeTime = nodeTime / 1000;
    // Serial.println(". After / 1000, Node Time (2) was " + String(nodeTime));

    unsigned long periodTime = nodeTime % cumDurations.back();

    for (unsigned int i = 0; i < cumDurations.size(); i++) {
        if (periodTime <= cumDurations[i]) {
            patternId = i;
            break; // out of the for loop
        }
    }
    unsigned long colorCycleSpeed = jsonPacket[patternId]["colorCycleSpeed"];
    if (patternId == 0) {
        patternTime = periodTime;
        hueTime = periodTime * colorCycleSpeed;
    } else {
        patternTime = periodTime - cumDurations[patternId - 1];
        hueTime = periodTime * colorCycleSpeed - cumHueDurations[patternId - 1];
    }
    patternFrame = patternTime / millisecondsPerFrame;
    hueFrame = (hueTime / millisecondsPerFrame) % 256;
    Serial.println("pattern Frame is " + String(patternFrame) + ", hue Frame is " + String(hueFrame));
}

void assignDurations() {
  // assign cumulative duration values for calculating pattern time from global clock
  unsigned long duration = 0;
  unsigned long hueDuration = 0;
  cumDurations.clear();
  cumHueDurations.clear();

  for (unsigned int i = 0; i < jsonPacket.size(); i++) {
    unsigned long nextDurationValue = jsonPacket[i]["duration"]; // * 1000;
    unsigned long colorCycleSpeed = jsonPacket[i]["colorCycleSpeed"];
    duration += nextDurationValue * 1000; // seconds to milliseconds
    hueDuration += nextDurationValue * 1000 * colorCycleSpeed;
    cumDurations.push_back(duration);
    cumHueDurations.push_back(hueDuration);
    Serial.println("Cumulative duration was " + String(duration) + " milliseconds.");
  }
}

#endif