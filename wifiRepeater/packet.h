#ifndef PACKET_SERIALIZATION_LIBRARY
#define PACKET_SERIALIZATION_LIBRARY

#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>

/*
Helper code for defining a Packet object containing instructions sent from the web browser
for communication via PainlessMesh. Also contains functions for serializing and deserializing
the Packet using JSON.
*/

#define JSON_BUFFER_SIZE 1000 * 100 // used to be just 1000

#define N_PATTERNS 10 // look at the length of pattern_names in construct_html.py
#define N_ADDONS 8 // look at the length of pattern_options_definitions
#define N_COLORS 15 // look at the number of addins under pattern_options_definitions for which show_colors == True, multiplied by 3
#define N_SPEEDS 14 // look at the number of addins under pattern_options_definitions for sliders_only == False, multiplied by 2, + the number of addins under pattern_options_definitions for which sliders_only == True, multiplied by 1

struct Packet {
  bool patterns[N_PATTERNS];
  long int colors[N_PATTERNS][N_COLORS]; // [N_PATTERNS][N_COLORS];
  int speeds[N_PATTERNS][N_SPEEDS]; // [N_PATTERNS][N_SPEEDS];
  int addons[N_PATTERNS][N_ADDONS]; // [N_PATTERNS][N_ADDONS];
  int currentPattern;
  int currentHue;
  int currentOffset;

  Packet() {
    bool defaultPatterns[N_PATTERNS] = {false, false, false, false, false, false, false, false, false, false};
    for (int p = 0; p < N_PATTERNS; p++) {
        patterns[p] = defaultPatterns[p];
    }

    long int defaultColors[] = {16711880, 0, 0, 16581374, 0, 0}; // initialize with hot pink
    for (int p = 0; p < N_PATTERNS; p++) {
      for (int c = 0; c < N_COLORS; c++) {
          colors[p][c] = defaultColors[c];
      }
    }

    int defaultSpeeds[N_SPEEDS] = {10, 20, 50};
    for (int p = 0; p < N_PATTERNS; p++) {
      for (int s = 0; s < N_SPEEDS; s++) {
          speeds[p][s] = defaultSpeeds[s];
      }
    }

    bool defaultAddons[N_ADDONS] = {false, false, false, false, false, false, false};
    for (int p = 0; p < N_PATTERNS; p++) {
      for (int a = 0; a < N_ADDONS; a++) {
          addons[p][a] = defaultAddons[a];
      }
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

  JsonArray outerSpeeds = doc.createNestedArray("speeds");
  for (int p = 0; p < N_PATTERNS; p++) {
    JsonArray innerSpeeds = doc.createNestedArray("speeds");
    for (int s = 0; s < N_SPEEDS; s++) {
      innerSpeeds.add(packet.speeds[s]);
    }
    outerSpeeds.add(innerSpeeds);
  }

  JsonArray outerColors = doc.createNestedArray("colors");
  for (int p = 0; p < N_PATTERNS; p++) {
    JsonArray innerColors = doc.createNestedArray("colors");
    for (int c = 0; c < N_COLORS; c++) {
      innerColors.add(packet.colors[c]);
    }
    outerColors.add(innerColors);
  }

  JsonArray outerAddons = doc.createNestedArray("addons");
  for (int p = 0; p < N_PATTERNS; p++) {
    JsonArray innerAddons = doc.createNestedArray("addons");
    for (int a = 0; a < N_ADDONS; a++) {
      innerAddons.add(packet.addons[a]);
    }
    outerAddons.add(innerAddons);
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
  for (int i = 0; i < N_PATTERNS; i++) {
      packet.patterns[i] = doc["patterns"][i];
  }

  for (int p = 0; p < N_PATTERNS; p++) {
    for (int c = 0; c < N_COLORS; c++) {
        packet.colors[p][c] = doc["colors"][p][c];
    }
  }

  for (int p = 0; p < N_PATTERNS; p++) {
    for (int s = 0; s < N_SPEEDS; s++) {
        packet.speeds[p][s] = doc["speeds"][p][s];
    }
  }

  for (int p = 0; p < N_PATTERNS; p++) {
    for (int a = 0; a < N_ADDONS; a++) {
        packet.addons[p][a] = doc["addons"][p][a];
    }
  }

  packet.currentPattern = doc["currentPattern"];
  packet.currentHue = doc["currentHue"];
  packet.currentOffset = doc["currentOffset"];

  return packet;
}

int getPatternFromArgId(String argId) {
  return argId.substring(1, 3).toInt();
}
int getIndexFromArgId(String argId) {
  return argId.substring(3, 5).toInt();
}

Packet parseArgs(AsyncWebServerRequest *request) {

  Packet packet; // generate packet to send, with default values

  int params = request->params();
  for (int i = 0; i < params; i++) {
    // get one parameter at a time
    AsyncWebParameter* p = request->getParam(i);
    String name = String(p->name());//.c_str();
    String value = String(p->value());//.c_str();
    int pat = getPatternFromArgId(name);
    int idx = getIndexFromArgId(name);

    // start interpreting the parameter, and use it to populate one field in the packet
    //
    if (name.startsWith("c")) {
      String temp_substring = value.substring(1);
      std::string temp_c_str = temp_substring.c_str();
      const char* temp_const_char = temp_c_str.c_str();
      long int color_int = strtol(temp_const_char, NULL, 16);

      packet.colors[pat][idx] = color_int;
    }

    else if (name.startsWith("s")) {
      int speed_int = value.toInt();
      packet.speeds[pat][idx] = speed_int;
    }

    else if (name.startsWith("p")) {
      packet.patterns[pat] = true; // ignore the value
    }

    else if (name.startsWith("a")) {
      packet.addons[pat][idx] = true;
    }

  }

  return packet;

}

#endif