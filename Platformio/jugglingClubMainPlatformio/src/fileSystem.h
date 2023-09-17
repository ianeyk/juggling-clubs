#ifndef INCLUDE_FILE_SYSTEM_LIBRARY
#define INCLUDE_FILE_SYSTEM_LIBRARY

String fileContentsBefore = "<!DOCTYPE html><html><head><title>Ian's Juggling Club Home Page</title></head><body><p>To proceed, click <a href='http://";
String fileContentsMiddle = "/index.html'>here</a>.</p><form action=\"http://";
String fileContentsAfter = "/upload\" method=\"post\"><input type=\"submit\" name=\"{a:[0, 1, 2], b: [9, 8, 7]}\" value=\"Upload\" /></form></body></html>";

void setupFileSystem() {
  // Initialize SPIFFS
  if (!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // if (!SPIFFS.format()) {
  //   Serial.println("An Error has occurred while formatting SPIFFS");
  //   return;
  // }

    for (unsigned int i = 0; i < respondWithPage.size() ; i ++) {
      if (SPIFFS.exists(respondWithPage[i])) {
        Serial.print("File system: "); Serial.print(respondWithPage[i]); Serial.println(" exists.");
      } else {
        Serial.print("File system: "); Serial.print(respondWithPage[i]); Serial.print(" DOES NOT exist!!!.");
      }
    }

}

void writeSplashPageFile() {
  if (SPIFFS.exists(splashPageFileName)) {
    SPIFFS.remove(splashPageFileName);
  }

  File file = SPIFFS.open(splashPageFileName, "w");

  if (!file) {
      Serial.println("There was an error opening the file for writing");
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

#endif