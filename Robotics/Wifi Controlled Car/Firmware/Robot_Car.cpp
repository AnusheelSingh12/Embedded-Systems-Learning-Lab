#include <WiFi.h>
#include <WebServer.h>

// Motor pins
#define IN1 26   // Left motor: forward
#define IN2 27   // Left motor: backward
#define IN3 14   // Right motor: forward
#define IN4 12   // Right motor: backward
#define ENA 25   // Left motor enable
#define ENB 13   // Right motor enable

const char* ssid = "RoboCar";
const char* password = "robot123";

WebServer server(80);
bool driving_forward = false;

void enableMotors() {
  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);
}

void disableMotors() {
  digitalWrite(ENA, LOW);
  digitalWrite(ENB, LOW);
}

void forward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  enableMotors();
  driving_forward = true;
}

void backward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  enableMotors();
  driving_forward = false;
}

void left() {
  if(driving_forward) {
    digitalWrite(IN1, LOW);      // Stop left wheel
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);     // Right wheel forward
    digitalWrite(IN4, LOW);
    enableMotors();
  }
}

void right() {
  if(driving_forward) {
    digitalWrite(IN1, HIGH);     // Left wheel forward
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);      // Stop right wheel
    digitalWrite(IN4, LOW);
    enableMotors();
  }
}

void stop() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  disableMotors();
  driving_forward = false;
}

const char page[] = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Robot Control</title>
<style>
body{
  margin:0;padding:0;
  background:#191a1b;
  color:#ededed;
  font-family:'Segoe UI',Arial,sans-serif;
  min-height:100vh;
}
.container{display:flex;flex-direction:column;align-items:center;justify-content:center;min-height:100vh;}
h1{
  color:#bbb;
  margin-bottom:16px;
  letter-spacing:2px;
  font-size:38px;
  font-weight:700;
  margin-top:16px;margin-bottom:12px;
}
.kbd-row {margin-bottom: 10px;}
.kbd-row kbd {
  background: #282828;
  color: #ababab;
  border: 1.8px solid #565656;
  border-radius: 5px;
  font-size:20px;
  letter-spacing:2px;
  padding:8px 18px;
  margin:0 3px;
  font-family:monospace;
  font-weight: bold;
  box-shadow: 0 2px 8px #1118;
}
.tiptext{
  color:#aaa;
  margin-bottom:22px;
  font-size:16px;
  text-align:center;
  letter-spacing:1.2px;
  font-weight:400;
}
.button-grid{
  display:grid;
  grid-template-columns:repeat(3, 107px);
  grid-template-rows:repeat(3, 107px);
  gap:18px;
  margin-bottom:16px;
}
.btn {
  display:flex;
  align-items:center;
  justify-content:center;
  color:#ededed;
  background:#232325;
  border:2.3px solid #636363;
  border-radius:15px;
  font-size:27px;
  font-family:inherit;
  cursor:pointer;
  outline:none;
  user-select:none;
  transition:background .13s, color .13s, box-shadow .13s, border-color .13s;
  box-shadow:0 3px 12px #0003;
}
.btn:active, .btn.pressed {
  background:#39393b;
  color:#ededed;
  border-color:#bbbbbb;
}
.empty {background:transparent;border:none;box-shadow:none;pointer-events:none;}
.footer{
  color:#444;
  margin-top:28px;
  font-size:14px;
  letter-spacing:1px;
}
</style>
</head>
<body>
  <div class="container">
    <h1>ROBOT CAR</h1>
    <div class="kbd-row">
      <kbd>W</kbd>
      <kbd>A</kbd>
      <kbd>S</kbd>
      <kbd>D</kbd>
    </div>
    <div class="tiptext">
      W = Forward, S = Back, A = Left, D = Right<br>
      Release key or click <b>STOP</b> to halt.
    </div>
    <div class="button-grid">
      <div class="empty"></div>
      <button class="btn" id="btnF" onmousedown="send('F')" onmouseup="send('S')" ontouchstart="send('F')" ontouchend="send('S')">FWD</button>
      <div class="empty"></div>
      <button class="btn" id="btnL" onmousedown="send('L')" onmouseup="send('S')" ontouchstart="send('L')" ontouchend="send('S')">LEFT</button>
      <button class="btn" id="btnS" onclick="send('S')">STOP</button>
      <button class="btn" id="btnR" onmousedown="send('R')" onmouseup="send('S')" ontouchstart="send('R')" ontouchend="send('S')">RIGHT</button>
      <div class="empty"></div>
      <button class="btn" id="btnB" onmousedown="send('B')" onmouseup="send('S')" ontouchstart="send('B')" ontouchend="send('S')">BACK</button>
      <div class="empty"></div>
    </div>
    <div class="footer">Powered by ESP32 &mdash; WSAD/Touch and Click Supported</div>
  </div>
  <script>
    function send(cmd){
      fetch('/go?d='+cmd);
    }
    let downMap = {};
    document.addEventListener('keydown',function(e){
      if(e.repeat) return;
      let key = e.key.toLowerCase();
      downMap[key]=true;
      switch(key){
        case 'w': send('F'); document.getElementById('btnF').classList.add('pressed'); break;
        case 'a': send('L'); document.getElementById('btnL').classList.add('pressed'); break;
        case 's': send('B'); document.getElementById('btnB').classList.add('pressed'); break;
        case 'd': send('R'); document.getElementById('btnR').classList.add('pressed'); break;
      }
    });
    document.addEventListener('keyup',function(e){
      let key = e.key.toLowerCase();
      if('wasd'.includes(key)){
        send('S');
        if(key=='w') document.getElementById('btnF').classList.remove('pressed');
        if(key=='a') document.getElementById('btnL').classList.remove('pressed');
        if(key=='s') document.getElementById('btnB').classList.remove('pressed');
        if(key=='d') document.getElementById('btnR').classList.remove('pressed');
        downMap[key]=false;
      }
    });
  </script>
</body>
</html>
)rawliteral";

void handleRoot() { server.send(200, "text/html", page); }

void handleGo() {
  if(server.hasArg("d")) {
    String d = server.arg("d");
    if(d=="F") forward();
    else if(d=="B") backward();
    else if(d=="L") left();
    else if(d=="R") right();
    else if(d=="S") stop();
    Serial.println("Command: " + d);
  }
  server.send(200, "text/plain", "OK");
}

void setup() {
  Serial.begin(115200);
  Serial.println("\n=== Robot Starting: mono theme, WSAD ===");
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT); pinMode(ENB, OUTPUT);
  stop();
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  Serial.print("Connect to: "); Serial.println(ssid);
  Serial.print("IP: "); Serial.println(WiFi.softAPIP());
  server.on("/", handleRoot);
  server.on("/go", handleGo);
  server.begin();
  Serial.println("Ready!");
}
void loop() { server.handleClient(); }
