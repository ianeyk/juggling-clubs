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

  AsyncResponseStream *response = request->beginResponseStream("text/html");
  // AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/index.html");

    // response->print("<!DOCTYPE html><html><head><title>Captive Portal</title></head><body>");
    // response->print("<p>This is out captive portal front page.</p>");
    // response->printf("<p>You were trying to reach: http://%s%s</p>", request->host().c_str(), request->url().c_str());
    // response->printf("<p>Try opening <a href='http://%s'>this link</a> instead</p>", WiFi.softAPIP().toString().c_str());
    // response->print("</body></html>");

    Serial.println("Handling Request...");
    request->send(SPIFFS, "/index.html", "text/html", false);
    // request->send(response);
    Serial.println("Request handled!");
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
    return;
  }

  Serial.begin(115200);
  //your other setup stuff...
  WiFi.softAP("esp-captive");
  dnsServer.start(53, "*", WiFi.softAPIP());
  // // Setup MDNS responder
  // if (!MDNS.begin(myHostname)) {
  //   Serial.println("Error setting up MDNS responder!");
  // } else {
  //   Serial.println("mDNS responder started");
  //   // Add service to MDNS-SD
  //   MDNS.addService("http", "tcp", 80);
  // }
  // server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);//only when requested from AP
  //more handlers...

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html");

    // call the user-defined parsing function, below
    //
    packet = parseArgs(request);
    Serial.println("-----------------------");
    Serial.println("Received JSON message: ");
    Serial.println("-----------------------");
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