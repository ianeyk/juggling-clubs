int led1 = 16;
int led2 = 2;

void setup() {
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);

  digitalWrite(led1, HIGH);
  digitalWrite(led2, HIGH);

  Serial.begin(115200);
}

String msg;
void loop() {

  msg = readSerial();
  interpretMessage(msg);
  delay(200);
}

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
  if (msg == "on" || msg == "off") {
    return true;
  }
  return false;
}

void interpretMessage(String msg) {
  if (!verifyMessage(msg)) {
    return;
  }
  if (msg == "on") {
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
  }
  if (msg == "off") {
    digitalWrite(led1, HIGH);
    digitalWrite(led2, HIGH);
  }
}