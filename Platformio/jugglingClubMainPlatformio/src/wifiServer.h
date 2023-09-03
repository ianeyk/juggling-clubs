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

std::vector<String> respondWithPage{"/connecttest.txt", "/index.html", "/style.css"};

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
    // check if url matches a string in array doNotCaptivePortal
    // return inStringArray(respondWithPage, request->url());
    // return !(inStringArray(doNotCaptivePortal, request->url()));
    return true; // make sue CaptiveRequestHandler is enabled after canHandle
  }

  void handleRequest(AsyncWebServerRequest *request) {
    Serial.print("The initial request was: ");
    Serial.println(request->url());

    if (request->url() == "/style.css") {
      AsyncWebServerResponse *response;
      Serial.println("received request for /style.css");
      response = request->beginResponse(SPIFFS, "/style.css");
      // response->addHeader("Content-Encoding", "gzip");
      request->send(response);
    }
    else if (request->url() == "/css") {
      Serial.println("received request for /css");
      AsyncWebServerResponse *response;
      response = request->beginResponse(SPIFFS, "/css");
      // response->addHeader("Content-Encoding", "gzip");
      request->send(response);
    }
    else if (request->url() == "/index.js.download") {
      Serial.println("received request for /index.js.download");
      AsyncWebServerResponse *response;
      response = request->beginResponse(SPIFFS, "/index.js.download");
      // response->addHeader("Content-Encoding", "gzip");
      request->send(response);
    }
    else if (request->url() == "/index.html") {
      Serial.println("received request for /index.html");
      AsyncWebServerResponse *response;
      response = request->beginResponse(SPIFFS, "/index.html");
      // response->addHeader("Content-Encoding", "gzip");
      request->send(response);
    }
    else {
      Serial.print("received arbitrary request");
      AsyncResponseStream *response = request->beginResponseStream("text/html");
      response->print("<!DOCTYPE html><html><head><title>Ian's Juggling Club Home Page</title></head><body>");
      response->printf("<p>To proceed, click <a href='http://%s/index.html'>here</a>.</p>", WiFi.softAPIP().toString().c_str());
      response->print("</body></html>");
      request->send(response);


      // Serial.print("trying to read SPIFFS");
      // response = request->beginResponse(SPIFFS, "/index.html");
      // Serial.print("successfully read SPIFFS");
      // request->send(response);
      // request->redirect("/index.html");
      // Serial.print("redirecting to /index.html");
    }
  }
    // AsyncResponseStream *response = request->beginResponseStream("text/html");
    // response->print("<!DOCTYPE html><html><head><title>Captive Portal</title></head><body>");
    // response->print("<p>This is out captive portal front page.</p>");
    // response->printf("<p>You were trying to reach: http://%s%s</p>", request->host().c_str(), request->url().c_str());
    // response->printf("<p>Try opening <a href='http://%s/index.html'>this link</a> instead</p>", WiFi.softAPIP().toString().c_str());
    // response->print("</body></html>");
    // request->send(response);
  // }
};

// void SPIFFSListDir(const char * dirname) {
//   Serial.printf("Listing directory: %s\n", dirname);

//   Dir root = SPIFFS.openDir(dirname);

//   while (root.next()) {
//         File file = root.openFile("r");
//         Serial.print(" FILE: ");
//         Serial.print(root.fileName());
//         Serial.print(" SIZE: ");
//         Serial.println(file.size());
//         file.close();
//         }
// Serial.println("");
// }

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

  if (SPIFFS.exists("/index.html")) {
    Serial.println("File system: /index.html exists.");
  } else {
    Serial.println("File system: /index.html DOES NOT exist!!!.");
  }

  if (SPIFFS.exists("/style.css")) {
    Serial.println("File system: /style.css exists.");
  } else {
    Serial.println("File system: /style.css DOES NOT exist!!!.");
  }

    if (SPIFFS.exists("/css")) {
    Serial.println("File system: /css exists.");
  } else {
    Serial.println("File system: /css DOES NOT exist!!!.");
  }

    if (SPIFFS.exists("/index.js.download")) {
    Serial.println("File system: /index.js.download exists.");
  } else {
    Serial.println("File system: /index.js.download DOES NOT exist!!!.");
  }

  // Serial.println("list empty dir");
  // SPIFFSListDir("");
  // Serial.println("list root dir");
  // SPIFFSListDir("/");
  // Serial.println("list data dir");
  // SPIFFSListDir("/data");

//  // Get all information of your SPIFFS
//   FSInfo fs_info;
//   SPIFFS.info(fs_info);

//   Serial.println("File sistem info.");

//   Serial.print("Total space:      ");
//   Serial.print(fs_info.totalBytes);
//   Serial.println("byte");

//   Serial.print("Total space used: ");
//   Serial.print(fs_info.usedBytes);
//   Serial.println("byte");

//   Serial.print("Block size:       ");
//   Serial.print(fs_info.blockSize);
//   Serial.println("byte");

//   Serial.print("Page size:        ");
//   Serial.print(fs_info.totalBytes);
//   Serial.println("byte");

//   Serial.print("Max open files:   ");
//   Serial.println(fs_info.maxOpenFiles);

//   Serial.print("Max path length:  ");
//   Serial.println(fs_info.maxPathLength);

//   Serial.println();

  // Async webserver
  // WiFi.softAP(STATION_SSID, STATION_PASSWORD);
  // WiFi.softAPConfig(myIP, myAPIP, subnet);
  WiFi.softAP(STATION_SSID);
  delay(100);
  dnsServer.start(53, "*", WiFi.softAPIP());
  // server.addHandler(new ServerRequestHandler()).setFilter(ON_AP_FILTER); //only when requested from AP
  server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER); //only when requested from AP
  // start web server
  server.begin();
}

#endif