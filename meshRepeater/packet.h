#ifndef PACKET_SERIALIZATION_LIBRARY
#define PACKET_SERIALIZATION_LIBRARY

#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>

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
  int currentPattern;
  int currentHue;
  int currentOffset;

  Packet() {
    long int defaultColors[] = {16711880, 0, 0, 16581374, 0, 0}; // initialize with hot pink
    for (int i = 0; i < N_COLORS; i++) {
        colors[i] = defaultColors[i];
    }

    bool defaultPatterns[N_PATTERNS] = {true, false, false, false, false, false, false, false, false, false, false, false, false, false, false};
    for (int i = 0; i < N_PATTERNS; i++) {
        patterns[i] = defaultPatterns[i];
    }

    int defaultSpeeds[N_SPEEDS] = {10, 20, 50};
    for (int i = 0; i < N_SPEEDS; i++) {
        speeds[i] = defaultSpeeds[i];
    }

    bool defaultAddons[N_ADDONS] = {true, true, false, false, false, false, false};
    for (int i = 0; i < N_ADDONS; i++) {
        addons[i] = defaultAddons[i];
    }

    currentPattern = 0;
    currentHue = 0;
    currentOffset = 0;
  }
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

  doc["currentPattern"] = packet.currentPattern;
  doc["currentHue"] = packet.currentHue;
  doc["currentOffset"] = packet.currentOffset;

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

  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, json);

  // Test if parsing succeeds.
  if (error) {
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

  packet.currentPattern = doc["currentPattern"];
  packet.currentHue = doc["currentHue"];
  packet.currentOffset = doc["currentOffset"];

  return packet;
}

Packet parseArgs(AsyncWebServerRequest *request) {

  Packet packet; // generate packet to send, with default values

  int params = request->params();
  for (int i = 0; i < params; i++) {
    // get one parameter at a time
    AsyncWebParameter* p = request->getParam(i);
    String name = String(p->name());//.c_str();
    String value = String(p->value());//.c_str();

    // start interpreting the parameter, and use it to populate one field in the packet
    //
    if (name.startsWith("c")) {
      int idx = name.substring(1, 2).toInt();

      String temp_substring = value.substring(1);
      std::string temp_c_str = temp_substring.c_str();
      const char* temp_const_char = temp_c_str.c_str();
      long int color_int = strtol(temp_const_char, NULL, 16);

      packet.colors[idx] = color_int;
    }

    else if (name.startsWith("s")) {
      int idx = name.substring(1, 2).toInt();

      int speed_int = value.toInt();

      packet.speeds[idx] = speed_int;

    }

    else if (name.startsWith("p")) {
      int idx = name.substring(1, 3).toInt(); // read two digits, not one

      // ignore the value

      packet.patterns[idx] = true;
    }

    else if (name.startsWith("a")) {
      int idx = name.substring(1, 3).toInt(); // read two digits, not one

      packet.addons[idx] = true;
    }

  }

  return packet;

}

#endif