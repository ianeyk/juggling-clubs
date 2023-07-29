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

std::vector<String> doNotCaptivePortal{"/index.html", "/style.css", "/wpad.dat"};
std::vector<String> respondWithPage{"/index.html", "/style.css"};

bool inStringArray(std::vector<String> myList, String stringToMatch) {
  for (unsigned int i = 0; i < myList.size() ; i ++){
    if (stringToMatch == myList[i]){
      return true;
    }
  }
  return false;
}

class ServerRequestHandler : public AsyncWebHandler {
public:
  ServerRequestHandler() {}
  virtual ~ServerRequestHandler() {}

  bool canHandle(AsyncWebServerRequest *request){
    // check if url matches a string in array respondWithPage
    return inStringArray(respondWithPage, request->url());
  }

  void handleRequest(AsyncWebServerRequest *request) {
        AsyncWebServerResponse *response;
        if (request->url() == "/style.css") {
          response = request->beginResponse(LittleFS, "/style.css");
        }
        else {
          response = request->beginResponse(LittleFS, "/index.html");
        }
        Serial.print("Attempting to send HTML Page; ");
        Serial.println("Spare Heap Remaining = " + String(ESP.getFreeHeap()));
        request->send(response);
        Serial.print("Sent HTML Page; ");
        Serial.println("After sending HTML Page, Spare Heap Remaining = " + String(ESP.getFreeHeap()));
  }
};

class CaptiveRequestHandler : public AsyncWebHandler {
public:
  CaptiveRequestHandler() {}
  virtual ~CaptiveRequestHandler() {}

  bool canHandle(AsyncWebServerRequest *request){
    // check if url matches a string in array doNotCaptivePortal
    return 1 - inStringArray(doNotCaptivePortal, request->url());
    // return true; // make sue CaptiveRequestHandler is enabled after canHandle
  }

  void handleRequest(AsyncWebServerRequest *request) {
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    Serial.print("The initial request was: ");
    Serial.println(request->url());
    // response->print("<!DOCTYPE html><html><head><title>Captive Portal</title></head><body>");
    // response->print("<p>This is out captive portal front page.</p>");
    // response->printf("<p>You were trying to reach: http://%s%s</p>", request->host().c_str(), request->url().c_str());
    // response->printf("<p>Try opening <a href='http://%s/index.html'>this link</a> instead</p>", WiFi.softAPIP().toString().c_str());
    // response->print("</body></html>");
    // Serial.println("Sending response now!");
    // request->send(response);
    Serial.println("Sending redirect now!");
    request->redirect("/index.html");
    // Serial.println("Sent Response.");
    // Serial.println("Spare Heap Remaining = " + String(ESP.getFreeHeap()));
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
  server.addHandler(new ServerRequestHandler()).setFilter(ON_AP_FILTER); //only when requested from AP
  server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER); //only when requested from AP
  // start web server
  server.begin();
}

#endif