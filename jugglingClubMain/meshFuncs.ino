#include "StringSplitter.h"

String msg_to_send = "no message";
String startChar = "msg";

const int led1 = 16;
const int led2 = 2;

void sendMessage() {
  String msg = startChar + "," + String(gCurrentPatternNumber) + "," + String(gHue) + ",";
  msg += mesh.getNodeId();
  mesh.sendBroadcast( msg );

  msg_to_send = "no message"; // reset the message
}

bool verifyMessage(String msg) {
    return msg.startsWith(startChar);
}

void interpretMessage(String msg) {
  if (!verifyMessage(msg)) {
    return;
  }
  StringSplitter *splitter = new StringSplitter(msg, ',', 3);
  int itemCount = splitter->getItemCount();

  for(int i = 0; i < itemCount; i++){
    String item = splitter->getItemAtIndex(i);
  }

  gCurrentPatternNumber = (splitter->getItemAtIndex(1)).toInt();
  gHue = (splitter->getItemAtIndex(2)).toInt();
}