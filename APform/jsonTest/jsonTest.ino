#include <ArduinoJson.h>
#include <FastLED.h>

#define N_PATTERNS 15
#define N_COLORS 6
#define N_SPEEDS 3

struct Packet {
  const char* colors  [N_COLORS];
  int speeds[N_SPEEDS];
  bool patterns[N_PATTERNS];
  int addon;
};

// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2022, Benoit BLANCHON
// MIT License
//
// This example shows how to generate a JSON document with ArduinoJson.
//
// https://arduinojson.org/v6/example/generator/


String serializePacket(Packet packet) {

  // Allocate the JSON document
  //
  // Inside the brackets, 200 is the RAM allocated to this document.
  // Don't forget to change this value to match your requirement.
  // Use https://arduinojson.org/v6/assistant to compute the capacity.

  StaticJsonDocument<1000> doc;

  // Add values in the document
  //
  JsonArray patterns = doc.createNestedArray("patterns");
  for (int i = 0; i < N_PATTERNS; i++) {
    patterns.add(packet.patterns[i]);
  }

  JsonArray speeds = doc.createNestedArray("speeds");
  for (int i = 0; i < N_SPEEDS; i++) {
    speeds.add(packet.speeds[i]);
  }

  JsonArray colors = doc.createNestedArray("colors");
  for (int i = 0; i < N_COLORS; i++) {
    colors.add(packet.colors[i]);
  }

  doc["addon"] = packet.addon;

  // Generate the minified JSON and send it to the Serial port.
  //
  String output;
  int size = serializeJson(doc, output);

  return output;
}



void setup() {
    Serial.begin(115200);
    Serial.println("Hello world!");

    Packet packet;

    // generate packet contents
    //
    const char* colors[] = {"ffffff", "ffff00", "ff0000", "00ff00", "0000ff", "00ffff"};
    for (int i = 0; i < N_COLORS; i++) {
        packet.colors[i] = colors[i];
    }

    bool patterns[N_PATTERNS] = {false, true, true, false, false, false, false, false, false, false, false, false, false, false, false};
    for (int i = 0; i < N_PATTERNS; i++) {
        packet.patterns[i] = patterns[i];
    }

    int speeds[N_SPEEDS] = {30, 45, 50};
    for (int i = 0; i < N_SPEEDS; i++) {
        packet.speeds[i] = speeds[i];
    }

    packet.addon = 0;

    // test contents of packet to make sure it was generated correctly

    Serial.println(packet.colors[0]);
    Serial.println(packet.colors[1]);
    Serial.println(packet.colors[2]);
    Serial.println(packet.addon);
    Serial.println(packet.speeds[0]);

    Serial.println("encoding JSON: ");
    Serial.println("---------------");

    String msg = serializePacket(packet);
    Serial.println(msg);
}

void loop() {
  // pass
}