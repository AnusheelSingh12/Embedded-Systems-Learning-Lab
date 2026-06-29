#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// --- Pin Definitions ---
#define GREEN_LED D1
#define RED_LED   D2
#define BUZZER    D3

// --- Wi-Fi and Password Settings ---
const char* ssid = "DoorLock_AP";
const char* password = "password123";

// --- IMPORTANT: Set your 6-digit secret PIN here ---
const char* SECRET_PIN = "123456";

ESP8266WebServer server(80);

// --- Webpage Assets stored in Flash Memory (PROGMEM) ---
const char webpage_header[] PROGMEM = R"=====(
<!DOCTYPE html><html><head><title>ESP8266 CyberLock</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<link href="https://fonts.googleapis.com/css?family=Orbitron:wght@600" rel="stylesheet">
<style>
  body{background:#0d0221;color:#f0f0f0;font-family:'Orbitron',monospace,sans-serif;margin:0;min-height:100vh;display:flex;justify-content:center;align-items:center;text-align:center;}
  .container{background:rgba(20,10,32,0.95);border:2px solid #08f7fe;border-radius:16px;padding:2em 2.5em;box-shadow:0 0 20px #08f7fe, 0 0 60px #f6019d30;width:90%;max-width:400px;}
  h2{color:#08f7fe;text-shadow:0 0 8px #f6019d90, 0 0 2px #08f7fe;margin-bottom:1em;letter-spacing:2px;}
  input[type='password']{font-family:inherit;background:#11002a;border:1.5px solid #f6019d;color:#08f7fe;padding:0.8em;width:85%;font-size:1.2em;border-radius:8px;margin-top:0.5em;box-shadow:0 0 9px #f6019d50;outline:none;transition:box-shadow .2s, border-color .2s;}
  input[type='password']:focus{box-shadow:0 0 18px #08f7fe80;border-color:#08f7fe;}
  input[type='submit']{margin-top:1.5em;background:linear-gradient(90deg, #f6019d 0%, #08f7fe 100%);border:none;color:#fff;font-size:1.25em;padding:0.7em 2.5em;border-radius:10px;cursor:pointer;box-shadow:0 0 10px #08f7fe88;letter-spacing:3px;font-family:inherit;text-shadow:0 0 8px #f6019d33;transition:all .3s;}
  input[type='submit']:hover{box-shadow:0 0 25px #08f7fe;transform:scale(1.05);}
  .message{font-size:1.5em;margin-bottom:1em;text-shadow:0 0 10px;font-weight:bold;}
  .success{color:#39ff14;}
  .fail{color:#ff3366;}
  a{display:inline-block;margin-top:1.5em;color:#fffb96;text-decoration:none;border:1px solid #fffb96;padding:0.5em 1em;border-radius:5px;transition:background 0.3s, color 0.3s;}
  a:hover{background:#fffb96;color:#0d0221;}
</style></head><body><div class="container">
)=====";

const char webpage_footer[] PROGMEM = R"=====(</div></body></html>)=====";

// --- Tone Functions ---
void playSuccessTone() {
  digitalWrite(GREEN_LED, HIGH); tone(BUZZER, 600); delay(100);
  digitalWrite(GREEN_LED, LOW);  tone(BUZZER, 800); delay(100);
  digitalWrite(GREEN_LED, HIGH); tone(BUZZER, 1000); delay(100);
  noTone(BUZZER); digitalWrite(GREEN_LED, LOW);
}

void playFailTone() {
  for (int i = 0; i < 2; i++) {
    digitalWrite(RED_LED, HIGH);
    for (int hz = 800; hz >= 400; hz -= 50) { tone(BUZZER, hz); delay(25); }
    digitalWrite(RED_LED, LOW);
    for (int hz = 400; hz <= 800; hz += 50) { tone(BUZZER, hz); delay(25); }
  }
  noTone(BUZZER); digitalWrite(RED_LED, LOW);
}

// --- Web Page Handlers ---
void handleRoot() {
  String page = FPSTR(webpage_header);
  page += F("<h2>SYSTEM TERMINAL</h2><form action='/check' method='POST'>"
            "<input type='password' name='pin' maxlength='6' required autocomplete='off' placeholder='Enter CyberPIN'><br>"
            "<input type='submit' value='AUTHENTICATE'></form>");
  page += FPSTR(webpage_footer);
  server.send(200, "text/html", page);
}

void handleCheck() {
  String page = FPSTR(webpage_header);
  if (server.hasArg("pin") && strcmp(server.arg("pin").c_str(), SECRET_PIN) == 0) {
    // CORRECT PIN
    Serial.println("Correct PIN.");
    playSuccessTone();
    page += F("<h2>ACCESS GRANTED</h2><p class='message success'>SEQUENCE ACCEPTED</p><a href='/'>RETURN</a>");
  } else {
    // INCORRECT PIN
    Serial.println("Incorrect PIN.");
    page += F("<h2>ACCESS DENIED</h2><p class='message fail'>ALARM TRIGGERED</p><a href='/'>TRY AGAIN</a>");
    playFailTone(); // Play tone after sending page data to prevent blocking
  }
  page += FPSTR(webpage_footer);
  server.send(200, "text/html", page);
}

void setup() {
  Serial.begin(115200);
  Serial.println("\n\n--- CyberLock System Booting ---");

  // Start Wi-Fi FIRST (power-intensive)
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  Serial.print("AP Started. IP: "); Serial.println(WiFi.softAPIP());

  // Initialize GPIO pins
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  digitalWrite(GREEN_LED, LOW); // Start with lights OFF
  digitalWrite(RED_LED, LOW);   // Start with lights OFF
  Serial.println("GPIO Initialized. System Ready.");

  // Start web server
  server.on("/", HTTP_GET, handleRoot);
  server.on("/check", HTTP_POST, handleCheck);
  server.begin();
}

void loop() {
  server.handleClient();
}