#include <FastLED.h>
#include <ArduinoJson.h>

#define NUM_CLUBS 3

////////// Defines for pattern properties //////////
// from: https://github.com/ianeyk/juggling-clubs-website/blob/main/src/data/patterns.ts
#define ALIAS_PATTERNNAME     "0"   //  #define ALIAS_PATTERNNAME     "patternName"      //
#define ALIAS_LINKCOLORS      "1"   //  #define ALIAS_LINKCOLORS      "linkColors"       //
#define ALIAS_COLOR           "2"   //  #define ALIAS_COLOR           "color"            //
#define ALIAS_SECONDARYCOLOR  "3"   //  #define ALIAS_SECONDARYCOLOR  "secondaryColor"   //
#define ALIAS_SPARKLECOLOR    "4"   //  #define ALIAS_SPARKLECOLOR    "sparkleColor"     //
#define ALIAS_FLASHCOLOR      "5"   //  #define ALIAS_FLASHCOLOR      "flashColor"       //
#define ALIAS_PATTERNSPEED    "6"   //  #define ALIAS_PATTERNSPEED    "patternSpeed"     //
#define ALIAS_PATTERNDURATION "7"   //  #define ALIAS_PATTERNDURATION "patternDuration"  //
#define ALIAS_COLORCYCLESPEED "8"   //  #define ALIAS_COLORCYCLESPEED "colorCycleSpeed"  //
#define ALIAS_SYNCHRONIZED    "9"   //  #define ALIAS_SYNCHRONIZED    "synchronized"     //
#define ALIAS_ON              "10"  //  #define ALIAS_ON              "on"               //
#define ALIAS_SPEED           "11"  //  #define ALIAS_SPEED           "speed"            //
#define ALIAS_INTENSITY       "12"  //  #define ALIAS_INTENSITY       "intensity"        //
#define ALIAS_DUTYCYCLE       "13"  //  #define ALIAS_DUTYCYCLE       "dutyCycle"        //
////////////////////////////////////////////////////

int get_club_id();


class Program {

    public:
        float duration;
        float patternSpeed;
        float colorCycleSpeed;

        Program() { /* Do nothing */ }
        virtual ~Program() { /* Do nothing */ }
        Program(const DynamicJsonDocument &json) {
            this->duration = json[ALIAS_PATTERNDURATION];
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
    prog.duration = json[ALIAS_PATTERNDURATION];
    prog.patternSpeed = json[ALIAS_PATTERNSPEED];
    prog.colorCycleSpeed = json[ALIAS_COLORCYCLESPEED];
}


void makeCrgb(JsonVariantConst json, CRGB &color) {
    // Expect json to look like this:
    // {ALIAS_LINKCOLORS: true, ALIAS_COLOR: "ffffffffffffffffffffffff"}

    int color_index;

    if(json[ALIAS_LINKCOLORS]) {
        color_index = 0;
    }
    else {
        color_index = get_club_id() + 1;
    }

    std::string colorString = json[ALIAS_COLOR].as<std::string>();
    color = CRGB(strtol(colorString.substr(color_index * 6, 6).c_str(), NULL, 16));

    Serial.println("color_index: " + String(color_index) + " r: " + String(color.r));
};


class PulsingColor: public Program {
    public:
        CRGB baseColor, sparkleColor, flashColor;

        PulsingColor() { /* Do nothing */ }
        void onTick(int patternFrame) {}
};

void convertFromJson(JsonVariantConst json, PulsingColor &prog) {
    convertFromJson(json, static_cast<Program &>(prog));

    makeCrgb(json[ALIAS_COLOR][ALIAS_COLOR], prog.baseColor);
    makeCrgb(json[ALIAS_SPARKLECOLOR][ALIAS_COLOR], prog.sparkleColor);
    makeCrgb(json[ALIAS_FLASHCOLOR][ALIAS_COLOR], prog.flashColor);

    Serial.println("Constructed PulsingColor...");
}

class VerticalWave: public Program {
    public:
        CRGB baseColor, secondaryColor, sparkleColor, flashColor;

        VerticalWave() {Serial.println("Default VerticalWave constructor");}
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

    makeCrgb(json[ALIAS_SPARKLECOLOR], prog.sparkleColor);
    makeCrgb(json[ALIAS_FLASHCOLOR], prog.flashColor);
    Serial.printf("Constructed BPM");
}


/*
    [
        { "class": "SolidColor", "data": {"color": [{"h":12,"s":255,"l":255}, {"h":12,"s":255,"l":255}, {"h":12,"s":255,"l":255}, {"h":12,"s":255,"l":255}] }},
        { "class": "SolidColor", "data": {"color": [{"h":12,"s":255,"l":255}, {"h":12,"s":255,"l":255}, {"h":12,"s":255,"l":255}, {"h":12,"s":255,"l":255}] }},
    ]
*/
