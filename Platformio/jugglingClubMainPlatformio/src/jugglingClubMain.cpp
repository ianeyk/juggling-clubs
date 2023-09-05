#include <Arduino.h>
#include "painlessMesh.h"

// **************************** //
#define LEADER true
// #define MY_UNIQUE_CLUB_ID 0
#define MY_UNIQUE_CLUB_ID 1
// #define MY_UNIQUE_CLUB_ID 2

// #define INCLUDE_LEDS true
#define INCLUDE_WIFI true
#define INCLUDE_MESH true
// #define PRINT_DEBUG true
// **************************** //

#define PI 3.1415926535897932384626433832795
#define HALF_PI 1.5707963267948966192313216916398
#define TWO_PI 6.283185307179586476925286766559

#define FRAMES_PER_SECOND  60
#define N_CLUBS 3

Scheduler userScheduler; // to control your personal task

#ifdef INCLUDE_LEDS
  #include "ledControl.h"
#endif

#ifdef INCLUDE_WIFI
  #include "wifiServer.h"
#endif

#ifdef INCLUDE_MESH
  #include "meshFuncs.h"
#endif

int myUniqueOrderNumber;
void getUniqueOrderNumber();
void sendDebugMessage();

const unsigned long incrementCounterInterval = TASK_SECOND * 1; // 1 second default
const unsigned long incrementHueInterval = TASK_MILLISECOND * 100; // 100 milliseconds default
const unsigned long debugMessageInterval = TASK_MILLISECOND * 100; // 1 second

#ifdef INCLUDE_LEDS
  Task taskUpdateLeds( TASK_MILLISECOND * int(1000 / FRAMES_PER_SECOND) , TASK_FOREVER, &updateLeds );
  Task taskIncrementCounters( incrementCounterInterval, TASK_FOREVER, &incrementCounters );
#endif

#ifdef PRINT_DEBUG
  Task taskSendDebugMessage( debugMessageInterval, TASK_FOREVER, &sendDebugMessage );
#endif

void setup() {

  Serial.begin(115200);
  delay(1000); // 1 second delay for recovery
  Serial.println("Hello World!");

  #ifdef INCLUDE_MESH
    setupMesh();
  #endif

  #ifdef INCLUDE_WIFI
    setupWifiServer();
  #endif

  #ifdef INCLUDE_LEDS
    fastLedSetup();
    // start tasks controlling LEDs
    userScheduler.addTask(taskUpdateLeds);
    taskUpdateLeds.enable();
    userScheduler.addTask(taskIncrementCounters);
    taskIncrementCounters.enable();
  #endif

  #ifdef PRINT_DEBUG
    userScheduler.addTask(taskSendDebugMessage);
    taskSendDebugMessage.enable();
  #endif
}

void loop()
{
  #ifdef LEADER
    #ifdef INCLUDE_WIFI
      dnsServer.processNextRequest();
    #endif
  #endif

  #ifdef INCLUDE_MESH
    mesh.update();
  #endif
}

// void getUniqueOrderNumber() {
//   myUniqueOrderNumber = 0;
//   int myId = mesh.getNodeList(true).front(); // assuming the first element is its own; // system_get_chip_id();
//   for (int otherNodeId : mesh.getNodeList(false)) {
//       if (otherNodeId < myId) {
//         myUniqueOrderNumber++;
//       }
//     }
// }

void sendDebugMessage() {
  Serial.println("Spare Heap Remaining = " + String(ESP.getFreeHeap()));
}