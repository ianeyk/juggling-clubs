#include "IPAddress.h"
#include "painlessMesh.h"
// #include "meshFuncs.ino"

#include "packet.h"

#define   MESH_PREFIX     "Circus IT Department"
#define   MESH_PASSWORD   "circusLuminescence"
#define   MESH_PORT       5555

IPAddress myIP(192,168,1,1);
IPAddress myAPIP(192,168,1,2);

Packet parseArgs(AsyncWebServerRequest *request);

unsigned long incrementPatternInterval = TASK_SECOND * 1; // 10 seconds default
unsigned long incrementHueInterval = TASK_MILLISECOND * 2; // 20 milliseconds default
#define FRAMES_PER_SECOND  120

void updateLeds();
Task taskUpdateLeds( TASK_MILLISECOND * int(1000 / FRAMES_PER_SECOND) , TASK_FOREVER, &updateLeds );

void incrementHue();
Task taskIncrementHue( incrementHueInterval , TASK_FOREVER, &incrementHue );

void incrementPattern();
Task taskIncrementPattern( incrementPatternInterval, TASK_FOREVER, &incrementPattern );

Scheduler userScheduler; // to control your personal task
painlessMesh mesh;
Packet packet; // Packet object that contains all information about patterns
int myUniqueOrderNumber;

const int nClubs = 3;
uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns


// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {

  packet = interpretMessage(msg);

}
void newConnectionCallback(uint32_t nodeId) {
    // pass
}
void changedConnectionCallback() {
    // pass
}
void nodeTimeAdjustedCallback(int32_t offset) {
    // pass
}

void setup() {
  Serial.begin(115200);
  delay(1000); // 1 second delay for recovery

  // set up PainlessMesh
  //
//   mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION );  // set before init() so that you can see startup messages
//   mesh.setDebugMsgTypes();  // set before init() so that you can see startup messages
  //
  // Channel set to 6. Make sure to use the same channel for your mesh and for you other network (STATION_SSID)
  // init function and station / host setup from web server code
  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT); // , WIFI_AP_STA, 6 );
  IPAddress staticIP(192, 168, 0, 51);
  // mesh.stationManual(STATION_SSID, STATION_PASSWORD);
  // mesh.setHostname(HOSTNAME);
  mesh.setRoot(true); // Bridge node, should (in most cases) be a root node. See [the wiki](https://gitlab.com/painlessMesh/painlessMesh/wikis/Possible-challenges-in-mesh-formation) for some background
  mesh.setContainsRoot(true); // This node and all other nodes should ideally know the mesh contains a root, so call this on all nodes

  //
  // set up PainlessMesh networking callbacks (both codes)
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  //
  // start tasks controlling LEDs
  userScheduler.addTask( taskUpdateLeds );
  userScheduler.addTask( taskIncrementHue );
  userScheduler.addTask( taskIncrementPattern );
  taskUpdateLeds.enable();
  taskIncrementHue.enable();
  taskIncrementPattern.enable();
}

void loop()
{
  mesh.update();
}

String serialMessage = "";
void updateLeds() {
        //Check to see if anything is available in the serial receive buffer
    char nextChar;
    bool wasSerialAvailable = false;
    while (Serial.available() > 0) {
        //Read the next available byte in the serial receive buffer
        nextChar = Serial.read();
        serialMessage += nextChar;
        wasSerialAvailable = true;
    }
    if (wasSerialAvailable) {
        if (nextChar == '\n') {
            Serial.print("New message from Serial: ");
            Serial.println(serialMessage);
            // we know the message is over
            packet = deSerializePacket(serialMessage);
            sendMessage(packet);
            serialMessage = "";
        }
    }
}

int patternUnchangedCounter = 0;
void incrementPattern()
{

  if (patternUnchangedCounter < packet.speeds[0]) {
    patternUnchangedCounter++;
    return;
  } // else:
  patternUnchangedCounter = 0;
  // and proceed with the pattern change

  // add one to the current pattern number, and wrap around at the end
  for (int i = 0; i < N_PATTERNS + 1; i++) {
    // cycle through N_PATTERNS + 1, so if no patterns are selected, we land on the (N + 1)'th element
    gCurrentPatternNumber = (gCurrentPatternNumber + 1) % N_PATTERNS;
    if (packet.patterns[gCurrentPatternNumber]) {
      break;
      // if the pattern is not enabled, continue incrementing
    }
  }

  // every time we update the pattern, also send the message to update every other club's patterns
  sendMessage(packet);
}

int hueUnchangedCounter = 0;
void incrementHue() {

  if (packet.speeds[1] == 0) {
    return; // freeze the animation in place if speed == 0
  }

  if (hueUnchangedCounter < packet.speeds[1]) {
    hueUnchangedCounter++;
    return;
  } // else:
  hueUnchangedCounter = 0;
  // and proceed with the pattern change

  gHue = (gHue + 1) % 256;
}