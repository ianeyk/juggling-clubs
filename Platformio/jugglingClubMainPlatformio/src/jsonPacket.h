#ifndef JSON_PACKET_LIBRARY
#define JSON_PACKET_LIBRARY

#include <ArduinoJson.h>

void assignDurations();
void sendMessage(String);

DynamicJsonDocument jsonPacket(6144);

const char* input = R"json([{"id":1689728727144,"displayName":"Vertical Wave","color":{"name":"Color","linkColors":true,"color":[{"h":12,"s":255,"l":255},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":120,"s":255,"l":255}]},"bodyColor":{"name":"Body Color","on":false,"speed":0,"duration":0,"linkColors":true,"color":[{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0}]},"ringColor":{"name":"Ring Color","on":false,"speed":0,"duration":0,"linkColors":true,"color":[{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0}]},"sparkleColor":{"name":"Sparkle Color","on":false,"speed":0,"duration":0,"linkColors":true,"color":[{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0}]},"flashColor":{"name":"Flash Color","on":false,"speed":0,"duration":0,"linkColors":true,"color":[{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0}]},"duration":10,"patternSpeed":1,"colorCycleSpeed":1,"secondaryColor":{"name":"Secondary Color","on":false,"speed":0,"duration":10,"linkColors":true,"color":[{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0}]}},{"id":1689903737722,"displayName":"Pulsing Color","linkColors":true,"color":{"name":"Color","linkColors":true,"color":[{"h":128,"s":55,"l":100},{"h":128,"s":55,"l":100},{"h":128,"s":55,"l":100},{"h":128,"s":55,"l":100}]},"bodyColor":{"name":"Body Color","on":false,"speed":0,"duration":0,"linkColors":true,"color":[{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0}]},"ringColor":{"name":"Ring Color","on":false,"speed":0,"duration":0,"linkColors":true,"color":[{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0}]},"sparkleColor":{"name":"Sparkle Color","on":false,"speed":0,"duration":0,"linkColors":true,"color":[{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0}]},"flashColor":{"name":"Flash Color","on":false,"speed":0,"duration":0,"linkColors":true,"color":[{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0}]},"duration":15,"patternSpeed":1,"colorCycleSpeed":2},{"id":1689903741273,"displayName":"BPM","sparkleColor":{"name":"Sparkle Color","on":false,"speed":0,"duration":0,"linkColors":true,"color":[{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0}]},"flashColor":{"name":"Flash Color","on":false,"speed":0,"duration":0,"linkColors":true,"color":[{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0}]},"duration":20,"patternSpeed":1,"colorCycleSpeed":1}])json";

void readJsonDocument(String jsonString) {
  Serial.println("Inside readJsonDocument. Memory is " + String(ESP.getFreeHeap()));

  DeserializationError error = deserializeJson(jsonPacket, input);
  // DeserializationError error = deserializeJson(jsonPacket, jsonString);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }
  Serial.println("After  readJsonDocument. Memory is " + String(ESP.getFreeHeap()));
  #ifdef INCLUDE_LEDS
    assignDurations();
  #endif
}

void broadcastJson() {
  Serial.println("Attempting to serialize and broadcast json! Memory is " + String(ESP.getFreeHeap()));
  String serialized = "";
  serializeJson(jsonPacket, serialized);
  sendMessage(serialized);
  Serial.println("Done broadcasting json. Memory is " + String(ESP.getFreeHeap()));
}


#endif