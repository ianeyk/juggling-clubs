#include <Arduino.h>
#include "painlessMesh.h"

// **************************** //
#define LEADER true
// #define MY_UNIQUE_CLUB_ID 0
#define MY_UNIQUE_CLUB_ID 1
// #define MY_UNIQUE_CLUB_ID 2

#define INCLUDE_LEDS true
#define INCLUDE_WIFI true
#define INCLUDE_MESH true
#define PRINT_DEBUG true
#define WRITE_FLASH true
// **************************** //

#define FRAMES_PER_SECOND  60
#define N_CLUBS 3

Scheduler userScheduler; // to control your personal task

#ifdef INCLUDE_WIFI
  #include "wifiServer.h"
#endif

#ifdef INCLUDE_MESH
  #include "meshFuncs.h"
#endif

#ifdef INCLUDE_LEDS
  #include "ledControl.h"
#endif

int myUniqueOrderNumber;
void getUniqueOrderNumber();
void sendDebugMessage();
void broadcastJson();

const unsigned long debugMessageInterval = 5 * TASK_SECOND;

#ifdef INCLUDE_LEDS
  Task taskUpdateLeds( TASK_MILLISECOND * int(1000 / FRAMES_PER_SECOND) , TASK_FOREVER, &updateLeds );
#endif

#ifdef INCLUDE_MESH
  #ifdef LEADER
    Task taskBroadcastJson( TASK_SECOND * 1 , TASK_FOREVER, &broadcastJson );
  #endif
#endif

#ifdef PRINT_DEBUG
  Task taskSendDebugMessage( debugMessageInterval, TASK_FOREVER, &sendDebugMessage );
#endif

void setup() {

  Serial.begin(115200);
  delay(1000); // 1 second delay for recovery
  Serial.println("Hello World!");
  pinMode(LED_BUILTIN, OUTPUT);

  #ifdef INCLUDE_MESH
    setupMesh();
    #ifdef LEADER
      userScheduler.addTask(taskBroadcastJson);
      taskBroadcastJson.enable();
    #endif
  #endif

  #ifdef INCLUDE_WIFI
    setupWifiServer();
  #endif

  #ifdef INCLUDE_LEDS
    fastLedSetup();
    // start tasks controlling LEDs
    userScheduler.addTask(taskUpdateLeds);
    taskUpdateLeds.enable();
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


int get_club_id() {
    // Return 0, 1, or 2
    return MY_UNIQUE_CLUB_ID;
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
  Serial.println("My IP Address = " + WiFi.softAPIP().toString() + " and Spare Heap Remaining = " + String(ESP.getFreeHeap()));
}