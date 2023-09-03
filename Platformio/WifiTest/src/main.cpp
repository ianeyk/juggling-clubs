#include <DNSServer.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include "ESPAsyncWebServer.h"
#include <FS.h>
#include <LittleFS.h>

DNSServer dnsServer;
AsyncWebServer server(80);

std::vector<String> respondWithPage{"/index.html", "/style.css"};

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
    //request->addInterestingHeader("ANY");
    return true;
  }


  void handleRequest(AsyncWebServerRequest *request) {
    Serial.print("The initial request was: ");
    Serial.println(request->url());
    Serial.println("Sending redirect now!");

    if (request->url() == "/style.css") {
      AsyncWebServerResponse *response;
      response = request->beginResponse(LittleFS, "/style.css");
      request->send(response);
    }
    else if (request->url() == "/index.html") {
      Serial.print("received request for /index.html");
      AsyncWebServerResponse *response;
      response = request->beginResponse(LittleFS, "/index.html");
      request->send(response);
    }
    else {
      Serial.print("received arbitrary request");
      AsyncWebServerResponse *response;

      Serial.print("trying to read LittleFS");
      response = request->beginResponse(LittleFS, "/index.html");
      Serial.print("successfully read LittleFS");
      request->send(response);
      // request->redirect("/index.html");
      // Serial.print("redirecting to /index.html");
    }
  }

  // void handleRequest(AsyncWebServerRequest *request) {
  //   AsyncResponseStream *response = request->beginResponseStream("text/html");
  //   response->print("<!DOCTYPE html><html><head><title>Captive Portal</title></head><body>");
  //   response->print("<p>This is out captive portal front page.</p>");
  //   response->printf("<p>You were trying to reach: http://%s%s</p>", request->host().c_str(), request->url().c_str());
  //   response->printf("<p>Try opening <a href='http://%s'>this link</a> instead</p>", WiFi.softAPIP().toString().c_str());
  //   response->print("</body></html>");
  //   request->send(response);
  // }
};

void littleFsListDir(const char * dirname) {
  Serial.printf("Listing directory: %s\n", dirname);

  Dir root = LittleFS.openDir(dirname);

  while (root.next()) {
        File file = root.openFile("r");
        Serial.print(" FILE: ");
        Serial.print(root.fileName());
        Serial.print(" SIZE: ");
        Serial.println(file.size());
        file.close();
        }
Serial.println("");
}

void setup(){
    Serial.begin(112500);
    delay(500);

  // Initialize LittleFS
  if (!LittleFS.begin()) {
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }

  if (!LittleFS.format()) {
    Serial.println("An Error has occurred while formatting LittleFS");
    return;
  }

  if (LittleFS.exists("/index.html")) {
    Serial.println("File system: /index.html exists.");
  } else {
    Serial.println("File system: /index.html DOES NOT exist!!!.");
  }

  if (LittleFS.exists("/style.css")) {
    Serial.println("File system: /style.css exists.");
  } else {
    Serial.println("File system: /style.css DOES NOT exist!!!.");
  }

  Serial.println("list empty dir");
  littleFsListDir("");
  Serial.println("list root dir");
  littleFsListDir("/");
  Serial.println("list data dir");
  littleFsListDir("/data");

    // To format all space in LittleFS
    // LittleFS.format()

    // Get all information of your LittleFS
    FSInfo fs_info;
    LittleFS.info(fs_info);

    Serial.println("File sistem info.");

    Serial.print("Total space:      ");
    Serial.print(fs_info.totalBytes);
    Serial.println("byte");

    Serial.print("Total space used: ");
    Serial.print(fs_info.usedBytes);
    Serial.println("byte");

    Serial.print("Block size:       ");
    Serial.print(fs_info.blockSize);
    Serial.println("byte");

    Serial.print("Page size:        ");
    Serial.print(fs_info.totalBytes);
    Serial.println("byte");

    Serial.print("Max open files:   ");
    Serial.println(fs_info.maxOpenFiles);

    Serial.print("Max path length:  ");
    Serial.println(fs_info.maxPathLength);

    Serial.println();
  //your other setup stuff...
  WiFi.softAP("esp-captive");
  dnsServer.start(53, "*", WiFi.softAPIP());
  server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);//only when requested from AP
  //more handlers...
  server.begin();
}

void loop(){
  dnsServer.processNextRequest();
}