#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "elevator_sim";
const char* password = "lift12345";
ESP8266WebServer server(80);

const int ledPins[5] = {5, 4, 0, 2, 15}; // D1, D2, D3, D4, D8
#define BUZZER_PIN 13 // D7 (GPIO13)

int currentFloor = 0;
int requestedFloor = 0;
bool isMoving = false;

// Monochrome CSS style
String simpleCSS = R"rawliteral(
body {
  background: #11141a;
  color: #b8eaff;
  font-family: 'Orbitron','Courier New',monospace;
  margin:0;padding:0;
}
.elevator-panel {
  margin:40px auto;
  padding:20px 32px;
  background: #181c24;
  border-radius: 15px;
  box-shadow: 0 0 24px #2fc6fd;
  max-width:430px;
  border:1.6px solid #2fc6fd;
}
h1 {
  letter-spacing:2px;
  margin-bottom: 38px;
}
.led-strip {
  display:flex;
  justify-content:space-between;
  margin:32px 0 20px 0;
}
.led {
  width:40px;height:40px;
  border-radius:50%;
  background:#212a35;
  border:3px solid #2fc6fd;
  box-shadow:0 0 10px #212a35;
  display:flex;align-items:center;justify-content:center;
  font-size:17px;font-weight:bold;
  color:#b8eaff;transition:background 0.2s,box-shadow 0.2s,color 0.15s;
}
.led.on {
  background:#2fc6fd;color:#181c24;
  box-shadow:0 0 9px #2fc6fd;
}
.floor-label {
  font-size:12px;margin-top:3px;color:#aad7ff;
}
.button {
  background:#2f343a;border:none;color:#b8eaff;padding:11px 16px;
  font-size:16px;margin:7px 6px;
  border-radius:10px; box-shadow:0 0 14px #152d3a,0 0 2px #2fc6fd;
  font-family:'Orbitron','Courier New',monospace;
  cursor:pointer;transition:background 0.16s,transform 0.15s;
}
.button:hover {
  background:#223a5c;color:#fff; transform:scale(1.05);
}
)rawliteral";

// Page renderer (LED strip and indicator, always synced)
String webPage(int floor) {
  String html = "<!DOCTYPE html><html><head><title>Elevator Logic Simulator</title><style>";
  html += simpleCSS;
  html += "</style></head><body>";
  html += "<div class='elevator-panel'><h1>Elevator Logic Simulator</h1>";

  // LED strip: 0 on left, 4 on right
  html += "<div class='led-strip'>";
  for(int i=0;i<5;i++) {
    html += "<div class='led";
    if(i==floor) html += " on";
    html += "'>" + String(i) + "<div class='floor-label'>F" + String(i) + "</div></div>";
  }
  html += "</div>";

  html += "<h2>Current Floor: " + String(floor) + "</h2><div>";

  // Floor buttons
  for(int i=0;i<5;i++) {
    html += "<form style='display:inline;' method='POST' action='/goto'>";
    html += "<input type='hidden' name='f' value='" + String(i) + "'>";
    html += "<button type='submit' class='button'>Go to Floor " + String(i) + "</button></form>";
  }
  html += "</div></div></body></html>";
  return html;
}

void handleRoot() { server.send(200, "text/html", webPage(currentFloor)); }

void handleGoto() {
  if(!isMoving && server.hasArg("f")) {
    int floor = server.arg("f").toInt();
    if(floor >= 0 && floor < 5 && floor != currentFloor) {
      requestedFloor = floor;
      isMoving = true;
      moveElevator(currentFloor, requestedFloor);
      isMoving = false;
      currentFloor = requestedFloor;
      server.send(200, "text/html", webPage(currentFloor));
      return;
    }
  }
  server.sendHeader("Location","/",true);
  server.send(302,"text/plain","");
}

// Play a "ding" sound on buzzer
void ding() {
  tone(BUZZER_PIN, 1500, 120); // 1500Hz for 120ms
  delay(130); // Wait for buzzer sound to complete
  noTone(BUZZER_PIN);
}

// Synchronous movement: UI and LEDs together, no delay/diff
void moveElevator(int from, int to) {
  int step = (to > from) ? 1 : -1;
  for(int f=from+step; (step>0)?(f<=to):(f>=to); f+=step) {
    for(int i=0;i<5;i++) digitalWrite(ledPins[i],(i == f) ? HIGH : LOW);
    ding(); // Sound the buzzer at each floor
    delay(800); // For realism
  }
}

void setup() {
  for(int i=0;i<5;i++) {
    pinMode(ledPins[i],OUTPUT);
    digitalWrite(ledPins[i],LOW);
  }
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  digitalWrite(ledPins[currentFloor],HIGH);
  WiFi.softAP(ssid,password);
  server.on("/",HTTP_GET,handleRoot);
  server.on("/goto",HTTP_POST,handleGoto);
  server.begin();
}

void loop() {
  server.handleClient();
  // Maintain correct floor LED
  for(int i=0;i<5;i++) digitalWrite(ledPins[i],(i==currentFloor)?HIGH:LOW);
}