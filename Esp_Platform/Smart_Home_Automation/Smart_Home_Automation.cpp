#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DHT.h>

// --- Pin Definitions ---
#define DHT_PIN D4        // DHT-11 Data Pin (GPIO2)
#define LED_LIVING D1     // Living Room LED (GPIO5)
#define LED_BEDROOM D2    // Bedroom LED (GPIO4)
#define LED_KITCHEN D5    // Kitchen LED (GPIO14)
#define LED_BATHROOM D6   // Bathroom LED (GPIO12)

#define DHT_TYPE DHT11
DHT dht(DHT_PIN, DHT_TYPE);

const char* ssid = "Smart_Home";
const char* password = "home1234";

ESP8266WebServer server(80);

// LED States
bool livingState = false;
bool bedroomState = false;
bool kitchenState = false;
bool bathroomState = false;

float temperature = 0.0;
float humidity = 0.0;

const char webpage[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Smart Home</title>
  <style>
    * { margin: 0; padding: 0; box-sizing: border-box; }
    body {
      font-family: Arial, sans-serif;
      background: linear-gradient(135deg, #1a1a2e 0%, #16213e 100%);
      min-height: 100vh;
      padding: 20px;
      color: white;
    }
    .container {
      max-width: 600px;
      margin: 0 auto;
    }
    h1 {
      text-align: center;
      color: #00d4ff;
      margin-bottom: 10px;
      font-size: 32px;
      text-shadow: 0 0 20px rgba(0, 212, 255, 0.5);
    }
    .subtitle {
      text-align: center;
      color: #888;
      margin-bottom: 30px;
      font-size: 14px;
    }
    .sensor-section {
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 15px;
      margin-bottom: 30px;
    }
    .sensor-box {
      background: linear-gradient(135deg, #2e3440 0%, #3b4252 100%);
      padding: 25px;
      border-radius: 15px;
      text-align: center;
      border: 1px solid rgba(255, 255, 255, 0.1);
    }
    .sensor-value {
      font-size: 42px;
      color: #00d4ff;
      font-weight: bold;
      text-shadow: 0 0 15px rgba(0, 212, 255, 0.5);
      margin-bottom: 5px;
    }
    .sensor-label {
      font-size: 13px;
      color: #aaa;
      text-transform: uppercase;
      letter-spacing: 1px;
    }
    .control-section {
      display: grid;
      gap: 12px;
    }
    .room-card {
      background: linear-gradient(135deg, #2e3440 0%, #3b4252 100%);
      border-radius: 12px;
      padding: 18px 20px;
      display: flex;
      justify-content: space-between;
      align-items: center;
      border: 1px solid rgba(255, 255, 255, 0.1);
      transition: all 0.3s;
    }
    .room-card:hover {
      transform: translateY(-2px);
      box-shadow: 0 8px 20px rgba(0, 212, 255, 0.2);
      border-color: rgba(0, 212, 255, 0.3);
    }
    .room-name {
      font-size: 18px;
      font-weight: 600;
      color: #e0e0e0;
    }
    .switch {
      position: relative;
      display: inline-block;
      width: 60px;
      height: 34px;
    }
    .switch input {
      opacity: 0;
      width: 0;
      height: 0;
    }
    .slider {
      position: absolute;
      cursor: pointer;
      top: 0; left: 0; right: 0; bottom: 0;
      background-color: #444;
      transition: 0.4s;
      border-radius: 34px;
    }
    .slider:before {
      position: absolute;
      content: "";
      height: 26px;
      width: 26px;
      left: 4px;
      bottom: 4px;
      background-color: #888;
      transition: 0.4s;
      border-radius: 50%;
    }
    input:checked + .slider {
      background-color: #00d4ff;
      box-shadow: 0 0 20px rgba(0, 212, 255, 0.6);
    }
    input:checked + .slider:before {
      transform: translateX(26px);
      background-color: white;
    }
    .footer {
      text-align: center;
      margin-top: 25px;
      color: #666;
      font-size: 12px;
    }
  </style>
</head>
<body>
  <div class="container">
    <h1>Smart Home</h1>
    <div class="subtitle">IoT Dashboard</div>
    
    <div class="sensor-section">
      <div class="sensor-box">
        <div class="sensor-value" id="temp">--</div>
        <div class="sensor-label">Temperature °C</div>
      </div>
      <div class="sensor-box">
        <div class="sensor-value" id="hum">--</div>
        <div class="sensor-label">Humidity %</div>
      </div>
    </div>

    <div class="control-section">
      <div class="room-card">
        <div class="room-name">Living Room</div>
        <label class="switch">
          <input type="checkbox" id="living" onchange="toggleLED('living')">
          <span class="slider"></span>
        </label>
      </div>

      <div class="room-card">
        <div class="room-name">Bedroom</div>
        <label class="switch">
          <input type="checkbox" id="bedroom" onchange="toggleLED('bedroom')">
          <span class="slider"></span>
        </label>
      </div>

      <div class="room-card">
        <div class="room-name">Kitchen</div>
        <label class="switch">
          <input type="checkbox" id="kitchen" onchange="toggleLED('kitchen')">
          <span class="slider"></span>
        </label>
      </div>

      <div class="room-card">
        <div class="room-name">Bathroom</div>
        <label class="switch">
          <input type="checkbox" id="bathroom" onchange="toggleLED('bathroom')">
          <span class="slider"></span>
        </label>
      </div>
    </div>

    <div class="footer">Powered by ESP8266</div>
  </div>

  <script>
    function update() {
      fetch('/data')
        .then(r => r.json())
        .then(data => {
          document.getElementById('temp').innerText = data.temp;
          document.getElementById('hum').innerText = data.hum;
          document.getElementById('living').checked = data.living;
          document.getElementById('bedroom').checked = data.bedroom;
          document.getElementById('kitchen').checked = data.kitchen;
          document.getElementById('bathroom').checked = data.bathroom;
        })
        .catch(e => console.log('Error:', e));
    }
    
    function toggleLED(room) {
      fetch('/toggle?room=' + room)
        .then(() => update())
        .catch(e => console.log('Toggle error:', e));
    }
    
    setInterval(update, 2000);
    setTimeout(update, 500);
  </script>
</body>
</html>
)=====";

void handleRoot() {
  Serial.println("Root page accessed");
  server.send(200, "text/html", webpage);
}

void handleData() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  
  if (!isnan(t)) temperature = t;
  if (!isnan(h)) humidity = h;
  
  String json = "{";
  json += "\"temp\":\"" + String(temperature, 1) + "°C\",";
  json += "\"hum\":\"" + String(humidity, 1) + "%\",";
  json += "\"living\":" + String(livingState ? "true" : "false") + ",";
  json += "\"bedroom\":" + String(bedroomState ? "true" : "false") + ",";
  json += "\"kitchen\":" + String(kitchenState ? "true" : "false") + ",";
  json += "\"bathroom\":" + String(bathroomState ? "true" : "false");
  json += "}";
  
  server.send(200, "application/json", json);
}

void handleToggle() {
  if (server.hasArg("room")) {
    String room = server.arg("room");
    
    if (room == "living") {
      livingState = !livingState;
      digitalWrite(LED_LIVING, livingState ? HIGH : LOW);
      Serial.println("Living Room: " + String(livingState ? "ON" : "OFF"));
    } 
    else if (room == "bedroom") {
      bedroomState = !bedroomState;
      digitalWrite(LED_BEDROOM, bedroomState ? HIGH : LOW);
      Serial.println("Bedroom: " + String(bedroomState ? "ON" : "OFF"));
    }
    else if (room == "kitchen") {
      kitchenState = !kitchenState;
      digitalWrite(LED_KITCHEN, kitchenState ? HIGH : LOW);
      Serial.println("Kitchen: " + String(kitchenState ? "ON" : "OFF"));
    }
    else if (room == "bathroom") {
      bathroomState = !bathroomState;
      digitalWrite(LED_BATHROOM, bathroomState ? HIGH : LOW);
      Serial.println("Bathroom: " + String(bathroomState ? "ON" : "OFF"));
    }
  }
  server.send(200, "text/plain", "OK");
}

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("\n========== Smart Home Starting ==========");
  
  dht.begin();
  Serial.println("DHT initialized");
  
  // Initialize LED pins
  pinMode(LED_LIVING, OUTPUT);
  pinMode(LED_BEDROOM, OUTPUT);
  pinMode(LED_KITCHEN, OUTPUT);
  pinMode(LED_BATHROOM, OUTPUT);
  
  digitalWrite(LED_LIVING, LOW);
  digitalWrite(LED_BEDROOM, LOW);
  digitalWrite(LED_KITCHEN, LOW);
  digitalWrite(LED_BATHROOM, LOW);
  Serial.println("All LEDs initialized (OFF)");
  
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  
  Serial.println("AP Mode Active");
  Serial.print("SSID: ");
  Serial.println(ssid);
  Serial.print("IP: ");
  Serial.println(WiFi.softAPIP());
  
  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.on("/toggle", handleToggle);
  server.begin();
  
  Serial.println("Server started at http://192.168.4.1");
  Serial.println("=========================================\n");
}

void loop() {
  server.handleClient();
}
