#ifndef JSON_PACKET_LIBRARY
#define JSON_PACKET_LIBRARY

#include <ArduinoJson.h>

DynamicJsonDocument jsonPacket(6144);

const String input = R"json([{"id":1689728727144,"displayName":"Vertical Wave","color":{"name":"Color","linkColors":true,"color":[{"h":12,"s":255,"l":255},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":120,"s":255,"l":255}]},"bodyColor":{"name":"Body Color","on":false,"speed":0,"duration":0,"linkColors":true,"color":[{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0}]},"ringColor":{"name":"Ring Color","on":false,"speed":0,"duration":0,"linkColors":true,"color":[{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0}]},"sparkleColor":{"name":"Sparkle Color","on":false,"speed":0,"duration":0,"linkColors":true,"color":[{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0}]},"flashColor":{"name":"Flash Color","on":false,"speed":0,"duration":0,"linkColors":true,"color":[{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0}]},"duration":10,"patternSpeed":1,"colorCycleSpeed":0,"secondaryColor":{"name":"Secondary Color","on":false,"speed":0,"duration":0,"linkColors":true,"color":[{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0}]}},{"id":1689903737722,"displayName":"Pulsing Color","linkColors":true,"color":{"name":"Color","linkColors":true,"color":[{"h":128,"s":55,"l":100},{"h":128,"s":55,"l":100},{"h":128,"s":55,"l":100},{"h":128,"s":55,"l":100}]},"bodyColor":{"name":"Body Color","on":false,"speed":0,"duration":0,"linkColors":true,"color":[{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0}]},"ringColor":{"name":"Ring Color","on":false,"speed":0,"duration":0,"linkColors":true,"color":[{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0}]},"sparkleColor":{"name":"Sparkle Color","on":false,"speed":0,"duration":0,"linkColors":true,"color":[{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0}]},"flashColor":{"name":"Flash Color","on":false,"speed":0,"duration":0,"linkColors":true,"color":[{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0}]},"duration":10,"patternSpeed":1,"colorCycleSpeed":0},{"id":1689903741273,"displayName":"BPM","sparkleColor":{"name":"Sparkle Color","on":false,"speed":0,"duration":0,"linkColors":true,"color":[{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0}]},"flashColor":{"name":"Flash Color","on":false,"speed":0,"duration":0,"linkColors":true,"color":[{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0}]},"duration":30,"patternSpeed":1}])json";
void readJsonDocument(String jsonString) {

  DeserializationError error = deserializeJson(jsonPacket, input);
  // DeserializationError error = deserializeJson(jsonPacket, jsonString);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }
}

#endif