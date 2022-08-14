#ifndef PACKET_SERIALIZATION_LIBRARY
#define PACKET_SERIALIZATION_LIBRARY

#include <ArduinoJson.h>

/*
Helper code for defining a Packet object containing instructions sent from the web browser
for communication via PainlessMesh. Also contains functions for serializing and deserializing
the Packet using JSON.
*/

#define JSON_BUFFER_SIZE 1000

#define N_PATTERNS 15
#define N_ADDONS 7
#define N_COLORS 6
#define N_SPEEDS 3

struct Packet {
  long int colors[N_COLORS];
  int speeds[N_SPEEDS];
  bool patterns[N_PATTERNS];
  int addons[N_ADDONS];

  Packet() {
    long int defaultColors[] = {0, 0, 0, 0, 0, 0};
    for (int i = 0; i < N_COLORS; i++) {
        colors[i] = defaultColors[i];
    }

    bool defaultPatterns[N_PATTERNS] = {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false};
    for (int i = 0; i < N_PATTERNS; i++) {
        patterns[i] = defaultPatterns[i];
    }

    int defaultSpeeds[N_SPEEDS] = {50, 50, 50};
    for (int i = 0; i < N_SPEEDS; i++) {
        speeds[i] = defaultSpeeds[i];
    }

    bool defaultAddons[N_ADDONS] = {false, false, false, false, false, false, false};
    for (int i = 0; i < N_ADDONS; i++) {
        addons[i] = defaultAddons[i];
    }  }
};

String serializePacket(Packet packet) {

  // Allocate the JSON document
  //
  // Inside the brackets, 200 is the RAM allocated to this document.
  // Don't forget to change this value to match your requirement.
  // Use https://arduinojson.org/v6/assistant to compute the capacity.
  StaticJsonDocument<JSON_BUFFER_SIZE> doc;

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

  JsonArray addons = doc.createNestedArray("addons");
  for (int i = 0; i < N_ADDONS; i++) {
    addons.add(packet.addons[i]);
  }

  // Generate the minified JSON and store it as a string.
  //
  String output;
  int size = serializeJson(doc, output);

  return output;
}

Packet deSerializePacket(String input) {
  // Allocate the JSON document
  //
  // Inside the brackets, 200 is the capacity of the memory pool in bytes.
  // Don't forget to change this value to match your JSON document.
  // Use https://arduinojson.org/v6/assistant to compute the capacity.
  StaticJsonDocument<JSON_BUFFER_SIZE> doc;

  Packet packet; // object to return

  // JSON input string.
  //
  // Copy Arduino String object to char array
  char json[JSON_BUFFER_SIZE];
  input.toCharArray(json, JSON_BUFFER_SIZE);
  Serial.println("I have successfully converted to char array.");

  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, json);

  // Test if parsing succeeds.
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return packet;
  }


  // Parse values and populate the Packet object.
  //
  // Most of the time, you can rely on the implicit casts.
  // In other case, you can do doc["time"].as<long>();

  for (int i = 0; i < N_COLORS; i++) {
      packet.colors[i] = doc["colors"][i];
  }

  for (int i = 0; i < N_PATTERNS; i++) {
      packet.patterns[i] = doc["patterns"][i];
  }

  for (int i = 0; i < N_SPEEDS; i++) {
      packet.speeds[i] = doc["speeds"][i];
  }

  for (int i = 0; i < N_ADDONS; i++) {
      packet.addons[i] = doc["addons"][i];
  }

  return packet;
}

#endif