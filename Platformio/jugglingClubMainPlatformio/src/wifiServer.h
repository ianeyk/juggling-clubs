#ifndef WIFI_SERVER_LIBRARY
#define WIFI_SERVER_LIBRARY

#include "IPAddress.h"
#include <FS.h>

#ifdef ESP8266
#include "Hash.h"
#include <ESPAsyncTCP.h>
#else
#include <AsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>

#define   STATION_SSID     "Ian's Juggling Clubss"
#define   STATION_PASSWORD "circusLuminescence"

#define HOSTNAME "HTTP_BRIDGE"

DNSServer dnsServer;
AsyncWebServer server(80);
IPAddress myIP(192,168,1,1);
IPAddress subnet(255,255,255,0);

class CaptiveRequestHandler : public AsyncWebHandler {
public:
  CaptiveRequestHandler() {
          /* THIS IS WHERE YOU CAN PLACE THE CALLS */
        server.onNotFound([](AsyncWebServerRequest *request){
        AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/index.html");
        request->send(response);
       });

      server.on("/ncsi.txt", HTTP_GET, [](AsyncWebServerRequest *request) {
        AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "Microsoft NCSI");
        request->send(response);
      });

      server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
        AsyncWebServerResponse* response = request->beginResponse(LittleFS, "/style.css");
         request->send(response);
      });

      server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request) {
        AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/index.html");
        request->send(response);
      });

      server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Attempting to send HTML Page");
        Serial.println("Spare Heap Remaining = " + String(ESP.getFreeHeap()));
        AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/index.html");
        request->send(response);
        Serial.println("Sent HTML Page");
      });
  }
  virtual ~CaptiveRequestHandler() {}

  unsigned long lastRequestHandleTimestamp = 0;
  unsigned long minimumHandlePeriod = 2 * 1000;
  bool canHandle(AsyncWebServerRequest *request){
    //request->addInterestingHeader("ANY");
    if (millis() - lastRequestHandleTimestamp < minimumHandlePeriod) {
        return false;
    } // else
    lastRequestHandleTimestamp = millis();
    return true;
  }

  void handleRequest(AsyncWebServerRequest *request) {
    // AsyncResponseStream *response = request->beginResponseStream("text/html");
    Serial.print("The initial request was: ");
    Serial.println(request->url());
    //List all collected headers (Compatibility)
    int headers = request->headers();
    int i;
    for (i=0;i<headers;i++) {
        Serial.printf("HEADER[%s]: %s\n", request->headerName(i).c_str(), request->header(i).c_str());
    }
    // response->print("<!DOCTYPE html><html><head><title>Captive Portal</title></head><body>");
    // response->print("<p>This is out captive portal front page.</p>");
    // response->printf("<p>You were trying to reach: http://%s%s</p>", request->host().c_str(), request->url().c_str());
    // response->printf("<p>Try opening <a href='http://%s/index.html'>this link</a> instead</p>", WiFi.softAPIP().toString().c_str());
    // response->print("</body></html>");
    // Serial.println("Sending response now!");
    // request->send(response);
    Serial.println("Sending redirect now!");
    request->redirect("/");
    Serial.println("Sent Response.");
    Serial.println("Spare Heap Remaining = " + String(ESP.getFreeHeap()));
  }
};

void setupWifiServer() {
  // Initialize LittleFS
  if(!LittleFS.begin()){
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }

  // Async webserver
  // WiFi.softAP(STATION_SSID, STATION_PASSWORD);
  // WiFi.softAPConfig(myIP, myAPIP, subnet);
  WiFi.softAP(STATION_SSID);
  delay(100);
  dnsServer.start(53, "*", WiFi.softAPIP());
  server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER); //only when requested from AP
  // start web server
  server.begin();
}

#endif