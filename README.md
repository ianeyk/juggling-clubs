# juggling-clubs
Arduino code for LED juggling clubs, including LED patterns and synchronization between multiple ESP8266 modules


There are three files in this repo that are important: 

#### 1) jugglingClubMain.ino

Upload this to the three clubs. Don't bother modifying the `#define LEADER` lines (or enabling the #ifdef LEADER` code blocks); in the current configuration, all clubs are followers and have identical code. The only line you need to change is `#define SHORTER_STRIPS` for clubs that have fewer LEDs to control. 

You should not need to upload the SPIFFS files to the juggling club boards.

#### 2) wifiRepeater.ino

Upload this to one of the boards on the breadboard with Serial connection to the `wifiMesh.ino` board. It is not important which board this is uploaded to, because they are connected symmetrically on the breadboard, as follows, but the convention is to upload this sketch to the board whose USB port is sticking out off the breadboard. 

* Vin <-> Vin
* GND <-> GND
* RX  <-> TX
* TX <->  RX

`wifiRepeater.ino` creates the front-end of the interface, including hosting the HTML page via WiFi Access Point. Therefore, you must remember to upload the SPIFFS to this board, or the interface will not appear. You can also easily update the interface by updating the `index.html` and `style.css` files inside the folder named `data` inside the `wifiRepeater` folder. I have given the WiFi access point the name `Juggling Club Controller`; you can change the interface (controller)'s access point name by changing the value in this sketch.

#### 3) meshRepeater.ino

This is the other half of the repeater and should be uploaded (by convention) to the other board on the breadboard whose USB port is only accessible by having the USB plug cover up the middle part of the breadboard. You do not need to upload any SPIFFS, although you must ensure that the mesh username and password are the same as in the `jugglingClubMain.ino` sketch. I have named this the `Circus IT Department`.
