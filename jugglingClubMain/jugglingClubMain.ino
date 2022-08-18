#define FASTLED_ESP8266_RAW_PIN_ORDER

// comment the following line out if not the leader
//
// **************************** //
// #define LEADER true
// #define SHORTER_STRIPS true
// #define MY_UNIQUE_CLUB_ID 0
// **************************** //
// #define MY_UNIQUE_CLUB_ID 1
// **************************** //
// #define MY_UNIQUE_CLUB_ID 2
// **************************** //
//

#include "IPAddress.h"
#include <FS.h>

#ifdef ESP8266
#include "Hash.h"
#include <ESPAsyncTCP.h>
#else
#include <AsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>

// #include <TaskScheduler.h>
#include <FastLED.h>
#include "painlessMesh.h"
// #include "meshFuncs.ino"

#include "packet.h"

FASTLED_USING_NAMESPACE

// FastLED "100-lines-of-code" demo reel, showing just a few
// of the kinds of animation patterns you can quickly and easily
// compose using FastLED.
//
// This example also shows one easy way to define multiple
// animations patterns and have them automatically rotate.
//
// -Mark Kriegsman, December 2014

#define PI 3.1415926535897932384626433832795
#define HALF_PI 1.5707963267948966192313216916398
#define TWO_PI 6.283185307179586476925286766559

#define DATA_PIN    4
//#define CLK_PIN   0
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#ifdef SHORTER_STRIPS
#define NUM_LEDS 54
#else
#define NUM_LEDS    66
#endif

#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120

CRGB leds[NUM_LEDS];

#ifdef SHORTER_STRIPS
int strip_len = 9;
#else
int strip_len = 12;
#endif

int ring_len = 18;

void handle_wave(int h, int s, int v);
void start_wave();


unsigned long incrementPatternInterval = TASK_SECOND * 1; // 10 seconds default
unsigned long incrementHueInterval = TASK_MILLISECOND * 2; // 20 milliseconds default


void updateLeds();
Task taskUpdateLeds( TASK_MILLISECOND * int(1000 / FRAMES_PER_SECOND) , TASK_FOREVER, &updateLeds );

void incrementHue();
Task taskIncrementHue( incrementHueInterval , TASK_FOREVER, &incrementHue );

void incrementPattern();
Task taskIncrementPattern( incrementPatternInterval, TASK_FOREVER, &incrementPattern );

// set up PainlessMesh
// #define   MESH_PREFIX     "Apple"
// #define   MESH_PASSWORD   "circusLuminescence"
// #define   MESH_PORT       5555

// from web server code
#define   MESH_PREFIX     "Circus IT Department"
#define   MESH_PASSWORD   "circusLuminescence"
#define   MESH_PORT       5555

#ifdef LEADER
#define   STATION_SSID     "Ian's Juggling Clubs"
#define   STATION_PASSWORD "circusLuminescence"

#define HOSTNAME "HTTP_BRIDGE"

AsyncWebServer server(80);
IPAddress myIP(192,168,1,1);
IPAddress myAPIP(192,168,1,2);
// end web server code

// Replaces placeholder with LED state value
String processor(const String& var) {
  return "Return val from processor";
}
Packet parseArgs(AsyncWebServerRequest *request);
#endif

Scheduler userScheduler; // to control your personal task
painlessMesh mesh;
Packet packet; // Packet object that contains all information about patterns
int myUniqueOrderNumber;
const int nClubs = 3;
uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns


// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  Serial.println("receiving message");
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());

  #ifndef LEADER
  taskUpdateLeds.restart();
  taskIncrementHue.restart();
  taskIncrementPattern.restart();
  #endif

  // it is important that interpretMessage be called AFTER restarting the tasks (not before)
  packet = interpretMessage(msg);
  if (packet.addons[4]) {
    gHue = (gHue + int(256 / nClubs) * myUniqueOrderNumber) % 256;
  }
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

  #ifdef LEADER
  // Initialize SPIFFS
  //
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  #endif

  // set up FastLED
  //
  // tell FastLED about the LED strip configuration
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

  #ifdef LEADER
  // set up PainlessMesh
  //
  mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION );  // set before init() so that you can see startup messages
  //
  // Channel set to 6. Make sure to use the same channel for your mesh and for you other network (STATION_SSID)
  // init function and station / host setup from web server code
  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT); // , WIFI_AP_STA, 6 );
  IPAddress staticIP(192, 168, 0, 51);
  // mesh.stationManual(STATION_SSID, STATION_PASSWORD);
  // mesh.setHostname(HOSTNAME);
  mesh.setRoot(true); // Bridge node, should (in most cases) be a root node. See [the wiki](https://gitlab.com/painlessMesh/painlessMesh/wikis/Possible-challenges-in-mesh-formation) for some background
  mesh.setContainsRoot(true); // This node and all other nodes should ideally know the mesh contains a root, so call this on all nodes

  myAPIP = IPAddress(mesh.getAPIP());
  Serial.println("My AP IP is " + myAPIP.toString());
  //
  // Async webserver
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false, processor);

    // call the user-defined parsing function, below
    //
    packet = parseArgs(request);
    Serial.println("-----------------------");
    Serial.println("Received JSON message: ");
    Serial.println("-----------------------");
    String serialized = serializePacket(packet);
    Serial.println(serialized);

    sendMessage(packet);
  });
  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });
  // start web server
  server.begin();

  #else
    mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION );  // set before init() so that you can see startup messages
    //
    mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);
    mesh.setContainsRoot(true); // This node and all other nodes should ideally know the mesh contains a root, so call this on all nodes
  #endif

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

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
// SimplePatternList gPatterns = { solid, rainbowWithGlitter, confetti, sinelon, juggle, bpm, rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm, rainbowWithGlitter, confetti, juggle};
SimplePatternList gPatterns = { pulse, solid, solid, rainbowWithGlitter, confetti, juggle, bpm, solid, solid, solid, solid, solid, solid, solid, solid };
SimplePatternList gAddins = { ring_solid, sparkle, ring_solid, ring_solid, ring_solid, ring_solid, ring_solid };

void updateLeds() {
  // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();

  // Call all the addins in order
  for (int i = 0; i < N_ADDONS; i++) {
    if (packet.addons[i]) {
      gAddins[i]();
    }
  }

  // send the 'leds' array out to the actual LED strip
  FastLED.show();
}

void loop()
{
  mesh.update();
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

  // update own unique order id number based on list of other nodes
  getUniqueOrderNumber();

  #ifdef LEADER
  // every time we update the pattern, also send the message to update every other club's patterns
  Serial.println("sending message...");
  sendMessage(packet);
  Serial.println("Done sending message.");
  #endif

  // to fix the off by one error, we have to send the message BEFORE incrementing the pattern

  // add one to the current pattern number, and wrap around at the end
  for (int i = 0; i < N_PATTERNS + 1; i++) {
    // cycle through N_PATTERNS + 1, so if no patterns are selected, we land on the (N + 1)'th element
    gCurrentPatternNumber = (gCurrentPatternNumber + 1) % N_PATTERNS;
    if (packet.patterns[gCurrentPatternNumber]) {
      break;
      // if the pattern is not enabled, continue incrementing
    }
  }
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

  Serial.print("My current ID is ");
  // Serial.print(system_get_chip_id());
  int myCurrentId = mesh.getNodeList(true).front();
  Serial.print(myCurrentId);
  Serial.print("and my Node list is: ");
  for (int node : mesh.getNodeList(false))
    {
      Serial.print("Node ");
      Serial.print(node);
      Serial.print(", ");
    }
  Serial.println("!");

  gHue = (gHue + 1) % 256;
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