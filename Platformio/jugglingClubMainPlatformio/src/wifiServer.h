#ifndef WIFI_SERVER_LIBRARY
#define WIFI_SERVER_LIBRARY

#include "IPAddress.h"
#include <FS.h>
// #include <SPIFFS.h>

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

DNSServer dnsServer;
AsyncWebServer server(80);
IPAddress myIP(192,168,1,1);
IPAddress subnet(255,255,255,0);

std::vector<String> respondWithPage{"/index.html", "/style.css", "/css", "/index.js.download", "/webfontloader.js", "/favicon.ico", "/iconfont.woff2"};
std::vector<String> jsFiles{"/index.js.download", "/webfontloader.js"};

bool inStringArray(std::vector<String> myList, String stringToMatch) {
  for (unsigned int i = 0; i < myList.size() ; i ++) {
    if (stringToMatch == myList[i]){
      return true;
    }
  }
  return false;
}

class CaptiveRequestHandler : public AsyncWebHandler {
public:
  CaptiveRequestHandler() {}
  virtual ~CaptiveRequestHandler() {}

  bool canHandle(AsyncWebServerRequest *request){
    // return inStringArray(respondWithPage, request->url());
    return true; // make sue CaptiveRequestHandler is enabled after canHandle
  }

  void handleRequest(AsyncWebServerRequest *request) {
    Serial.print("The initial request was: ");
    Serial.println(request->url());

    for (unsigned int i = 0; i < respondWithPage.size() ; i ++) {
      if (request->url() == respondWithPage[i]){
        AsyncWebServerResponse *response;
        Serial.println("received request for " + respondWithPage[i]);
        if (inStringArray(jsFiles, respondWithPage[i])) {
          response = request->beginResponse(SPIFFS, respondWithPage[i], "text/javascript");
        } else {
          response = request->beginResponse(SPIFFS, respondWithPage[i]);
        }
        request->send(response);
        return;
      }
    } // if page name not found in respondWithPage:
    Serial.print("received arbitrary request");
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    response->print("<!DOCTYPE html><html><head><title>Ian's Juggling Club Home Page</title></head><body>");
    response->printf("<p>To proceed, click <a href='http://%s/index.html'>here</a>.</p>", WiFi.softAPIP().toString().c_str());
    response->print("</body></html>");
    request->send(response);
  }
};

void setupWifiServer() {
  // Initialize SPIFFS
  if (!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // if (!SPIFFS.format()) {
  //   Serial.println("An Error has occurred while formatting SPIFFS");
  //   return;
  // }

    for (unsigned int i = 0; i < respondWithPage.size() ; i ++) {
      if (SPIFFS.exists(respondWithPage[i])) {
        Serial.println("File system: " + respondWithPage[i] + " exists.");
      } else {
        Serial.println("File system: " + respondWithPage[i] + " DOES NOT exist!!!.");
      }
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