#ifndef WIFI_SERVER_LIBRARY
#define WIFI_SERVER_LIBRARY

#include "IPAddress.h"
#include <FS.h>
// #include <SPIFFS.h>

#ifdef ESP32
  #include <WiFi.h>
  #include <AsyncTCP.h>
  // #include <WiFiUdp.h>
  // #include "ESPAsyncUDP.h"
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>

#define   STATION_SSID     "Ian's Juggling Clubss"
#define   STATION_PASSWORD "circusLuminescence"
#define splashPageFileName "/splashpage.html" // to minimize typos
// #define splashPageFileName "/connecttest.txt" // to minimize typos

DNSServer dnsServer;
AsyncWebServer server(80);
IPAddress myIP(192,168,1,1);
IPAddress gatewayIP(192,168,1,4);
IPAddress subnet(255,255,255,0);

std::vector<const char*> respondWithPage{"/index.html", "/style.css", "/css", "/index.js.download", "/webfontloader.js", "/favicon.ico", "/iconfont.woff2", splashPageFileName};
std::vector<const char*> jsFiles{"/index.js.download", "/webfontloader.js"};

bool inStringArray(std::vector<const char*> myList, const char* stringToMatch) {
  for (unsigned int i = 0; i < myList.size() ; i ++) {
    if (strcmp(stringToMatch, myList[i]) == 0){
      return true;
    }
  }
  return false;
}

void onRequest(AsyncWebServerRequest *request){
  //Handle Unknown Request
  Serial.println("Sending 404. Memory is now " + String(ESP.getFreeHeap()));
  request->send(404);
  Serial.println("Sent 404. Memory is now " + String(ESP.getFreeHeap()));
}

class CaptiveRequestHandler : public AsyncWebHandler {
public:
  CaptiveRequestHandler() {}
  virtual ~CaptiveRequestHandler() {}

String request_url = "";
  bool canHandle(AsyncWebServerRequest *request){
    // return inStringArray(respondWithPage, request->url());

    Serial.println("requesting url. Memory is now " + String(ESP.getFreeHeap()));
    Serial.print("The initial request was: ");
    request_url = request->url();
    // Serial.println("requested url. Memory is now " + String(ESP.getFreeHeap()));
    Serial.println(request_url);
    bool canHandleVal = !(request_url.equals("/wpad.dat"));
    Serial.println("checked .equals() on url. Memory is now " + String(ESP.getFreeHeap()));
    if (canHandleVal) {
      Serial.println("it's not the same (canHandleVal is true)");
    } else {
      Serial.println("it IS the same (canHandleVal is false)");
    }
    return canHandleVal;

    // return true; // make sue CaptiveRequestHandler is enabled after canHandle
  }

  void handleRequest(AsyncWebServerRequest *request) {
    Serial.print("The initial request was: ");
    Serial.print(request->url());
    Serial.println(". Memory is " + String(ESP.getFreeHeap()));
    // userScheduler.pause();

    for (unsigned int i = 0; i < respondWithPage.size() ; i ++) {
      if (strcmp(request->url().c_str(), respondWithPage[i]) == 0){
        AsyncWebServerResponse *response;
        // Serial.println("received request for " + respondWithPage[i]);
        if (inStringArray(jsFiles, respondWithPage[i])) {
          response = request->beginResponse(SPIFFS, respondWithPage[i], "text/javascript");
        } else {
          response = request->beginResponse(SPIFFS, respondWithPage[i]);
        }
        request->send(response);
        // userScheduler.resume();
        return;
      }
    } // if page name not found in respondWithPage:
    Serial.println("That did not match a file. Memory is " + String(ESP.getFreeHeap()));
    AsyncWebServerResponse *response;
    response = request->beginResponse(SPIFFS, splashPageFileName);
    request->send(response);
    // AsyncResponseStream *response = request->beginResponseStream("text/html");
    // response->print("<!DOCTYPE html><html><head><title>Ian's Juggling Club Home Page</title></head><body>");
    // response->printf("<p>To proceed, click <a href='http://%s/index.html'>here</a>.</p>", WiFi.softAPIP().toString().c_str());
    // response->print("</body></html>");
    // request->send(response);
    // userScheduler.resume();
    Serial.println("After sending home page text, memory is " + String(ESP.getFreeHeap()));
  }
};

    void writeSplashPageFile();
    String fileContentsBefore = "<!DOCTYPE html><html><head><title>Ian's Juggling Club Home Page</title></head><body><p>To proceed, click <a href='http://";
    String fileContentsAfter = "/index.html'>here</a>.</p></body></html>";

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
        Serial.print("File system: "); Serial.print(respondWithPage[i]); Serial.println(" exists.");
      } else {
        Serial.print("File system: "); Serial.print(respondWithPage[i]); Serial.print(" DOES NOT exist!!!.");
      }
    }

  // Async webserver
  // WiFi.softAP(STATION_SSID, STATION_PASSWORD);
  WiFi.softAPConfig(myIP, gatewayIP, subnet);
  WiFi.softAP(STATION_SSID);
  delay(100);
  dnsServer.start(53, "*", WiFi.softAPIP());
  server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER); //only when requested from AP
  server.onNotFound(onRequest);

  // start web server
  server.begin();
  writeSplashPageFile();
}

void writeSplashPageFile() {
  if (SPIFFS.exists(splashPageFileName)) {
    SPIFFS.remove(splashPageFileName);
  }

  File file = SPIFFS.open(splashPageFileName, "w");

  if (!file) {
      Serial.println("There was an error opening the file for writing");
      return;
  }

  if (file.print("TEST")) {
      Serial.println("File was written");
  } else {
      Serial.println("File write failed");
  }

    file.print(fileContentsBefore);
    file.print(String(WiFi.softAPIP().toString().c_str()));
    file.print(fileContentsAfter);

  file.close();
}

#endif