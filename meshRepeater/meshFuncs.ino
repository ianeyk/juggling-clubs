#include "StringSplitter.h"
#include "packet.h"

void sendMessage(Packet packet) {
  packet.currentPattern = gCurrentPatternNumber;
  packet.currentHue = gHue;
  packet.currentOffset = 0;
  String msg = serializePacket(packet);
  mesh.sendBroadcast( msg );
}

Packet interpretMessage(String msg) {

  Packet packet = deSerializePacket(msg);

  gCurrentPatternNumber = packet.currentPattern;
  gHue = packet.currentHue;

  return packet;
}