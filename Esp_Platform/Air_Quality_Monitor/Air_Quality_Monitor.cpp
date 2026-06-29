/*
 * ESP32 Air Quality Monitor in Access Point (AP) Mode
 * * Hardware:
 * - ESP32 (NodeMCU or similar)
 * - DHT-11 (Temperature & Humidity)
 * - MQ-135 (Air Quality)
 * * AP Details:
 * - SSID: ESP32_Air_Quality
 * - Password: password123
 * * Web Interface:
 * - Connect to the "ESP32_Air_Quality" WiFi network.
 * - Open a browser and go to http://192.168.4.1
 * * Wiring:
 * - DHT-11 Data Pin -> 14 (GPIO14)
 * - MQ-135 A0 Pin   -> 34 (GPIO34 / ADC1_CH6)
 */

#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>

#define DHTPIN 14       // GPIO for DHT11
#define MQ135PIN 34     // ADC GPIO for MQ-135 (Note: This is ADC1_CH6)
#define DHTTYPE DHT11

const char *ssid = "ESP32_Air_Quality";
const char *password = "password123";

DHT dht(DHTPIN, DHTTYPE);
WebServer server(80);

float temperature = 0.0;
float humidity = 0.0;
int airQualityRaw = 0;
float airQualityValue = 0.0; // Smoothed value, starts at 0 (assuming low value is bad)

// Adjust these for your sensor/environment!
// Run the sensor in clean air and note the 'airQualityRaw' value from serial monitor.
const int airQualityMin = 215;   // Baseline ADC value
// Expose the sensor to some pollutants (e.g., alcohol vapor) to find a high value.
const int airQualityMax = 1500;  // Worst/polluted value (experiment to find this)

// This smoothing factor makes the value change more gradually.
// Lower value = slower, smoother changes.
const float SMOOTHING_FACTOR = 0.15;

const char htmlPage[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Air Quality Monitor</title>
<style>
  body {background: #222; color: #eee; font-family: Arial, 'Segoe UI', sans-serif;}
  h2 {text-align: center; margin-top: 15px;}
  .container {max-width: 400px; margin: auto; background: #333; padding: 20px; border-radius: 12px;}
  .sensor {margin-top: 20px; padding: 10px; background: #444; border-radius: 8px; box-shadow: 0 4px 12px #0006;}
  .label {font-weight: bold; font-size: 1.2em; margin-bottom: 6px;}
  .value {font-size: 1.5em; text-align: right; color: #d1f237;}
  .bar {background: #555; border-radius: 6px; height: 20px; overflow: hidden; margin-top: 6px;}
  .progress {height: 100%; width: 0%; transition: width 1s;}
  /* Progress bar colors */
  #airBar.excellent {background: #74f07d;}
  #airBar.moderate  {background: #fed426;}
  #airBar.poor      {background: #f7931e;}
  #airBar.hazardous {background: #f05959;}
  #tempBar {background: #ff8c00;} /* Orange for temp */
  #humBar {background: #00bfff;} /* Deep sky blue for humidity */
  .status {font-weight: 700; font-size: 1.5em; margin-top: 12px; text-align: center;}
</style>
</head>
<body>
  <div class="container">
    <h2>ESP32 Air Quality Monitor</h2>
    <div class="sensor">
      <div class="label">Temperature (°C):</div>
      <div class="value" id="tempVal">--</div>
      <div class="bar"><div id="tempBar" class="progress"></div></div>
    </div>
    <div class="sensor">
      <div class="label">Humidity (%):</div>
      <div class="value" id="humVal">--</div>
      <div class="bar"><div id="humBar" class="progress"></div></div>
    </div>
    <div class="sensor">
      <div class="label">Air Quality:</div>
      <div class="value"><span id="airVal">--</span></div>
      <div class="bar"><div id="airBar" class="progress"></div></div>
      <div class="status" id="airStatus">--</div>
    </div>
  </div>
<script>
function updateData(){
  fetch('/data').then(response => response.json()).then(data => {
    let temp = isNaN(data.temperature) ? '--' : data.temperature.toFixed(1);
    let hum = isNaN(data.humidity) ? '--' : data.humidity.toFixed(1);
    let air = data.airQualityValue !== undefined ? data.airQualityValue : '--';

    document.getElementById('tempVal').textContent = temp;
    document.getElementById('humVal').textContent = hum;
    document.getElementById('airVal').textContent = air;
    
    // Update progress bars
    // Temp bar: 0-50 C range
    document.getElementById('tempBar').style.width = (temp === '--' ? 0 : Math.min(100, Math.max(0,(temp/50)*100))) + '%';
    // Humidity bar: 0-100% range
    document.getElementById('humBar').style.width = (hum === '--' ? 0 : Math.min(100, Math.max(0,hum))) + '%';
    // Air quality bar: 0-100% range
    document.getElementById('airBar').style.width = (air === '--' ? 0 : air) + '%';

    let status = "";
    let statusClass = "";

    // Air Quality mapping (100 = good, 0 = bad)
    if (air >= 75) {
      status = "Excellent";
      statusClass = "excellent";
    } else if (air >= 50) {
      status = "Moderate";
      statusClass = "moderate";
    } else if (air >= 25) {
      status = "Poor";
      statusClass = "poor";
    } else {
      status = "Hazardous";
      statusClass = "hazardous";
    }
    document.getElementById('airStatus').textContent = status;
    document.getElementById('airBar').className = "progress " + statusClass;
  });
}
setInterval(updateData, 2000); // Fetch data every 2 seconds
window.onload = updateData;     // Fetch data on page load
</script>
</body>
</html>
)rawliteral";

/**
 * @brief Handles the root ("/") request and sends the main HTML page.
 */
void handleRoot() {
  server.send_P(200, "text/html", htmlPage); // Send HTML from PROGMEM
}

/**
 * @brief Reads sensors, calculates values, and sends data as JSON.
 */
void handleData() {
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  airQualityRaw = analogRead(MQ135PIN);

  // Print raw value for calibration
  Serial.print("Raw MQ-135: ");
  Serial.println(airQualityRaw);

  // Map the raw value to a 0-100 scale
  // USER REQUEST: Reversed logic.
  // Low raw value (airQualityMin) maps to 0 (Hazardous)
  // High raw value (airQualityMax) maps to 100 (Excellent)
  int mapped = map(constrain(airQualityRaw, airQualityMin, airQualityMax),
                   airQualityMin, airQualityMax,
                   0, 100); // REVERSED mapping: low raw value = bad air (0)
  
  mapped = constrain(mapped, 0, 100);

  // Apply exponential smoothing to the value
  airQualityValue = airQualityValue * (1 - SMOOTHING_FACTOR) + mapped * SMOOTHING_FACTOR;

  // Create JSON string
  String json = "{\"temperature\":";
  json += isnan(temperature) ? "null" : String(temperature,1);
  json += ",\"humidity\":";
  json += isnan(humidity) ? "null" : String(humidity,1);
  json += ",\"airQualityValue\":";
  json += String((int)airQualityValue); // Send the smoothed, integer value
  json += "}";

  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  
  // Note: GPIO 34 is an ADC1 pin, no need for pinMode()

  WiFi.softAP(ssid, password);
  Serial.println("Access Point started");
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
  // Sensor reads are now done "on-demand" when /data is requested.
}

