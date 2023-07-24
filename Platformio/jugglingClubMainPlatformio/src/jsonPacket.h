#ifndef JSON_PACKET_LIBRARY
#define JSON_PACKET_LIBRARY

#include <ArduinoJson.h>

DynamicJsonDocument jsonPacket(6144);

void readJsonDocument(String jsonString) {

  String input = R"json([{"id":1689728727144,"displayName":"Vertical Wave","color":{"name":"Color","linkColors":true,"color":[{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0}]},"bodyColor":{"name":"Body Color","on":false,"speed":0,"duration":0,"linkColors":true,"color":[{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0}]},"ringColor":{"name":"Ring Color","on":false,"speed":0,"duration":0,"linkColors":true,"color":[{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0}]},"sparkleColor":{"name":"Sparkle Color","on":false,"speed":0,"duration":0,"linkColors":true,"color":[{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0}]},"flashColor":{"name":"Flash Color","on":false,"speed":0,"duration":0,"linkColors":true,"color":[{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0}]},"patternSpeed":0,"colorCycleSpeed":0,"secondaryColor":{"name":"Secondary Color","on":false,"speed":0,"duration":0,"linkColors":true,"color":[{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0}]}},{"id":1689903737722,"displayName":"Pulsing Color","linkColors":true,"color":{"name":"Color","linkColors":true,"color":[{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0}]},"bodyColor":{"name":"Body Color","on":false,"speed":0,"duration":0,"linkColors":true,"color":[{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0}]},"ringColor":{"name":"Ring Color","on":false,"speed":0,"duration":0,"linkColors":true,"color":[{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0}]},"sparkleColor":{"name":"Sparkle Color","on":false,"speed":0,"duration":0,"linkColors":true,"color":[{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0}]},"flashColor":{"name":"Flash Color","on":false,"speed":0,"duration":0,"linkColors":true,"color":[{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0}]},"patternSpeed":0,"colorCycleSpeed":0},{"id":1689903741273,"displayName":"BPM","sparkleColor":{"name":"Sparkle Color","on":false,"speed":0,"duration":0,"linkColors":true,"color":[{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0}]},"flashColor":{"name":"Flash Color","on":false,"speed":0,"duration":0,"linkColors":true,"color":[{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0},{"h":0,"s":0,"l":0}]},"patternSpeed":0}])json";

  DeserializationError error = deserializeJson(jsonPacket, jsonString);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }
}

#endif