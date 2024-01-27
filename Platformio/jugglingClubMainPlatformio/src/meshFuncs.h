#ifndef MESH_FUNCS_LIBRARY
#define MESH_FUNCS_LIBRARY

#include "painlessMesh.h"
#include "jsonPacket.h"

// from web server code
#define MESH_PREFIX "Circus IT Department"
#define MESH_PASSWORD "circusLuminescence"
#define MESH_PORT 5555
painlessMesh mesh;
IPAddress myAPIP(192, 168, 1, 2);

// Needed for painless library
void receivedCallback(uint32_t from, String &msg)
{
  Serial.println("receiving message");
  const char *m = msg.c_str();
  Serial.printf("startHere: Received from %u msg=%s\n", from, m);
  if (strlen(m) < MAX_MESSAGE_SIZE)
  {
    strcpy(incomingDataBuffer, m);
    readJsonDocument(incomingDataBuffer);
  }
  else
  {
    Serial.println("Received program that was too long. Max length is " + String(MAX_MESSAGE_SIZE) + " and received length " + String(msg.length()));
  }
  // readJsonDocument(m, strlen(m));
}

void newConnectionCallback(uint32_t nodeId)
{
  Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
  mesh.sendSingle(nodeId, incomingDataBuffer);
}

void setupMesh()
{
#ifdef LEADER
  // set up PainlessMesh
  //
  IPAddress staticIP(192, 168, 0, 51);
  mesh.setDebugMsgTypes(ERROR | STARTUP | CONNECTION); // set before init() so that you can see startup messages
  //
  // Channel set to 6. Make sure to use the same channel for your mesh and for you other network (STATION_SSID)
  // init function and station / host setup from web server code
  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT); // , WIFI_AP_STA, 6 );
  // mesh.stationManual(STATION_SSID, STATION_PASSWORD);
  // mesh.setHostname(HOSTNAME);
  // mesh.setRoot(true); // Bridge node, should (in most cases) be a root node. See [the wiki](https://gitlab.com/painlessMesh/painlessMesh/wikis/Possible-challenges-in-mesh-formation) for some background
  // mesh.setContainsRoot(true); // This node and all other nodes should ideally know the mesh contains a root, so call this on all nodes

  // myAPIP = IPAddress(mesh.getAPIP());
  // Serial.println("My AP IP is " + myAPIP.toString());

#else
  mesh.setDebugMsgTypes(ERROR | STARTUP | CONNECTION); // set before init() so that you can see startup messages
  //
  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);
  // mesh.setContainsRoot(true); // This node and all other nodes should ideally know the mesh contains a root, so call this on all nodes
#endif

  // set up PainlessMesh networking callbacks (both codes)
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
}

void sendMessage(const char *msg)
{
  Serial.println("Broadcasting message over Painlessmesh.");
  mesh.sendBroadcast(msg);
}

void blinkDebugLed()
{
  unsigned long nodeTime = mesh.getNodeTime();
  // Serial.println("nodeTime is " + String(nodeTime));
  if ((nodeTime / 1000 / 1000 / 3) % 2)
  {
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else
  {
    digitalWrite(LED_BUILTIN, LOW);
  }
}

#endif
