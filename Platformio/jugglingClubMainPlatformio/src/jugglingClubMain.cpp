#include <Arduino.h>
#include "painlessMesh.h"

// **************************** //
#define LEADER true
// #define MY_UNIQUE_CLUB_ID 0
#define MY_UNIQUE_CLUB_ID 1
// #define MY_UNIQUE_CLUB_ID 2
// **************************** //

#define PI 3.1415926535897932384626433832795
#define HALF_PI 1.5707963267948966192313216916398
#define TWO_PI 6.283185307179586476925286766559

#define FRAMES_PER_SECOND  10
#define N_CLUBS 3

Scheduler userScheduler; // to control your personal task
// #include "ledControl.h"
#include "wifiServer.h"
// #include "meshFuncs.h"

int myUniqueOrderNumber;
void getUniqueOrderNumber();

// unsigned long incrementCounterInterval = TASK_SECOND * 1; // 1 second default
// unsigned long incrementHueInterval = TASK_MILLISECOND * 100; // 100 milliseconds default

// Task taskUpdateLeds( TASK_MILLISECOND * int(1000 / FRAMES_PER_SECOND) , TASK_FOREVER, &updateLeds );
// Task taskIncrementCounters( incrementCounterInterval, TASK_FOREVER, &incrementCounters );

void setup() {
  Serial.begin(115200);
  delay(1000); // 1 second delay for recovery
  Serial.println("Hello World!");

  // setupMesh();
  setupWifiServer();
  // fastLedSetup();

  // // start tasks controlling LEDs
  // userScheduler.addTask(taskUpdateLeds);
  // taskUpdateLeds.enable();
  // userScheduler.addTask(taskIncrementCounters);
  // taskIncrementCounters.enable();
}

void loop()
{
  #ifdef LEADER
  dnsServer.processNextRequest();
  #endif
  // mesh.update();
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