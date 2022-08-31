#include <DNSServer.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include "ESPAsyncWebServer.h"
#include <ESP8266mDNS.h>
#include <FS.h>

#define DEBUG true

void debug_print(String arg); // function declarations
void debug_print(int arg); // function declarations
void debug_print(char* arg); // function declarations
void debug_println(String arg);
void debug_println(int arg);
void debug_println(char* arg);

#include "packet.h"
Packet packet;

DNSServer dnsServer;
AsyncWebServer server(80);

class CaptiveRequestHandler : public AsyncWebHandler {
public:
  CaptiveRequestHandler() {}
  virtual ~CaptiveRequestHandler() {}

  bool canHandle(AsyncWebServerRequest *request){
    //request->addInterestingHeader("ANY");
    return true;
  }

  void handleRequest(AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/index.html");
    request->send(SPIFFS, "/index.html", "text/html", false);
  }
};

/* hostname for mDNS. Should work at least on windows. Try http://esp8266.local */
const char *myHostname = "esp8266";

// String serializedString;
char* serializedString;

void setup(){

  Serial.begin(115200);
  // test serialization
  //
  // String serialized = serializePacket(packet);
  // debug_println(serialized);
  debug_println("==============================  (#1)");
  // serialized = serializePacket(packet);
  // debug_println(serialized);
  debug_println("==============================  (#2)");
  // serialized = serializePacket(packet);
  // debug_println(serialized);
  debug_println("==============================  (#3)");

  // serializedString = serializePacket(packet);
  // debug_println("Packet has serialized");
  // debug_println(serializedString);

  // debug_println("DeSerializing (creating Packet)");
  // packet = deSerializePacket(serializedString);
  // debug_println("Done DeSerializing !!");

  // serializedString = serializePacket(packet);
  // debug_println("Packet has serialized");
  // debug_println(serializedString);s

  // Initialize SPIFFS
  //
  if(!SPIFFS.begin()){
    debug_println("An Error has occurred while mounting SPIFFS");
    pinMode(16, OUTPUT); // Built-in LED #1
    pinMode(2, OUTPUT); // Built-in LED #2

    while(true) {
      digitalWrite(16, HIGH);
      digitalWrite(2, LOW);
      delay(100);
      digitalWrite(16, LOW);
      digitalWrite(2, HIGH);
      delay(100);
    }
    return;
  }

  //your other setup stuff...
  WiFi.softAP("Juggling Club Controller");
  dnsServer.start(53, "*", WiFi.softAPIP());
  // server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);//only when requested from AP
  //more handlers...

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html");

    // call the user-defined parsing function, below
    //
    debug_println("Received a request");
    packet = parseArgs(request);
    debug_println("Packet has been parsed");
    // serializedString = serializePacket(packet);
    #ifdef DEBUG
    serializedString = serializePacket(packet);
    debug_println("Packet has serialized");
    debug_println(serializedString);
    #else
    serializePacket(packet);
    #endif
    // debug_println("DeSerializing (creating Packet)");
    // packet = deSerializePacket(serializedString);
    // debug_println("Done DeSerializing !!");

    // serializedString = serializePacket(packet);
    // debug_println("Packet has serialized");
    // debug_println(serializedString);

  });

  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });

  // Route to load style.css file
  //
  // server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request){
  //   request->send(SPIFFS, "/favicon.ico");
  // });

  server.begin();
}

void loop() {
  dnsServer.processNextRequest();
}

void debug_print(String arg) {
#ifdef DEBUG
  Serial.print(arg);
#endif
}
void debug_print(int arg) {
#ifdef DEBUG
  Serial.print(arg);
#endif
}
void debug_print(char* arg) {
#ifdef DEBUG
  Serial.print(arg);
#endif
}

void debug_println(String arg) {
#ifdef DEBUG
  Serial.println(arg);
#endif
}
void debug_println(int arg) {
#ifdef DEBUG
  Serial.println(arg);
#endif
}
void debug_println(char* arg) {
#ifdef DEBUG
  Serial.println(arg);
#endif
}