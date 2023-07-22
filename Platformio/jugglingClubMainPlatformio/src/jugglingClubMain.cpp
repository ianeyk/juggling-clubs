#include <Arduino.h>

// **************************** //
#define LEADER true
// #define MY_UNIQUE_CLUB_ID 0
#define MY_UNIQUE_CLUB_ID 1
// #define MY_UNIQUE_CLUB_ID 2
// **************************** //

#include "painlessMesh.h"
IPAddress myIP(192,168,1,1);
IPAddress myAPIP(192,168,1,2);
IPAddress subnet(255,255,255,0);

#define PI 3.1415926535897932384626433832795
#define HALF_PI 1.5707963267948966192313216916398
#define TWO_PI 6.283185307179586476925286766559

#define FRAMES_PER_SECOND  40
#define N_CLUBS 3

void getUniqueOrderNumber();
#include "ledControl.h"
#include "wifiServer.h"

unsigned long incrementPatternInterval = TASK_SECOND * 10; // 10 seconds default
unsigned long incrementHueInterval = TASK_MILLISECOND * 20; // 20 milliseconds default

Task taskUpdateLeds( TASK_MILLISECOND * int(1000 / FRAMES_PER_SECOND) , TASK_FOREVER, &updateLeds );

void incrementPattern();
Task taskIncrementCounters( incrementPatternInterval, TASK_FOREVER, &incrementCounters );

// set up PainlessMesh
// #define   MESH_PREFIX     "Apple"
// #define   MESH_PASSWORD   "circusLuminescence"
// #define   MESH_PORT       5555

// from web server code
#define   MESH_PREFIX     "Circus IT Department"
#define   MESH_PASSWORD   "circusLuminescence"
#define   MESH_PORT       5555

Scheduler userScheduler; // to control your personal task
painlessMesh mesh;
// Packet packet; // Packet object that contains all information about patterns
int myUniqueOrderNumber;

// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  Serial.println("receiving message");
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());

  #ifndef LEADER
  taskUpdateLeds.restart();
  taskIncrementCounters.restart();
  #endif

  // it is important that interpretMessage be called AFTER restarting the tasks (not before)
  // packet = interpretMessage(msg);
  // if (packet.addons[4]) {
  //   gHue = (gHue + int(256 / nClubs) * myUniqueOrderNumber) % 256;
  // }
}
void newConnectionCallback(uint32_t nodeId) {
    Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}
void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
}
void nodeTimeAdjustedCallback(int32_t offset) {
    Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}

void setup() {
  Serial.begin(115200);
  delay(1000); // 1 second delay for recovery
  Serial.println("Hello World!");

  #ifdef LEADER
  // set up PainlessMesh
  //
  IPAddress staticIP(192, 168, 0, 51);
  mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION );  // set before init() so that you can see startup messages
  //
  // Channel set to 6. Make sure to use the same channel for your mesh and for you other network (STATION_SSID)
  // init function and station / host setup from web server code
  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT); // , WIFI_AP_STA, 6 );
  // mesh.stationManual(STATION_SSID, STATION_PASSWORD);
  // mesh.setHostname(HOSTNAME);
  mesh.setRoot(true); // Bridge node, should (in most cases) be a root node. See [the wiki](https://gitlab.com/painlessMesh/painlessMesh/wikis/Possible-challenges-in-mesh-formation) for some background
  mesh.setContainsRoot(true); // This node and all other nodes should ideally know the mesh contains a root, so call this on all nodes

  // myAPIP = IPAddress(mesh.getAPIP());
  // Serial.println("My AP IP is " + myAPIP.toString());
  setupWifiServer();

  #else
    mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION );  // set before init() so that you can see startup messages
    //
    mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);
    mesh.setContainsRoot(true); // This node and all other nodes should ideally know the mesh contains a root, so call this on all nodes
  #endif


  // set up PainlessMesh networking callbacks (both codes)
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  //
  // start tasks controlling LEDs
  userScheduler.addTask( taskUpdateLeds );
  userScheduler.addTask( taskIncrementCounters );
  taskUpdateLeds.enable();
  taskIncrementCounters.enable();
}

void loop()
{
  #ifdef LEADER
  dnsServer.processNextRequest();
  #endif
  mesh.update();
}

// IPAddress getlocalIP() {
//   return IPAddress(mesh.getStationIP());
// }

void getUniqueOrderNumber() {
  myUniqueOrderNumber = 0;
  int myId = mesh.getNodeList(true).front(); // assuming the first element is its own; // system_get_chip_id();
  for (int otherNodeId : mesh.getNodeList(false)) {
      if (otherNodeId < myId) {
        myUniqueOrderNumber++;
      }
    }
}