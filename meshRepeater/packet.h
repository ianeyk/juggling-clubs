#ifndef PACKET_SERIALIZATION_LIBRARY
#define PACKET_SERIALIZATION_LIBRARY

#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>

#define IN_MESH_REPEATER
/*
Helper code for defining a Packet object containing instructions sent from the web browser
for communication via PainlessMesh. Also contains functions for serializing and deserializing
the Packet using JSON.
*/

#define JSON_BUFFER_SIZE (1024 * 8) // used to be just 1000

#define N_PATTERNS 10 // look at the length of pattern_names in construct_html.py
#define N_ADDONS 8 // look at the length of pattern_options_definitions
#define N_COLORS 15 // look at the number of addins under pattern_options_definitions for which show_colors == True, multiplied by 3
#define N_SPEEDS 14 // look at the number of addins under pattern_options_definitions for sliders_only == False, multiplied by 2, + the number of addins under pattern_options_definitions for which sliders_only == True, multiplied by 1

struct Packet {
  bool patterns[N_PATTERNS];
  long int colors[N_PATTERNS][N_COLORS]; // [N_PATTERNS][N_COLORS];
  int speeds[N_PATTERNS][N_SPEEDS]; // [N_PATTERNS][N_SPEEDS];
  bool addons[N_PATTERNS][N_ADDONS]; // [N_PATTERNS][N_ADDONS];
  int currentPattern;
  int currentHue;
  int currentOffset;

  Packet() {
    // bool defaultPatterns[N_PATTERNS] = {false, false, false, false, false, false, false, false, false, false};
    for (int p = 0; p < N_PATTERNS; p++) {
        // patterns[p] = defaultPatterns[p];
        patterns[p] = false;
    }

    // long int defaultColors[] = {16711880, 0, 0, 16581374, 0, 0}; // initialize with hot pink
    for (int p = 0; p < N_PATTERNS; p++) {
      for (int c = 0; c < N_COLORS; c++) {
          // colors[p][c] = defaultColors[c];
          colors[p][c] = 0;
      }
    }

    // int defaultSpeeds[N_SPEEDS] = {10, 20, 50};
    for (int p = 0; p < N_PATTERNS; p++) {
      for (int s = 0; s < N_SPEEDS; s++) {
          // speeds[p][s] = defaultSpeeds[s];
          speeds[p][s] = 50;
      }
    }

    // bool defaultAddons[N_ADDONS] = {false, false, false, false, false, false, false};
    for (int p = 0; p < N_PATTERNS; p++) {
      for (int a = 0; a < N_ADDONS; a++) {
          // addons[p][a] = defaultAddons[a];
          addons[p][a] = false;
      }
    }

    currentPattern = 0;
    currentHue = 0;
    currentOffset = 0;
  }
};

  #ifdef IN_MESH_REPEATER
char deserializedPacketOutput[JSON_BUFFER_SIZE];
// char* serializePacket(Packet packet) {
void serializePacket(Packet packet) {
  #else
void serializePacket(Packet packet) {
  #endif
// DynamicJsonDocument doc(JSON_BUFFER_SIZE);

// void serializePacket(Packet packet) {
  debug_println("Beginning Serialization");
  // Allocate the JSON document
  //
  // Inside the brackets, 200 is the RAM allocated to this document.
  // Don't forget to change this value to match your requirement.
  // Use https://arduinojson.org/v6/assistant to compute the capacity.
  // doc.clear();
  // doc.garbageCollect();
  DynamicJsonDocument doc(JSON_BUFFER_SIZE);
  // doc.shrinkToFit();
  // deserializedPacketOutput = "";
  debug_println("Document has been set up");

  // Add values in the document
  //
  JsonArray patterns = doc.createNestedArray("patterns");
  for (int i = 0; i < N_PATTERNS; i++) {
    debug_println("adding pattern " + String(i));
    patterns.add(packet.patterns[i]);
  }

  JsonArray outerSpeeds = doc.createNestedArray("speeds");
  for (int p = 0; p < N_PATTERNS; p++) {
    JsonArray innerSpeeds = outerSpeeds.createNestedArray();
    for (int s = 0; s < N_SPEEDS; s++) {
      innerSpeeds.add(packet.speeds[p][s]);
      debug_println("adding speed [" + String(p) + "][" + String(s) + "] .");
    }
  }

  JsonArray outerColors = doc.createNestedArray("colors");
  for (int p = 0; p < N_PATTERNS; p++) {
    JsonArray innerColors = outerColors.createNestedArray();
    for (int c = 0; c < N_COLORS; c++) {
      innerColors.add(packet.colors[p][c]);
      debug_println("adding color [" + String(p) + "][" + String(c) + "] .");
    }
  }

  JsonArray outerAddons = doc.createNestedArray("addons");
  for (int p = 0; p < N_PATTERNS; p++) {
    JsonArray innerAddons = outerAddons.createNestedArray();
    for (int a = 0; a < N_ADDONS; a++) {
      innerAddons.add(packet.addons[p][a]);
      debug_println("adding addon [" + String(p) + "][" + String(a) + "] .");
    }
  }

  doc["currentPattern"] = packet.currentPattern;
  doc["currentHue"] = packet.currentHue;
  doc["currentOffset"] = packet.currentOffset;

  // Generate the minified JSON and store it as a string.
  //
  debug_println("Serializing Document");
  // int size = serializeJson(doc, Serial); // This works nicely for the deployment version!
  // deserializedPacketOutput = "";
  #ifdef IN_MESH_REPEATER
  int size = serializeJson(doc, deserializedPacketOutput);
  #else
  int size = serializeJson(doc, Serial);
  Serial.println("");
  #endif
  debug_println("Document has been serialized");
  debug_print("with size: ");
  debug_println(size);


  debug_print("memory usage after serializing is: ");
  debug_print(doc.memoryUsage());
  debug_println(" bytes");

  doc.clear();
  doc.garbageCollect();

  #ifdef IN_MESH_REPEATER
  Serial.print("Output of deserialization was: ");
  Serial.println(deserializedPacketOutput);
  // return deserializedPacketOutput;
  #else
  return;
  #endif
}

Packet deSerializePacket(String input) {
  debug_print("Input was: ");
  debug_println(input);
  // Allocate the JSON document
  //
  // Inside the brackets, 200 is the capacity of the memory pool in bytes.
  // Don't forget to change this value to match your JSON document.
  // Use https://arduinojson.org/v6/assistant to compute the capacity.
  DynamicJsonDocument doc(JSON_BUFFER_SIZE);
  // doc.clear();
  // doc.garbageCollect();
  // doc.shrinkToFit(); // DO NOT ADD THIS LINE

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
    Serial.println("We Encountered an Error !!!: ");
    Serial.println(error.c_str());
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

  doc.clear();
  doc.garbageCollect();

  return packet;
}

int getPatternFromArgId(String argId) {
  return argId.substring(1, 3).toInt();
}
int getIndexFromArgId(String argId) {
  return argId.substring(3, 5).toInt();
}

void printParsingStep(String argType, int pat, int idx, long int val);

Packet parseArgs(AsyncWebServerRequest *request) {
  debug_println("Beginning to parse args!!!");

  Packet packet; // generate packet to send, with default values
  debug_println("Packet initialized.");

  int params = request->params();
  debug_println("Params initialized.");
  for (int i = 0; i < params; i++) {
    debug_println("getting param" + String(i));
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
      printParsingStep("Color", pat, idx, color_int);
    }

    else if (name.startsWith("s")) {
      int speed_int = value.toInt();
      packet.speeds[pat][idx] = speed_int;
      printParsingStep("Speed", pat, idx, (long int)speed_int);
    }

    else if (name.startsWith("p")) {
      packet.patterns[pat] = true; // ignore the value
      printParsingStep("Pattern", pat, pat, 1);
    }

    else if (name.startsWith("a")) {
      packet.addons[pat][idx] = true;
      printParsingStep("Addin", pat, idx, 1);
    }

  }

  debug_println("returning Packet");
  return packet;

}

void printParsingStep(String argType, int pat, int idx, long int val) {
    debug_print("Setting ");
    debug_print(argType);
    debug_print("[");
    debug_print(pat);
    debug_print("][");
    debug_print(idx);
    debug_print("] to ");
    debug_print(val);
    debug_println(" !");
}

#endif