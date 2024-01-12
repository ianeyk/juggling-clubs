#ifndef WIFI_SERVER_LIBRARY
#define WIFI_SERVER_LIBRARY

#include "IPAddress.h"
#include "jsonPacket.h"

#include <FS.h>
#include <LittleFS.h> // comment this out to use SPIFFS
#define CONFIG_LITTLEFS_SPIFFS_COMPAT 1 // also necessary for LittleFS

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
// #define splashPageFileName "/connecttest.txt" // to minimize typos

void sendMessage(const char*);

DNSServer dnsServer;
AsyncWebServer server(80);        // Our server instance; put listen events on this
IPAddress myIP(192,168,1,1);
// IPAddress gatewayIP(0,0,0,0);
IPAddress gatewayIP(192,168,1,4);
IPAddress subnet(255,255,255,0);

#define splashPageFileName "/splashpage.html" // to minimize typos
#define programStorageFileName "/programStorageFile.json"
std::vector<const char*> respondWithPage{"/index.html", "/style.css", "/css", "/index.js.download", "/webfontloader.js", "/favicon.ico", "/iconfont.woff2", splashPageFileName, programStorageFileName};
std::vector<const char*> jsFiles{"/index.js.download", "/webfontloader.js"};

#define MAX_MESSAGE_SIZE 6144   // chars
char incomingDataBuffer[MAX_MESSAGE_SIZE + 1];

#include "fileSystem.h"

bool inStringArray(std::vector<const char*> myList, const char* stringToMatch) {
  for (unsigned int i = 0; i < myList.size() ; i ++) {
    if (strcmp(stringToMatch, myList[i]) == 0){
      return true;
    }
  }
  return false;
}

void handleNotFound(AsyncWebServerRequest *request){
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
          response = request->beginResponse(LittleFS, respondWithPage[i], "text/javascript");
        } else {
          response = request->beginResponse(LittleFS, respondWithPage[i]);
        }
        request->send(response);
        // userScheduler.resume();
        return;
      }
    } // if page name not found in respondWithPage:
    Serial.println("That did not match a file. Memory is " + String(ESP.getFreeHeap()));
    AsyncWebServerResponse *response;
    response = request->beginResponse(LittleFS, splashPageFileName);
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



class PatternHandler : public AsyncWebHandler {
// Handle POST to /submit
public:
  PatternHandler() {}
  virtual ~PatternHandler() {}

String request_url = "";
  bool canHandle(AsyncWebServerRequest *request){
    if (request->method() != HTTP_POST) {
      return false;
    }

    return request->url().equals("/submit");
  }


  // void handleRequest(AsyncWebServerRequest *request) {
  //   // AsyncWebServerResponse *response;
  //   // response = request->beginResponse(LittleFS, splashPageFileName);
  //   // request->send(response);
  //   Serial.println("Handling pattern request");
  //   request->send(200, "text/plain", "Received!");
  // }


// #define MAX_PATTERN_SIZE 1000
// char pattern[MAX_PATTERN_SIZE + 1];


void handleBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){

  if(index == 0) {
    newProgramsArriving();
  }

  for(size_t i = 0; i < len; i++){
    incomingDataBuffer[i + index] = data[i];
  }


  if(index + len < total) {
    return;
  }

  incomingDataBuffer[total+1] = '\0';   // Make sure our buffer terminates

  Serial.println("Deserializing packet: " + String((const char *)incomingDataBuffer)); // TODO: Delete
  const char* error = readJsonDocument(incomingDataBuffer);

  if(error) {
    request->send(422, "text/plain", error);    // HTTP 422 Unprocessable Entity
    return;
  }

  request->send(200);
  sendMessage(incomingDataBuffer);
}


};


void setupWifiServer() {
  setupFileSystem();
  // Async webserver
  // WiFi.softAP(STATION_SSID, STATION_PASSWORD);
  WiFi.softAP(STATION_SSID);
// IPAddress myIP(192,168,1,1);
// IPAddress gatewayIP(0,0,0,0);
// IPAddress subnet(255,255,255,0);

  // WiFi.softAPConfig(myIP, gatewayIP, subnet);
  Serial.println(WiFi.softAPIP().toString());
  delay(100);
  dnsServer.start(53, "*", WiFi.softAPIP());

  // // respond to GET requests on URL /heap
  // server.on("/upload", HTTP_POST, [](AsyncWebServerRequest *request){
  //   request->send(200);
  //   Serial.println("WE HAVE RECEIVED A POST REQUEST!!!");
  // });

  server.on("/upload", HTTP_POST, [](AsyncWebServerRequest * request){}, NULL, [](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {
      Serial.println("WE HAVE RECEIVED A POST REQUEST!!!");
      for (size_t i = 0; i < len; i++) {
        Serial.write(data[i]);
      }
      Serial.println("POST REQUEST DATA IS ABOVE");
      request->redirect(splashPageFileName);
  });

  server.addHandler(new PatternHandler())       .setFilter(ON_AP_FILTER); //only when requested from AP
  server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER); //only when requested from AP
  server.onNotFound(handleNotFound);

  // start web server
  server.begin();
  writeSplashPageFile();
}

#endif