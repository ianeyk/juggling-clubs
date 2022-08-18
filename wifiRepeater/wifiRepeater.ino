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

void setup(){

  Serial.begin(115200);
  // Initialize SPIFFS
  //
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
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
    packet = parseArgs(request);
    String serialized = serializePacket(packet);
    Serial.println(serialized);
  });

  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });

  server.begin();
}

void loop(){
  dnsServer.processNextRequest();
}