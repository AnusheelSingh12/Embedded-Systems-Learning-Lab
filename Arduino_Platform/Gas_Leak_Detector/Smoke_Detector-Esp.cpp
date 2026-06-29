// --- Pin Definitions ---
const int MQ5_SENSOR_PIN = A0;   // Analog pin connected to the MQ-5 sensor's A0 output
const int RED_LED_PIN = D1;      // GPIO5
const int GREEN_LED_PIN = D2;    // GPIO4
const int BUZZER_PIN = D5;       // GPIO14

// --- Calibration ---
// IMPORTANT: You must calibrate this value!
// 1. Upload the code and open the Serial Monitor.
// 2. Let the sensor warm up for a minute in clean air.
// 3. Note the "Sensor Value" reading. This is your baseline.
// 4. Set the GAS_THRESHOLD to a value slightly higher than the baseline (e.g., if your baseline is 150, try 250 or 300).
int GAS_THRESHOLD = 550;

// --- Setup Function: Runs once when the ESP8266 starts ---
void setup() {
  // Start serial communication for debugging
  Serial.begin(9600);
  
  // Set the pin modes for our output components
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Initial state: Green ON, Red OFF
  digitalWrite(GREEN_LED_PIN, HIGH);
  digitalWrite(RED_LED_PIN, LOW);
  
  // --- Sensor Warm-up ---
  Serial.println("MQ-5 Sensor is warming up. Please wait for about 20-30 seconds...");
  // A delay to allow the sensor's internal heater to stabilize
  delay(20000); 
  Serial.println("Sensor is ready!");
}

// --- Loop Function: Runs repeatedly ---
void loop() {
  // Read the analog value from the gas sensor
  int sensorValue = analogRead(MQ5_SENSOR_PIN);

  // Print the value to the Serial Monitor for debugging and calibration
  Serial.print("Sensor Value: ");
  Serial.println(sensorValue);

  // Check if the sensor value has exceeded the threshold
  if (sensorValue > GAS_THRESHOLD) {
    // --- GAS DETECTED! DANGER! --- 🚨
    Serial.println("!!! GAS DETECTED !!!");
    
    digitalWrite(RED_LED_PIN, HIGH);   // Turn on the red LED
    digitalWrite(GREEN_LED_PIN, LOW);  // Turn off the green LED
    tone(BUZZER_PIN, 1500);            // Make the buzzer sound at a 1500 Hz tone
    
  } else {
    // --- SAFE CONDITION --- ✅
    Serial.println("Air quality is safe.");
    
    digitalWrite(RED_LED_PIN, LOW);    // Turn off the red LED
    digitalWrite(GREEN_LED_PIN, HIGH); // Turn on the green LED
    noTone(BUZZER_PIN);                // Stop the buzzer
  }

  // Wait for half a second before taking the next reading
  delay(500);
}