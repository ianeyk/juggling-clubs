//************************************************************
// this is a simple example that uses the painlessMesh library to
// connect to a another network and broadcast message from a webpage to the edges of the mesh network.
// This sketch can be extended further using all the abilities of the AsyncWebserver library (WS, events, ...)
// for more details
// https://gitlab.com/painlessMesh/painlessMesh/wikis/bridge-between-mesh-and-another-network
// for more details about my version
// https://gitlab.com/Assassynv__V/painlessMesh
// and for more details about the AsyncWebserver library
// https://github.com/me-no-dev/ESPAsyncWebServer
//************************************************************

#include "IPAddress.h"
#include "painlessMesh.h"
#include <FS.h>

#include "packet.h"

#ifdef ESP8266
#include "Hash.h"
#include <ESPAsyncTCP.h>
#else
#include <AsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>

#define   MESH_PREFIX     "Ian's Juggling Club"
#define   MESH_PASSWORD   "circusLuminescence"
#define   MESH_PORT       5555

#define   STATION_SSID     "mySSID"
#define   STATION_PASSWORD "myPASSWORD"

#define HOSTNAME "HTTP_BRIDGE"

Packet parseArgs(AsyncWebServerRequest *request);
// Prototype
void receivedCallback( const uint32_t &from, const String &msg );
IPAddress getlocalIP();

painlessMesh  mesh;
AsyncWebServer server(80);
IPAddress myIP(192,168,1,1);
IPAddress myAPIP(192,168,1,2);

// Replaces placeholder with LED state value
String processor(const String& var) {
  return "Return val from processor";
}

void setup() {
  Serial.begin(115200);

  // Initialize SPIFFS
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION );  // set before init() so that you can see startup messages

  // Channel set to 6. Make sure to use the same channel for your mesh and for you other
  // network (STATION_SSID)
  mesh.init( MESH_PREFIX, MESH_PASSWORD, MESH_PORT, WIFI_AP_STA, 6 );
  mesh.onReceive(&receivedCallback);

  mesh.stationManual(STATION_SSID, STATION_PASSWORD);
  mesh.setHostname(HOSTNAME);

  // Bridge node, should (in most cases) be a root node. See [the wiki](https://gitlab.com/painlessMesh/painlessMesh/wikis/Possible-challenges-in-mesh-formation) for some background
  mesh.setRoot(true);
  // This node and all other nodes should ideally know the mesh contains a root, so call this on all nodes
  mesh.setContainsRoot(true);

  myAPIP = IPAddress(mesh.getAPIP());
  Serial.println("My AP IP is " + myAPIP.toString());

  //Async webserver
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false, processor);

    // call the user-defined parsing function, below
    //
    Packet packet = parseArgs(request);
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

  // Route to set GPIO to HIGH
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("Received a message ON!!");
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  // Route to set GPIO to LOW
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("Received a message OFF!!ojeaf");
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  server.begin();

}

void loop() {
  mesh.update();
  if(myIP != getlocalIP()){
    myIP = getlocalIP();
    Serial.println("My IP is " + myIP.toString());
  }
}

void receivedCallback( const uint32_t &from, const String &msg ) {
  Serial.printf("bridge: Received from %u msg=%s\n", from, msg.c_str());
}

IPAddress getlocalIP() {
  return IPAddress(mesh.getStationIP());
}

Packet parseArgs(AsyncWebServerRequest *request) {

  Packet packet; // generate packet to send, with default values

  int params = request->params();
  for (int i = 0; i < params; i++) {
    AsyncWebParameter* p = request->getParam(i);
    String name = String(p->name());//.c_str();
    // Serial.print("name: ");
    // Serial.print(name);
    String value = String(p->value());//.c_str();
    // Serial.print(", value: ");
    // Serial.println(value);

    // initialize a Packet with default values


    // start interpreting

    if (name.startsWith("c")) {
      Serial.println("first letter was 'c'!");
      int idx = name.substring(1, 2).toInt();

      String temp_substring = value.substring(1);
      std::string temp_c_str = temp_substring.c_str();
      const char* temp_const_char = temp_c_str.c_str();
      long int color_int = strtol(temp_const_char, NULL, 16);

      packet.colors[idx] = color_int;

      Serial.print("Storing color as int: ");
      Serial.println(packet.colors[idx]);
    }

    else if (name.startsWith("s")) {
      Serial.println("first letter was 's'!");
      int idx = name.substring(1, 2).toInt();

      int speed_int = value.toInt();

      packet.speeds[idx] = speed_int;

      Serial.print("Storing speed as int: ");
      Serial.println(packet.speeds[idx]);
    }

    else if (name.startsWith("p")) {
      Serial.println("first letter was 'p'!");
      int idx = name.substring(1, 3).toInt(); // read two digits, not one

      // ignore the value

      packet.patterns[idx] = true;

      Serial.print("Setting pattern number ");
      Serial.print(idx);
      Serial.println(" to true");
    }

    else if (name.startsWith("a")) {
      Serial.println("first letter was 'a'!");
      int idx = name.substring(1, 3).toInt(); // read two digits, not one

      packet.addons[idx] = true;

      Serial.print("Setting addon number ");
      Serial.print(idx);
      Serial.println(" to true");
    }

  }

  return packet;

}

  // c0=%23000000&c1=%23000000&c2=%23000000&c3=%23000000&c4=%23000000&c5=%23000000&s0=1&s1=2&s2=2&p04=4&a03=3&a04=4