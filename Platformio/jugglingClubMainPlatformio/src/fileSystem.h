#ifndef INCLUDE_FILE_SYSTEM_LIBRARY
#define INCLUDE_FILE_SYSTEM_LIBRARY

String fileContentsBefore = "<!DOCTYPE html><html><head><title>Ian's Juggling Club Home Page</title></head><body><p>To proceed, click <a href='http://";
String fileContentsMiddle = "/index.html'>here</a>.</p><form action=\"http://";
String fileContentsAfter = "/upload\" method=\"post\"><input type=\"submit\" name=\"{a:[0, 1, 2], b: [9, 8, 7]}\" value=\"Upload\" /></form></body></html>";

void writeProgramsToMemory(const char *);
void readProgramsFromMemory();

void setupFileSystem() {
  // Initialize LittleFS
  if (!LittleFS.begin()) {
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }

  // // run the reformatting code once when you switch between SPIFFS and LittleFS
  // if (!LittleFS.format()) {
  //   Serial.println("An Error has occurred while formatting LittleFS");
  //   return;
  // }

  for (unsigned int i = 0; i < respondWithPage.size() ; i ++) {
    if (LittleFS.exists(respondWithPage[i])) {
      Serial.print("File system: "); Serial.print(respondWithPage[i]); Serial.println(" exists.");
    } else {
      Serial.print("File system: "); Serial.print(respondWithPage[i]); Serial.print(" DOES NOT exist!!!.");
    }
  }

  readProgramsFromMemory();
}

void writeSplashPageFile() {
  if (LittleFS.exists(splashPageFileName)) {
    LittleFS.remove(splashPageFileName);
  }

  File file = LittleFS.open(splashPageFileName, "w");

  if (!file) {
      Serial.println("writeSplashPageFile: There was an error opening the file for writing");
      return;
  }

  if (file.print("TEST")) {
      Serial.println("File was written");
  } else {
      Serial.println("File write failed");
  }

    file.print(fileContentsBefore);
    file.print(String(WiFi.softAPIP().toString().c_str()));
    file.print(fileContentsMiddle);
    file.print(String(WiFi.softAPIP().toString().c_str()));
    file.print(fileContentsAfter);

  file.close();
}


void writeProgramsToMemory(const char *jsonString) {
  if (LittleFS.exists(programStorageFileName)) {
    LittleFS.remove(programStorageFileName);
  }

  File file = LittleFS.open(programStorageFileName, "w");

  if (!file) {
      Serial.println("writeProgramsToMemory: There was an error opening the file for writing");
      return;
  }

  if (file.print(jsonString)) {
      Serial.println("File was written");
  } else {
      Serial.println("File write failed");
  }

  file.close();
}

void readProgramsFromMemory() {
  File file = LittleFS.open(programStorageFileName, "r");

  if (!file) {
      Serial.println("Failed to open file for reading");
      return;
  }
  int len = file.size();
  String data = file.readString();
  Serial.println("Reading file from memory (" + String(len) + "bytes): " + data);

  file.close();
  strcpy(incomingDataBuffer, data.c_str());
  readJsonDocument(incomingDataBuffer);
}

#endif