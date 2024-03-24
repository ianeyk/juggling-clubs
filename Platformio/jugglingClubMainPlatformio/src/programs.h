#include <ArduinoJson.h>
#include <FastLED.h>

#include "ledDef.h"
#include "programPropertyAliases.h"

#define NUM_CLUBS 3

int get_club_id();

class Program {
   public:
    float duration;
    float patternSpeed;
    float colorCycleSpeed;

    Program() { /* Do nothing */
    }
    virtual ~Program() { /* Do nothing */
    }
    Program(const DynamicJsonDocument &json) {
        Serial.println("Inside of parent Program constructor function");
        // this->duration = json[ALIAS_PATTERNDURATION];
        this->duration = json[ALIAS_COLORCYCLESPEED];
        this->patternSpeed = json[ALIAS_PATTERNSPEED];
        this->colorCycleSpeed = json[ALIAS_COLORCYCLESPEED];
    }

    virtual void onTick(int) = 0;

    // void onTick(int patternFrame) {
    //     // Run each step of the program
    //     Serial.println("The parent Program class onTick() function is being called.");
    // }
};

void convertFromJson(JsonVariantConst json, Program &prog) {
    prog.duration = json[ALIAS_COLORCYCLESPEED];
    prog.patternSpeed = json[ALIAS_PATTERNSPEED];
    prog.colorCycleSpeed = json[ALIAS_COLORCYCLESPEED];
}

void makeCrgb(JsonVariantConst json, CRGB &color) {
    // Expect json to look like this:
    // {ALIAS_LINKCOLORS: true, ALIAS_COLOR: ["#F44336", "#000000", "#000000", "#000000"]}

    int color_index;

    if (json[ALIAS_LINKCOLORS]) {
        color_index = 0;
    } else {
        color_index = get_club_id() + 1;
    }

    String colorString = json[ALIAS_COLOR][color_index];
    Serial.println("colorString = " + colorString);
    if (colorString.startsWith("#")) {
        colorString.remove(0, 1);
    }
    color = CRGB(strtol(colorString.c_str(), NULL, 16));

    Serial.println("color_index: " + String(color_index) + " r: " + String(color.r));
};

class PulsingColor : public Program {
   public:
    CRGB baseColor, sparkleColor, flashColor;

    PulsingColor() { /* Do nothing */
    }
    PulsingColor(const DynamicJsonDocument &json) {
        Serial.println("Inside of PulsingColor constructor");
    }
    void onTick(int patternFrame) {
        // Serial.println("Filling solid with color r: " + String(baseColor.r) + ", g: " + String(baseColor.g) + ", b: " + String(baseColor.b));
        fill_solid(leds, NUM_LEDS, baseColor);
    }
};

void convertFromJson(JsonVariantConst json, PulsingColor &prog) {
    // Serial.println("Inside of convertFromJson");
    convertFromJson(json, static_cast<Program &>(prog));
    // Serial.println("After default convertFromJson!!");

    makeCrgb(json[ALIAS_COLOR], prog.baseColor);
    makeCrgb(json[ALIAS_SPARKLECOLOR], prog.sparkleColor);
    makeCrgb(json[ALIAS_FLASHCOLOR], prog.flashColor);

    // Serial.println("Constructed PulsingColor...");
    // Serial.println("Finished with convertFromJson");
}

class VerticalWave : public Program {
   public:
    CRGB baseColor, secondaryColor, sparkleColor, flashColor;

    VerticalWave() { Serial.println("Default VerticalWave constructor"); }
    VerticalWave(const DynamicJsonDocument &json) : Program(json) {
        Serial.println("Parameterized VerticalWave constructor");
        makeCrgb(json[ALIAS_COLOR][ALIAS_COLOR], this->baseColor);
        Serial.println(this->baseColor.r);
        Serial.println(this->baseColor.g);
        Serial.println(this->baseColor.b);
        makeCrgb(json[ALIAS_SECONDARYCOLOR][ALIAS_COLOR], this->secondaryColor);
        makeCrgb(json[ALIAS_SPARKLECOLOR][ALIAS_COLOR], this->sparkleColor);
        makeCrgb(json[ALIAS_FLASHCOLOR][ALIAS_COLOR], this->flashColor);

        Serial.println("Constructed VerticalWave...");
    }
    void onTick(int patternFrame) {}
};

void convertFromJson(JsonVariantConst json, VerticalWave &prog) {
    // Serial.println("Here TTT: " + String(prog));

    convertFromJson(json, static_cast<Program &>(prog));

    makeCrgb(json[ALIAS_COLOR][ALIAS_COLOR], prog.baseColor);
    makeCrgb(json[ALIAS_SECONDARYCOLOR][ALIAS_COLOR], prog.secondaryColor);
    makeCrgb(json[ALIAS_SPARKLECOLOR][ALIAS_COLOR], prog.sparkleColor);
    makeCrgb(json[ALIAS_FLASHCOLOR][ALIAS_COLOR], prog.flashColor);

    // Serial.println("Constructed VerticalWave...");
}

class Bpm : public Program {
   public:
    CRGB sparkleColor, flashColor;

    Bpm() { /* Do nothing */
    }
    void onTick(int patternFrame) {}
};

class BlinkTest : public Program {
   public:
    BlinkTest() { Serial.println("Blink test default constructor."); /* empty constructor */ }
    void onTick(int patternFrame) {
        // patternFrame; // each frame is 50ms long; 20 frames per second
        // duration; // duration in seconds; always pass 5 seconds
        // patternSpeed; // scalar that we are trying to watch
        // 200ms (4 frames) on; 200ms (4 frames) off, up to patternSpeed times. If patternSpeed < 5, this will take less than 2s.
        // Serial.println("Inside of BlinkTest. patternFrame = " + String(patternFrame));

        int blinkOnTime = 4;
        int blinkOnOffTime = 2 * blinkOnTime;
        bool flag = false;
        for (int i = 0; i < this->colorCycleSpeed; i++) {
            if (patternFrame >= blinkOnOffTime * i && patternFrame < blinkOnOffTime * i + blinkOnTime) {
                flag = true;
            }
        }
        if (flag) {
            // digitalWrite(LED_BUILTIN, LOW);
        } else {
            // digitalWrite(LED_BUILTIN, HIGH);
        }
    }
};

void convertFromJson(JsonVariantConst json, BlinkTest &prog) {
    convertFromJson(json, static_cast<Program &>(prog));
}

void convertFromJson(JsonVariantConst json, Bpm &prog) {
    convertFromJson(json, static_cast<Program &>(prog));

    makeCrgb(json[ALIAS_SPARKLECOLOR], prog.sparkleColor);
    makeCrgb(json[ALIAS_FLASHCOLOR], prog.flashColor);
    Serial.printf("Constructed BPM");
}
