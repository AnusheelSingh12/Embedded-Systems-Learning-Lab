#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DHT.h>

// Pin Definitions
#define DHTPIN D2
#define DHTTYPE DHT11
#define SOIL_PIN A0        // Analog Soil moisture to A0
#define LDR_PIN D5         // LDR digital module OUT to D5
#define BLUE_LED_PIN D4    // Internal blue LED (active LOW)

DHT dht(DHTPIN, DHTTYPE);
ESP8266WebServer server(80);

// Wi-Fi AP Settings
const char* ssid = "PlantMonitorAP";
const char* password = "12345678";

// Plant Status: Plant-1 online, Plant-2 offline
const char* plant1Status = "online";
const char* plant2Status = "offline";

// ---- SENSOR READ FUNCTIONS ----

// Soil Moisture: calibrate min/max with your own serial values
int readSoilMoisture() {
  int raw = analogRead(SOIL_PIN);      // typical: wet ~400, dry ~900–1023
  int percent = map(raw, 900, 400, 0, 100); // map dry=900 to 0%, wet=400 to 100%
  percent = constrain(percent, 0, 100); 
  return percent;
}

// LDR: swapped logic as requested (HIGH = Dim, LOW = Bright)
String readLDR() {
  int state = digitalRead(LDR_PIN);
  return (state == HIGH) ? "Dim" : "Bright";
}

// ---- WEB PAGE HANDLING ----

void handleRoot() {
  digitalWrite(BLUE_LED_PIN, LOW); // Turn internal blue LED ON when page is loaded (active LOW)

  String html = R"rawliteral(
<!DOCTYPE html>
<html lang='en'>
<head>
  <meta charset='UTF-8'>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Plant Monitoring System</title>
  <style>
    body {
      margin: 0; padding: 0; min-height: 100vh;
      background: radial-gradient(ellipse at top left, #c1f1fa 70%, #e0feea 100%);
      font-family: 'Segoe UI', 'Roboto', 'Verdana', Arial, sans-serif;
      display: flex; flex-direction: column; justify-content: flex-start; align-items: center;
    }
    h1 {
      margin-top: 36px; margin-bottom: 10px;
      font-size: 2.45em;
      letter-spacing: 1.3px;
      color: #137835;
      text-shadow: 0 3px 18px #b8e0b7;
    }
    .plant-status-wrap {
      display: flex; gap: 25px; justify-content: center; margin: 35px 0 12px 0;
    }
    .plant-card {
      background: rgba(255,255,255,0.72);
      border-radius: 24px;
      box-shadow: 0 2px 24px #88cdc7ad;
      padding: 23px 36px 18px 36px;
      min-width: 153px;
      border:0.2em solid;
      position: relative;
    }
    .plant-title {
      font-weight: bold; font-size: 1.08em; margin-bottom: 7px; color: #247a1b; letter-spacing: 0.9px;
    }
    .plant-online {
      color: #068b21;
      background: #d3ffe3;
      border-color: #32c463;
      font-weight: bold;
      border-width: 0.13em;
      box-shadow: 0 0 16px #c3ffc6;
    }
    .plant-offline {
      color: #d63b2b;
      background: #fff2f2;
      border-color: #ed8787;
      font-weight: bold;
      border-width: 0.13em;
      box-shadow: 0 0 16px #ffd1d1;
    }
    .metrics-box {
      margin-top: 28px;
      background: rgba(240,251,246,0.9);
      border-radius: 26px;
      box-shadow: 0 4px 32px 4px #4dddd6bb;
      padding: 29px 35px 28px 35px;
      max-width: 340px;
      width: 93vw;
      min-width: 245px;
      transition: box-shadow 0.25s;
    }
    .metrics-box:hover {
      box-shadow: 0 8px 44px 8px #137c5d44;
    }
    .metric-row {
      display: flex; justify-content: space-between; align-items: center;
      margin-bottom: 16px; font-size: 1.18em;
      font-weight: 500; color: #202626;
      padding: 7px 0 5px 0;
      border-bottom: 1px solid #e2ecee;
    }
    .metric-row:last-child {
      border-bottom: none; margin-bottom: 0;
    }
    .label {color: #36bab3; font-weight:600; font-size:1em;}
    .value {color: #287748;}
    @media (max-width:600px){
      .plant-status-wrap{flex-direction:column;align-items:center;}
      .plant-card{padding:18px 16px;}
      .metrics-box{padding:18px 6vw;}
      h1{font-size:2em;}
    }
  </style>
  <script>
    function updateReadings(){
      var xhr = new XMLHttpRequest();
      xhr.onreadystatechange = function(){
        if(this.readyState == 4 && this.status == 200){
          let data = JSON.parse(this.responseText);
          document.getElementById('temp').textContent = data.temperature + ' °C';
          document.getElementById('hum').textContent = data.humidity + ' %';
          document.getElementById('soil').textContent = data.soil + ' %';
          document.getElementById('ldr').textContent  = data.ldr;
        }
      };
      xhr.open('GET', '/data', true);
      xhr.send();
    }
    setInterval(updateReadings, 5000);
    window.onload = updateReadings;
  </script>
</head>
<body>
  <h1>Plant Monitoring System</h1>
  <div class="plant-status-wrap">
    <div class="plant-card plant-online">
      <div class="plant-title">Plant-1</div>
      Online
    </div>
    <div class="plant-card plant-offline">
      <div class="plant-title">Plant-2</div>
      Offline
    </div>
  </div>
  <div class="metrics-box">
    <div class="metric-row"><span class="label">Temperature</span><span id="temp" class="value">...</span></div>
    <div class="metric-row"><span class="label">Humidity</span>   <span id="hum" class="value">...</span></div>
    <div class="metric-row"><span class="label">Soil Moisture</span> <span id="soil" class="value">...</span></div>
    <div class="metric-row"><span class="label">Light</span>   <span id="ldr" class="value">...</span></div>
  </div>
</body>
</html>
)rawliteral";

  server.send(200, "text/html", html);
}

// AJAX endpoint for dynamic data
void handleReadings() {
  float humidity    = dht.readHumidity();
  float temperature = dht.readTemperature();
  int soil          = readSoilMoisture();
  String ldr        = readLDR();

  String json = "{\"temperature\":\"" + String(temperature, 1)
              + "\",\"humidity\":\""  + String(humidity, 1)
              + "\",\"soil\":\""      + String(soil)
              + "\",\"ldr\":\""       + ldr + "\"}";
  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(SOIL_PIN, INPUT);
  pinMode(LDR_PIN, INPUT);                // Digital LDR
  pinMode(BLUE_LED_PIN, OUTPUT);
  digitalWrite(BLUE_LED_PIN, HIGH);       // Internal blue LED off (active LOW)

  WiFi.softAP(ssid, password);
  server.on("/", handleRoot);
  server.on("/data", handleReadings);
  server.begin();
  Serial.println("Access Point: PlantMonitorAP");
  Serial.println("Open http://192.168.4.1");
}

void loop() {
  server.handleClient();
}