#ifndef INCLUDE_FILE_SYSTEM_LIBRARY
#define INCLUDE_FILE_SYSTEM_LIBRARY

String fileContentsBefore = "<!DOCTYPE html><html><head><title>Ian's Juggling Club Home Page</title></head><body><p>To proceed, click <a href='http://";
String fileContentsMiddle = "/index.html'>here</a>.</p><form action=\"http://";
String fileContentsAfter = "/upload\" method=\"post\"><input type=\"submit\" name=\"{a:[0, 1, 2], b: [9, 8, 7]}\" value=\"Upload\" /></form></body></html>";

void writeProgramsToMemory(const char*);
void readProgramsFromMemory();
String readFile(const char*);
void writeIfNotEqual(const char*, const char*);
void writeIpFile();
void writeSplashPageFile();

void setupFileSystem() {
    // Initialize LittleFS
    if (!LittleFS.begin()) {
        Serial.println("An Error has occurred while mounting LittleFS");
        return;
    }

#ifdef FORMAT_LITTLEFS
    // run the reformatting code once when you switch between SPIFFS and LittleFS
    if (!LittleFS.format()) {
        Serial.println("An Error has occurred while formatting LittleFS");
        return;
    }
#endif

#ifdef WRITE_FLASH
    writeIpFile();
    writeSplashPageFile();
#endif

    for (unsigned int i = 0; i < respondWithPage.size(); i++) {
        if (LittleFS.exists(respondWithPage[i])) {
            Serial.print("File system: ");
            Serial.print(respondWithPage[i]);
            Serial.println(" exists.");
        } else {
            Serial.print("File system: ");
            Serial.print(respondWithPage[i]);
            Serial.print(" DOES NOT exist!!!.");
        }
    }

    readProgramsFromMemory();
}

void writeIpFile() {
    writeIfNotEqual(ipStorageFileName, WiFi.softAPIP().toString().c_str());
}

void writeSplashPageFile() {
    String splashPageContents =
        fileContentsBefore +
        WiFi.softAPIP().toString() +
        fileContentsMiddle +
        WiFi.softAPIP().toString() +
        fileContentsAfter;
    writeIfNotEqual(splashPageFileName, splashPageContents.c_str());
}

void writeProgramsToMemory(const char* jsonString) {
    writeIfNotEqual(programStorageFileName, jsonString);
}

void readProgramsFromMemory() {
    String data = readFile(programStorageFileName);
    strcpy(incomingDataBuffer, data.c_str());
    readJsonDocument(incomingDataBuffer);
}

String readFile(const char* filename) {
    File file = LittleFS.open(filename, "r");

    if (!file) {
        Serial.println("readFile: Failed to open file " + String(filename));
        return "";
    }
    int len = file.size();
    String data = file.readString();
    Serial.println("Reading file from memory (" + String(len) + "bytes): " + data);

    file.close();
    return data;
}

void writeIfNotEqual(const char* filename, const char* stringToWrite) {
    if (LittleFS.exists(filename)) {
        if (readFile(filename).equals(stringToWrite)) {
            Serial.println("writeIfNotEqual: file matches contents of" + String(filename));
            return;
        }
        LittleFS.remove(filename);
    }

    File file = LittleFS.open(filename, "w");

    if (!file) {
        Serial.println("writeIfNotEqual: There was an error opening the file " + String(filename));
        return;
    }

    if (file.print(stringToWrite)) {
        Serial.println("File was written");
    } else {
        Serial.println("File write failed");
    }

    file.close();
}

#endif
