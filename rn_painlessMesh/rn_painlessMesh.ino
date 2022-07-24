/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-mesh-esp32-esp8266-painlessmesh/

  This is a simple example that uses the painlessMesh library: https://github.com/gmag11/painlessMesh/blob/master/examples/basic/basic.ino
*/

#include "painlessMesh.h"

#define   MESH_PREFIX     "Apple"
#define   MESH_PASSWORD   "circusLuminescence"
#define   MESH_PORT       5555

Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;

String state = "no message";

// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain
void toggleLED();
void acquireSerial();

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );
// Task taskToggleLED( TASK_SECOND * 1 , TASK_FOREVER, &toggleLED );
Task taskAcquireSerial( TASK_MILLISECOND * 100 , TASK_FOREVER, &acquireSerial );

bool LEDstate = 0;
void toggleLED() {
  LEDstate = 1 - LEDstate;
  digitalWrite(16, LEDstate);
  digitalWrite(2, LEDstate);
}

void sendMessage() {
  String msg = state;
  state = "no message";
  msg += mesh.getNodeId();
  mesh.sendBroadcast( msg );
  // taskSendMessage.setInterval( random( TASK_SECOND * 1, TASK_SECOND * 5 ));
//  Serial.println("_-_-_-_-_-_-_-_-_-_ Sending Message _-_-_-_-_-_-_-_-_-_");
}

// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
  interpretMessage(msg);
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

int led1 = 16;
int led2 = 2;
void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("hellow world!");

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  digitalWrite(led1, HIGH);
  digitalWrite(led2, HIGH);

  //  mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  userScheduler.addTask( taskSendMessage );
  userScheduler.addTask( taskAcquireSerial );
  taskSendMessage.enable();
  taskAcquireSerial.enable();
}

void loop() {
  // it will run the user scheduler as well
  mesh.update();
}

//////////////////////////////    Serial interpretation /////////

String readSerial() {
  String msg = "";
  while (Serial.available() > 0) {
    char nextChar = char(Serial.read());
    if (nextChar == '\n') {
      break;
    }
    msg = msg + nextChar;
  }

  Serial.print("Received: \"");
  Serial.print(msg);
  Serial.println("\"");

  return msg;
}

bool verifyMessage(String msg) {
  if (msg.startsWith("on") || msg.startsWith("off")) {
    return true;
  }
  return false;
}

void interpretMessage(String msg) {
  if (!verifyMessage(msg)) {
    return;
  }
  if (msg.startsWith("on")) {
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
  }
  if (msg.startsWith("off")) {
    digitalWrite(led1, HIGH);
    digitalWrite(led2, HIGH);
  }
}

void acquireSerial() {
  String msg = readSerial();
  interpretMessage(msg);
  if (verifyMessage(msg)) {
    state = msg;
  }
}