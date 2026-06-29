#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// --- Pin Definitions ---
#define IR_SENSOR_1   D1  // GPIO5 - Parking Spot 1
#define IR_SENSOR_2   D2  // GPIO4 - Parking Spot 2
#define GREEN_LED_1   D5  // GPIO14 - Spot 1 Empty Indicator
#define GREEN_LED_2   D6  // GPIO12 - Spot 2 Empty Indicator
#define RED_LED_1     D7  // GPIO13 - Spot 1 Occupied Indicator
#define RED_LED_2     D8  // GPIO15 - Spot 2 Occupied Indicator

// --- Wi-Fi Settings ---
const char* ssid = "Smart_Parking";
const char* password = "park1234";

ESP8266WebServer server(80);

// --- Parking Spot States ---
bool spot1Occupied = false;
bool spot2Occupied = false;

// Previous states for edge detection
bool prevSpot1 = false;
bool prevSpot2 = false;

// Statistics
int totalSlots = 2;
int occupiedSlots = 0;
int emptySlots = 2;

// --- Update LED States ---
void updateLEDs() {
  // Spot 1
  if (spot1Occupied) {
    digitalWrite(RED_LED_1, HIGH);
    digitalWrite(GREEN_LED_1, LOW);
  } else {
    digitalWrite(RED_LED_1, LOW);
    digitalWrite(GREEN_LED_1, HIGH);
  }
  
  // Spot 2
  if (spot2Occupied) {
    digitalWrite(RED_LED_2, HIGH);
    digitalWrite(GREEN_LED_2, LOW);
  } else {
    digitalWrite(RED_LED_2, LOW);
    digitalWrite(GREEN_LED_2, HIGH);
  }
}

// --- Read Sensor States ---
void updateParkingStatus() {
  // Read sensors (LOW = occupied, HIGH = empty)
  bool currentSpot1 = (digitalRead(IR_SENSOR_1) == LOW);
  bool currentSpot2 = (digitalRead(IR_SENSOR_2) == LOW);
  
  // Detect Spot 1 changes
  if (currentSpot1 != prevSpot1) {
    if (currentSpot1) {
      Serial.println("Spot 1: Car ENTERED");
    } else {
      Serial.println("Spot 1: Car LEFT");
    }
    prevSpot1 = currentSpot1;
  }
  
  // Detect Spot 2 changes
  if (currentSpot2 != prevSpot2) {
    if (currentSpot2) {
      Serial.println("Spot 2: Car ENTERED");
    } else {
      Serial.println("Spot 2: Car LEFT");
    }
    prevSpot2 = currentSpot2;
  }
  
  // Update states
  spot1Occupied = currentSpot1;
  spot2Occupied = currentSpot2;
  
  // Calculate statistics
  occupiedSlots = (spot1Occupied ? 1 : 0) + (spot2Occupied ? 1 : 0);
  emptySlots = totalSlots - occupiedSlots;
  
  // Update LEDs
  updateLEDs();
}

// --- HTML Web Page ---
const char webpage[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Smart Parking System</title>
  <style>
    * { margin: 0; padding: 0; box-sizing: border-box; }
    body {
      font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
      background: linear-gradient(135deg, #0f2027 0%, #203a43 50%, #2c5364 100%);
      min-height: 100vh;
      padding: 20px;
      color: white;
    }
    .container {
      max-width: 700px;
      margin: 0 auto;
    }
    h1 {
      text-align: center;
      color: #00ff88;
      margin-bottom: 10px;
      font-size: 36px;
      text-shadow: 0 0 20px rgba(0, 255, 136, 0.5);
    }
    .subtitle {
      text-align: center;
      color: #aaa;
      margin-bottom: 30px;
      font-size: 14px;
      letter-spacing: 2px;
      text-transform: uppercase;
    }
    .stats-grid {
      display: grid;
      grid-template-columns: repeat(3, 1fr);
      gap: 15px;
      margin-bottom: 30px;
    }
    .stat-box {
      background: rgba(255, 255, 255, 0.05);
      border: 1px solid rgba(255, 255, 255, 0.1);
      border-radius: 12px;
      padding: 20px;
      text-align: center;
      backdrop-filter: blur(10px);
    }
    .stat-value {
      font-size: 42px;
      font-weight: bold;
      margin-bottom: 8px;
    }
    .stat-value.total { color: #00d4ff; }
    .stat-value.empty { color: #00ff88; }
    .stat-value.occupied { color: #ff6b6b; }
    .stat-label {
      font-size: 12px;
      color: #bbb;
      text-transform: uppercase;
      letter-spacing: 1px;
    }
    .parking-grid {
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 20px;
      margin-bottom: 20px;
    }
    .parking-spot {
      background: rgba(255, 255, 255, 0.05);
      border: 2px solid rgba(255, 255, 255, 0.1);
      border-radius: 15px;
      padding: 30px 20px;
      text-align: center;
      transition: all 0.3s;
      position: relative;
      overflow: hidden;
    }
    .parking-spot::before {
      content: '';
      position: absolute;
      top: 0;
      left: 0;
      right: 0;
      height: 5px;
      transition: all 0.3s;
    }
    .parking-spot.empty::before {
      background: linear-gradient(90deg, #00ff88, #00d4ff);
    }
    .parking-spot.occupied::before {
      background: linear-gradient(90deg, #ff6b6b, #ff4757);
    }
    .spot-number {
      font-size: 18px;
      color: #aaa;
      margin-bottom: 15px;
      font-weight: 600;
    }
    .spot-icon {
      font-size: 48px;
      margin-bottom: 15px;
    }
    .spot-status {
      font-size: 20px;
      font-weight: bold;
      text-transform: uppercase;
      letter-spacing: 2px;
    }
    .status-empty {
      color: #00ff88;
      text-shadow: 0 0 10px rgba(0, 255, 136, 0.5);
    }
    .status-occupied {
      color: #ff6b6b;
      text-shadow: 0 0 10px rgba(255, 107, 107, 0.5);
    }
    .footer {
      text-align: center;
      margin-top: 30px;
      color: #666;
      font-size: 12px;
    }
    @keyframes pulse {
      0%, 100% { opacity: 1; }
      50% { opacity: 0.6; }
    }
    .occupied .spot-icon {
      animation: pulse 2s infinite;
    }
  </style>
</head>
<body>
  <div class="container">
    <h1>🚗 Smart Parking</h1>
    <div class="subtitle">IoT Parking Management</div>
    
    <div class="stats-grid">
      <div class="stat-box">
        <div class="stat-value total" id="totalSlots">2</div>
        <div class="stat-label">Total Slots</div>
      </div>
      <div class="stat-box">
        <div class="stat-value empty" id="emptySlots">2</div>
        <div class="stat-label">Empty</div>
      </div>
      <div class="stat-box">
        <div class="stat-value occupied" id="occupiedSlots">0</div>
        <div class="stat-label">Occupied</div>
      </div>
    </div>

    <div class="parking-grid">
      <div class="parking-spot empty" id="spot1">
        <div class="spot-number">SPOT 1</div>
        <div class="spot-icon">🚙</div>
        <div class="spot-status status-empty" id="status1">EMPTY</div>
      </div>

      <div class="parking-spot empty" id="spot2">
        <div class="spot-number">SPOT 2</div>
        <div class="spot-icon">🚙</div>
        <div class="spot-status status-empty" id="status2">EMPTY</div>
      </div>
    </div>

    <div class="footer">Powered by ESP8266 IoT System</div>
  </div>

  <script>
    function updateDashboard() {
      fetch('/data')
        .then(r => r.json())
        .then(data => {
          // Update statistics
          document.getElementById('totalSlots').innerText = data.total;
          document.getElementById('emptySlots').innerText = data.empty;
          document.getElementById('occupiedSlots').innerText = data.occupied;
          
          // Update Spot 1
          const spot1 = document.getElementById('spot1');
          const status1 = document.getElementById('status1');
          if (data.spot1) {
            spot1.className = 'parking-spot occupied';
            status1.className = 'spot-status status-occupied';
            status1.innerText = 'OCCUPIED';
          } else {
            spot1.className = 'parking-spot empty';
            status1.className = 'spot-status status-empty';
            status1.innerText = 'EMPTY';
          }
          
          // Update Spot 2
          const spot2 = document.getElementById('spot2');
          const status2 = document.getElementById('status2');
          if (data.spot2) {
            spot2.className = 'parking-spot occupied';
            status2.className = 'spot-status status-occupied';
            status2.innerText = 'OCCUPIED';
          } else {
            spot2.className = 'parking-spot empty';
            status2.className = 'spot-status status-empty';
            status2.innerText = 'EMPTY';
          }
        })
        .catch(e => console.log('Error:', e));
    }
    
    setInterval(updateDashboard, 1000);
    setTimeout(updateDashboard, 300);
  </script>
</body>
</html>
)=====";

void handleRoot() {
  server.send(200, "text/html", webpage);
}

void handleData() {
  String json = "{";
  json += "\"total\":" + String(totalSlots) + ",";
  json += "\"empty\":" + String(emptySlots) + ",";
  json += "\"occupied\":" + String(occupiedSlots) + ",";
  json += "\"spot1\":" + String(spot1Occupied ? "true" : "false") + ",";
  json += "\"spot2\":" + String(spot2Occupied ? "true" : "false");
  json += "}";
  
  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n========== Smart Parking System ==========");
  
  // Initialize sensor pins
  pinMode(IR_SENSOR_1, INPUT);
  pinMode(IR_SENSOR_2, INPUT);
  Serial.println("IR Sensors initialized");
  
  // Initialize LED pins
  pinMode(GREEN_LED_1, OUTPUT);
  pinMode(GREEN_LED_2, OUTPUT);
  pinMode(RED_LED_1, OUTPUT);
  pinMode(RED_LED_2, OUTPUT);
  Serial.println("LEDs initialized");
  
  // Set initial LED states
  updateLEDs();
  
  // Start Wi-Fi AP
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  Serial.println("Access Point Started");
  Serial.print("SSID: ");
  Serial.println(ssid);
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());
  
  // Setup web server
  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.begin();
  
  Serial.println("Web server started at http://192.168.4.1");
  Serial.println("==========================================\n");
}

void loop() {
  server.handleClient();
  updateParkingStatus();
  delay(100);  // Small delay for stability
}
