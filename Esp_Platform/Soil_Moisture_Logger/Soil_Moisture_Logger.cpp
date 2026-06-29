#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>

// ----- Pin Definitions (selected for safest ESP32 use) -----
#define DHTPIN 27         // DHT sensor digital pin - GPIO27, safe
#define DHTTYPE DHT11     // Or DHT22, as per your sensor
#define SOIL_PIN 34       // Soil analog sensor pin - GPIO34, input-only

DHT dht(DHTPIN, DHTTYPE);
WebServer server(80);

// ----- WiFi AP Settings -----
const char* ssid     = "SoilLoggerAP";
const char* password = "logger123";

// ----- Soil Moisture Function -----
int readSoilMoisture() {
  int raw = analogRead(SOIL_PIN); // Calibrate this for your sensor
  int percent = map(raw, 3500, 1800, 0, 100); // adjust dry/wet as needed
  percent = constrain(percent, 0, 100);
  return percent;
}

// ----- Web Page Handling -----
void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang='en'>
<head>
  <meta charset='UTF-8'>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Soil Moisture & Temperature Logger</title>
  <style>
    body {
      margin: 0; padding: 0; min-height: 100vh;
      background: linear-gradient(135deg, #ecf9ff 0%, #f9fef6 100%);
      font-family: 'Segoe UI', 'Roboto', 'Verdana', Arial, sans-serif;
      display: flex; flex-direction: column; align-items: center;
    }
    h1 {
      margin-top: 40px; margin-bottom: 20px;
      font-size: 2.2em; color: #006b3c;
      font-weight: bold; letter-spacing: 1.2px;
      text-shadow: 0 2px 12px #b5e5cb;
    }
    .metrics-box {
      margin-top: 30px;
      background: #fff;
      border-radius: 18px;
      box-shadow: 0 2px 18px #b9f0f355;
      padding: 28px 32px;
      width: 100%;
      max-width: 370px;
      min-width: 230px;
    }
    .metric-row {
      display: flex; justify-content: space-between; align-items: center;
      margin-bottom: 18px; font-size: 1.18em;
      font-weight: 600; color: #1e5c36;
      padding-bottom: 10px; border-bottom: 1px solid #f2f2f2;
    }
    .metric-row:last-child { border-bottom: none; margin-bottom: 0; }
    .label {
      color: #00997b; 
    }
    .value {
      color: #1b3a4b; font-family: 'Menlo', 'Consolas', monospace;
      background: #e5f8dc; 
      padding: 3px 13px;
      border-radius: 7px;
      min-width: 60px;
      text-align: right;
      letter-spacing: 0.5px;
      font-size: 1.18em;
    }
    @media (max-width:600px){
      .metrics-box{padding:14px 4vw;}
      h1{font-size:1.4em;}
      .metric-row{font-size:1em;}
      .label, .value{font-size:1em;}
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
        }
      };
      xhr.open('GET', '/data', true);
      xhr.send();
    }
    setInterval(updateReadings, 4000);
    window.onload = updateReadings;
  </script>
</head>
<body>
  <h1>Soil Moisture &amp; Temperature Logger</h1>
  <div class="metrics-box">
    <div class="metric-row">
      <span class="label">🌡️ Temperature</span>
      <span id="temp" class="value">--</span>
    </div>
    <div class="metric-row">
      <span class="label">💧 Humidity</span>
      <span id="hum" class="value">--</span>
    </div>
    <div class="metric-row">
      <span class="label">🌱 Soil Moisture</span>
      <span id="soil" class="value">--</span>
    </div>
  </div>
</body>
</html>
)rawliteral";
  server.send(200, "text/html", html);
}

// ----- AJAX Data Endpoint -----
void handleData() {
  float humidity    = dht.readHumidity();
  float temperature = dht.readTemperature();
  int soil          = readSoilMoisture();
  String json = "{\"temperature\":\"" + String(temperature, 1)
              + "\",\"humidity\":\""  + String(humidity, 1)
              + "\",\"soil\":\""      + String(soil) + "\"}";
  server.send(200, "application/json", json);
}

// ----- WiFi/AP & HTTP Server Setup -----
void setup() {
  Serial.begin(115200);
  delay(100);
  dht.begin();
  pinMode(SOIL_PIN, INPUT);

  WiFi.softAP(ssid, password);
  Serial.print("Access Point: "); Serial.println(ssid);
  Serial.print("IP: "); Serial.println(WiFi.softAPIP());
  Serial.println("Open http://192.168.4.1");

  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.begin();
}

void loop() {
  server.handleClient();
}