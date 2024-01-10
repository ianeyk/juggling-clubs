#include <FastLED.h>
#include <ArduinoJson.h>

#define NUM_CLUBS 3

int get_club_id();


class Program {

    public:
        float duration;
        float patternSpeed;
        float colorCycleSpeed;

        Program() { /* Do nothing */ }
        Program(const DynamicJsonDocument &json) {
            this->duration = json["duration"];
            this->patternSpeed = json["patternSpeed"];
            this->colorCycleSpeed = json["colorCycleSpeed"];
        }

        virtual void onTick(int) = 0;

        // void onTick(int patternFrame) {
        //     // Run each step of the program
        //     Serial.println("The parent Program class onTick() function is being called.");
        // }
};

void convertFromJson(JsonVariantConst json, Program &prog) {
    prog.duration = json["duration"];
    prog.patternSpeed = json["patternSpeed"];
    prog.colorCycleSpeed = json["colorCycleSpeed"];
}


void makeCrgb(const JsonVariantConst &json, CRGB &color) {
    // Expect json to look like this:
    // [{ "h": 12, "s": 255, "l": 255 }, { "h": 44, "s": 66, "l": 80 }, { "h": 45, "s": 66, "l": 80 }, { "h": 12255, "s": 255, "l": 255 }]

    int color_index;

    if(json["linkColors"]) {
        color_index = 0;
    }
    else {
        color_index = get_club_id() + 1;
    }
    CHSV hsv = CHSV(int(json[color_index]["h"]), int(json[color_index]["s"]), int(json[color_index]["l"]));
    hsv2rgb_rainbow(hsv, color);
    // Serial.println("color_index: " + String(color_index) + " h: " + String(json) + " r: " + String(color.r));
};


class PulsingColor: public Program {
    public:
        CRGB baseColor, sparkleColor, flashColor;

        PulsingColor() { /* Do nothing */ }
        void onTick(int patternFrame) {}
};

void convertFromJson(JsonVariantConst json, PulsingColor &prog) {
    convertFromJson(json, static_cast<Program &>(prog));

    makeCrgb(json["color"]["color"], prog.baseColor);
    makeCrgb(json["sparkleColor"]["color"], prog.sparkleColor);
    makeCrgb(json["flashColor"]["color"], prog.flashColor);

    Serial.println("Constructed PulsingColor...");
}

class VerticalWave: public Program {
    public:
        CRGB baseColor, secondaryColor, sparkleColor, flashColor;

        VerticalWave() {Serial.println("Default VerticalWave constructor");}
        VerticalWave(const DynamicJsonDocument &json) : Program(json) {
            Serial.println("Parameterized VerticalWave constructor");
            makeCrgb(json["color"]["color"], this->baseColor);
            Serial.println(this->baseColor.r);
            Serial.println(this->baseColor.g);
            Serial.println(this->baseColor.b);
            makeCrgb(json["secondaryColor"]["color"], this->secondaryColor);
            makeCrgb(json["sparkleColor"]["color"], this->sparkleColor);
            makeCrgb(json["flashColor"]["color"], this->flashColor);

            Serial.println("Constructed VerticalWave...");

        }
        void onTick(int patternFrame) {}
};


void convertFromJson(JsonVariantConst json, VerticalWave &prog) {
    // Serial.println("Here TTT: " + String(prog));

    convertFromJson(json, static_cast<Program &>(prog));

    makeCrgb(json["color"]["color"], prog.baseColor);
    makeCrgb(json["secondaryColor"]["color"], prog.secondaryColor);
    makeCrgb(json["sparkleColor"]["color"], prog.sparkleColor);
    makeCrgb(json["flashColor"]["color"], prog.flashColor);

    // Serial.println("Constructed VerticalWave...");
}



class Bpm: public Program {
    public:
        CRGB sparkleColor, flashColor;

        Bpm() { /* Do nothing */ }
        void onTick(int patternFrame) {}
};


class BlinkTest: public Program {
    public:
        BlinkTest() { Serial.println("Blink test default constructor."); /* empty constructor */}
        void onTick(int patternFrame) {
            // patternFrame; // each frame is 50ms long; 20 frames per second
            // duration; // duration in seconds; always pass 5 seconds
            // patternSpeed; // scalar that we are trying to watch
            // 200ms (4 frames) on; 200ms (4 frames) off, up to patternSpeed times. If patternSpeed < 5, this will take less than 2s.
            Serial.println("Inside of BlinkTest. patternFrame = " + String(patternFrame));

            int blinkOnTime = 4;
            int blinkOnOffTime = 2 * blinkOnTime;
            bool flag = false;
            for (int i = 0; i < this->colorCycleSpeed; i++) {
                if (patternFrame >= blinkOnOffTime * i && patternFrame < blinkOnOffTime * i + blinkOnTime) {
                    flag = true;
                }
            }
            if (flag) {
                digitalWrite(LED_BUILTIN, LOW);
            }
            else {
                digitalWrite(LED_BUILTIN, HIGH);
            }
        }
};



void convertFromJson(JsonVariantConst json, BlinkTest &prog) {
    convertFromJson(json, static_cast<Program &>(prog));

}

void convertFromJson(JsonVariantConst json, Bpm &prog) {
    convertFromJson(json, static_cast<Program &>(prog));

    makeCrgb(json["sparkleColor"], prog.sparkleColor);
    makeCrgb(json["flashColor"], prog.flashColor);
    Serial.printf("Constructed BPM");
}


/*
    [
        { "class": "SolidColor", "data": {"color": [{"h":12,"s":255,"l":255}, {"h":12,"s":255,"l":255}, {"h":12,"s":255,"l":255}, {"h":12,"s":255,"l":255}] }},
        { "class": "SolidColor", "data": {"color": [{"h":12,"s":255,"l":255}, {"h":12,"s":255,"l":255}, {"h":12,"s":255,"l":255}, {"h":12,"s":255,"l":255}] }},
    ]
*/
