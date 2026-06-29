#include <ESP8266WiFi.h>

const char* ssid = "SmartDispenser_AP";
const char* password = "12345678";

const int RELAY1_PIN = D1; // Tap-1
const int RELAY2_PIN = D2; // Tap-2

WiFiServer server(80);

// Track relay states and auto-off timing
unsigned long relay1Timer = 0;
unsigned long relay2Timer = 0;
const unsigned long ON_DURATION = 3000; // 3 seconds in ms

void setup() {
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  digitalWrite(RELAY1_PIN, HIGH);
  digitalWrite(RELAY2_PIN, HIGH);

  WiFi.softAP(ssid, password);
  server.begin();
  Serial.begin(115200);
}

void loop() {
  WiFiClient client = server.available();

  // Handle relay timers
  if (relay1Timer && millis() - relay1Timer >= ON_DURATION) {
    digitalWrite(RELAY1_PIN, HIGH);
    relay1Timer = 0;
  }
  if (relay2Timer && millis() - relay2Timer >= ON_DURATION) {
    digitalWrite(RELAY2_PIN, HIGH);
    relay2Timer = 0;
  }

  if (!client) return;

  String req = client.readStringUntil('\r');
  client.flush();

  if (req.indexOf("/tap1ON") != -1) {
    digitalWrite(RELAY1_PIN, LOW);    // Turn relay ON
    relay1Timer = millis();           // Start timer
  }
  if (req.indexOf("/tap2ON") != -1) {
    digitalWrite(RELAY2_PIN, LOW);    // Turn relay ON
    relay2Timer = millis();           // Start timer
  }

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println();
  client.println("<!DOCTYPE html>");
  client.println("<html><head><title>Smart Dispenser</title>");
  client.println("<style>"
                  "body { font-family:Arial,sans-serif; background:#222; color:#fff; text-align:center; }"
                  ".btn { border:none; background:#444; color:#fff; font-size:20px; padding:15px 35px; margin:10px; border-radius:10px; cursor:pointer; transition: background 0.2s; display:inline-flex; align-items:center; }"
                  ".btn:hover { background:#fff; color:#222; }"
                  ".icon { font-size:28px; margin-right:8px; }"
                  "h2 { font-weight:normal; letter-spacing:2px; margin-bottom:24px; }"
                "</style>");
  client.println("</head><body>");
  client.println("<h2>ESP8266 Smart Dispenser</h2>");
  // Tap-1 Controls (single ON button)
  client.println("<button class='btn' onclick=\"location.href='/tap1ON'\"><span class='icon'>&#128688;</span>Tap-1 Dispense</button>");
  client.println("<br><br>");
  // Tap-2 Controls (single ON button)
  client.println("<button class='btn' onclick=\"location.href='/tap2ON'\"><span class='icon'>&#128688;</span>Tap-2 Dispense</button>");
  client.println("</body></html>");
}
