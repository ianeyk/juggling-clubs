#define FASTLED_ESP8266_RAW_PIN_ORDER

// comment the following line out if not the leader
//
// **************************** //
#define LEADER true
#define SHORTER_STRIPS true
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


unsigned long incrementPatternInterval = TASK_SECOND * 10;


void updateLeds();
Task taskUpdateLeds( TASK_MILLISECOND * int(1000 / FRAMES_PER_SECOND) , TASK_FOREVER, &updateLeds );

void incrementHue();
Task taskIncrementHue( TASK_MILLISECOND * 20 , TASK_FOREVER, &incrementHue );

void incrementPattern();
Task taskIncrementPattern( incrementPatternInterval, TASK_FOREVER, &incrementPattern );

// set up PainlessMesh
// #define   MESH_PREFIX     "Apple"
// #define   MESH_PASSWORD   "circusLuminescence"
// #define   MESH_PORT       5555

// from web server code
#define   MESH_PREFIX     "Apple"
#define   MESH_PASSWORD   "circusLuminescence"
#define   MESH_PORT       5555

#ifdef LEADER
#define   STATION_SSID     "Apple"
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
Packet instructionPacket; // Packet object that contains all information about patterns

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
  instructionPacket = interpretMessage(msg);
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
    Packet packet = parseArgs(request);
    Serial.println("-----------------------");
    Serial.println("Received JSON message: ");
    instructionPacket = packet;
    Serial.println("-----------------------");
    String serialized = serializePacket(packet);
    Serial.println(serialized);

    sendMessage(instructionPacket);
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
SimplePatternList gPatterns = { solid, sparkle, solid, solid, solid, solid, solid, solid, solid, solid, solid, solid, solid, solid, solid };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

// void start_wave() {
//   runner.add_task(task_handle_wave);
// }

void updateLeds() {
  // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();
}

void loop()
{
  mesh.update();
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void incrementPattern()
{

  #ifdef LEADER
  // every time we update the pattern, also send the message to update every other club's patterns
  Serial.println("sending message...");
  sendMessage(instructionPacket);
  Serial.println("Done sending message.");
  #endif

  // to fix the off by one error, we have to send the message BEFORE incrementing the pattern

  // add one to the current pattern number, and wrap around at the end
  for (int i = 0; i < N_PATTERNS + 1; i++) {
    gCurrentPatternNumber = (gCurrentPatternNumber + 1) % N_PATTERNS;
    if (instructionPacket.patterns[gCurrentPatternNumber]) {
      break;
      // if the pattern is not enabled, continue incrementing
    }
  }
}

void incrementHue() {
  Serial.print("My Node list: ");
  for (int node : mesh.getNodeList())
    {
      Serial.print("Node ");
      Serial.print(node);
      Serial.print(", ");
    }
  Serial.println("!");

  gHue = (gHue + 1) % 255;
}

void solid() {
  fill_solid( leds, NUM_LEDS, instructionPacket.colors[0]);
}

void sparkle() {
  fill_solid( leds, NUM_LEDS, instructionPacket.colors[1]);
  addGlitter(80);
}

void rainbow()
{
  // FastLED's built-in rainbow generator

  fill_rainbow( leds, NUM_LEDS, int(gHue / 50) * 50, 7);
}

void rainbowWithGlitter()
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter)
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti()
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS-1 );
  leds[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  uint8_t dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

int handle_wave_pos = 0;
void handle_wave(int h, int s, int v) {
  set_handle(handle_wave_pos, 0, 0, 0); // set the previous LED to off
  handle_wave_pos = (handle_wave_pos + 1) % strip_len;
  set_handle(handle_wave_pos, h, s, v); // set the next LED to on
}

float better_handle_wave_pos = 0;
int handle_wave_val(int x) {
  // return floor(pow(2, -abs(better_handle_wave_pos - float(x))) * 255);
  float d = better_handle_wave_pos - float(x) + 0.005;
  // return floor(((sin(d) / d) + 0.25) * 1 / 1.25 * 255);
  return floor(max(cos(d), double(0)) * 255);
}

void better_handle_wave(int h, int s, int v) {
  for (int i = 0; i < strip_len; i++) {
    if (abs(better_handle_wave_pos - i) < 2) {
      set_handle(i, h, s, handle_wave_val(i));
    }
    else {
      set_handle(i, h, s, 0);
    }
  }
  better_handle_wave_pos += 0.05;
  if (better_handle_wave_pos >= strip_len + 2) {
    better_handle_wave_pos = -2;
  }
}

void set_handle(int height, int h, int s, int v) {
  leds[height] = CHSV(h, s, v);
  leds[2 * strip_len - 1 - height] = CHSV(h, s, v);
  leds[2 * strip_len - 1 + height] = CHSV(h, s, v);
  leds[4 * strip_len - 1 - height] = CHSV(h, s, v);
}

void set_ring(int h, int s, int v) {
  for (int i = 0; i < ring_len; i++) {
    leds[4 * strip_len + i] = CHSV(h, s, v);
  }
}

float longit_wave_pos = 0;
int longit_wave_val(float angle) {
  float d = fmod(longit_wave_pos - angle, TWO_PI);
  if ((0 < d && d < HALF_PI) || 3 * HALF_PI < d && d < TWO_PI) {
    return floor(cos(d) * 255);
  } //else
  return 0;
}

void longitudinal_wave(int h, int s, int v) {
  // set the vertical strips
  for (int handle_strip = 0; handle_strip < 4; handle_strip++) {
    for (int i = 0; i < strip_len; i++) {
      leds[handle_strip * strip_len + i] = CHSV(h, s, longit_wave_val(handle_strip * HALF_PI));
    }
  }
  // set the ring
  for (int i = 0; i < ring_len; i++) {
    leds[4 * strip_len + i] = CHSV(h, s, longit_wave_val(i * TWO_PI / ring_len));
  }
  longit_wave_pos += 0.08;
  if (longit_wave_pos > TWO_PI) {
    longit_wave_pos = 0;
  }
}

IPAddress getlocalIP() {
  return IPAddress(mesh.getStationIP());
}