#ifndef JSON_PACKET_LIBRARY
#define JSON_PACKET_LIBRARY

#include <ArduinoJson.h>
#include <Array.h>
#include <ESPAsyncWebServer.h>

#include "programs.h"

void assignDurations();
void sendMessage(String);
void writeProgramsToMemory(const char *);



const long BAUD = 115200;

const size_t MAX_PROGRAMS = 256;
const size_t MAX_PROGRAM_SIZE = 6144;   // chars

Array<Program *, MAX_PROGRAMS> programs;


DynamicJsonDocument jsonPacket(MAX_PROGRAM_SIZE);
// StaticJsonDocument<MAX_PROGRAM_SIZE> jsonPacket;

// const char* input = R"json([{"id":1689728727144,"displayName":"Vertical Wave","color":{"name":"Color","linkColors":true,"color":[{"h":12,"s":255,"l":255},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":120,"s":255,"l":255}]},"bodyColor":{"name":"Body Color","on":false,"speed":0,"duration":0,"linkColors":true,"color":[{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0}]},"ringColor":{"name":"Ring Color","on":false,"speed":0,"duration":0,"linkColors":true,"color":[{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0}]},"sparkleColor":{"name":"Sparkle Color","on":false,"speed":0,"duration":0,"linkColors":true,"color":[{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0}]},"flashColor":{"name":"Flash Color","on":false,"speed":0,"duration":0,"linkColors":true,"color":[{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0}]},"duration":10,"patternSpeed":1,"colorCycleSpeed":1,"secondaryColor":{"name":"Secondary Color","on":false,"speed":0,"duration":10,"linkColors":true,"color":[{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0}]}},{"id":1689903737722,"displayName":"Pulsing Color","linkColors":true,"color":{"name":"Color","linkColors":true,"color":[{"h":128,"s":55,"l":100},{"h":128,"s":55,"l":100},{"h":128,"s":55,"l":100},{"h":128,"s":55,"l":100}]},"bodyColor":{"name":"Body Color","on":false,"speed":0,"duration":0,"linkColors":true,"color":[{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0}]},"ringColor":{"name":"Ring Color","on":false,"speed":0,"duration":0,"linkColors":true,"color":[{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0}]},"sparkleColor":{"name":"Sparkle Color","on":false,"speed":0,"duration":0,"linkColors":true,"color":[{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0}]},"flashColor":{"name":"Flash Color","on":false,"speed":0,"duration":0,"linkColors":true,"color":[{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0}]},"duration":15,"patternSpeed":1,"colorCycleSpeed":2},{"id":1689903741273,"displayName":"BPM","sparkleColor":{"name":"Sparkle Color","on":false,"speed":0,"duration":0,"linkColors":true,"color":[{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0}]},"flashColor":{"name":"Flash Color","on":false,"speed":0,"duration":0,"linkColors":true,"color":[{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0}]},"duration":20,"patternSpeed":1,"colorCycleSpeed":1}])json";
// const uint8_t* blinkTestInput = (const uint8_t*)R"json([{"displayName":"Blink Test","colorCycleSpeed":1,"duration":5,"patternSpeed": 1])json";
// size_t blinkTestLen = 80;

// const char* readJsonDocument(AsyncWebServerRequest *request) {
const char* readJsonDocument(const char *jsonString) {

  // Return NULL if OK, or error message if there's a problem
  // Serial.println("Inside readJsonDocument. Memory is " + String(ESP.getFreeHeap()));

  // Serial.println(request->);

  // DeserializationError error = deserializeJson(jsonPacket, blinkTestInput);
  DeserializationError error = deserializeJson(jsonPacket, jsonString);
  // DeserializationError error = deserializeJson(jsonPacket, request);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return error.c_str();
  }

  if (jsonPacket.size() > MAX_PROGRAMS) {
    Serial.println(F("WARNING: Number of programs sent exceeds available slots (") + String(jsonPacket.size()) + F(").  Will only load ") + String(MAX_PROGRAMS));
  }

  for(size_t i = 0; i < min(jsonPacket.size(), MAX_PROGRAMS); i++) {
    String patternName = jsonPacket[i]["displayName"];

    if (patternName == "Vertical Wave"){
      Serial.println("Creating VerticalWave");
      VerticalWave *p = new VerticalWave(jsonPacket[i]);
      programs.push_back(p);
      Serial.println(p->baseColor.r);
    }

    else if (patternName == "Pulsing Color"){
      programs.push_back(new PulsingColor(jsonPacket[i])); // TODO: Have this return a bool to tell if decoding happened correctly
    }

    else if (patternName == "BPM"){
      programs.push_back(new Bpm(jsonPacket[i]));
    }

    else if (patternName == "Blink Test"){
      Serial.println("Deserialized a Blink Test! Current number of programs is " + String(programs.size()));
      programs.push_back(new BlinkTest(jsonPacket[i]));
      Serial.println("New number of programs is " + String(programs.size()));
    }

    else Serial.println(F("Unknown program name: ") + patternName);
  }

  // once the program is verified, stick it into flash memory
  #ifdef WRITE_FLASH
  writeProgramsToMemory(jsonString);
  #endif

  // Serial.println("After  readJsonDocument. Memory is " + String(ESP.getFreeHeap()));
  #ifdef INCLUDE_LEDS
    assignDurations();
  #endif
  return NULL;
}

void newProgramsArriving() {
  // Delete in reverse order to try to reduce memory fragementation
  while(programs.size() > 0) {
    delete programs.back();    // Retrieve last item in array
    programs.pop_back();                // Remove the now dead item from the array
  }

}

void broadcastJson() {
  // Serial.println("Attempting to serialize and broadcast json! Memory is " + String(ESP.getFreeHeap()));
  String serialized = "";
  serializeJson(jsonPacket, serialized);
  sendMessage(serialized);
  // Serial.println("Done broadcasting json. Memory is " + String(ESP.getFreeHeap()));
}


#endif