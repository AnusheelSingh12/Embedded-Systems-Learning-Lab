#include <Servo.h>
Servo myServo;

// Pin assignments
const int flamePin = 2;      // Flame sensor digital output
const int relayPin = 3;      // Relay for water pump
const int greenLED = 6;      // Green LED pin
const int redLED = 7;        // Red LED pin
const int buzzerPin = 8;     // Passive buzzer pin
const int servoPin = 9;      // Servo signal pin

// Averaging settings
const int numReadings = 10;
int readings[numReadings];
int readIndex = 0;
int total = 0;
int average = 0;

int currentPos = 0;
bool sweeping = true;
bool fireActed = false;

void setup() {
  pinMode(flamePin, INPUT);
  pinMode(relayPin, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  myServo.attach(servoPin);

  for (int i = 0; i < numReadings; i++) {
    readings[i] = HIGH; // Assume no fire at start
    total += readings[i];
  }
}

void playSuccessTone() {
  tone(buzzerPin, 262, 200); delay(250); // C4
  tone(buzzerPin, 330, 150); delay(200); // E4
  tone(buzzerPin, 392, 200); delay(250); // G4
  noTone(buzzerPin);
}

void loop() {
  // Read and average the flame sensor input
  total -= readings[readIndex];
  readings[readIndex] = digitalRead(flamePin);
  total += readings[readIndex];
  readIndex = (readIndex + 1) % numReadings;
  average = total / numReadings;

  // Flame sensor is "active LOW"
  if (average == LOW) { // Fire detected
    sweeping = false;
    fireActed = true;
    digitalWrite(relayPin, HIGH);    // Water pump ON
    digitalWrite(redLED, HIGH);      // Red LED ON
    digitalWrite(greenLED, LOW);     // Green LED OFF
    tone(buzzerPin, 2000);           // Buzzer ON
    // Hold servo at current position
    myServo.write(currentPos);
  } else { // No flame detected
    digitalWrite(relayPin, LOW);     // Water pump OFF
    digitalWrite(redLED, LOW);       // Red LED OFF
    digitalWrite(greenLED, HIGH);    // Green LED ON
    noTone(buzzerPin);
    // Play tone only when fire just extinguished (once)
    if (fireActed) {
      playSuccessTone();
      fireActed = false;
    }
    sweeping = true;
  }

  // Sweep servo smoothly while in sentry mode
  if (sweeping) {
    currentPos++;
    if (currentPos > 180) currentPos = 0;
    myServo.write(currentPos);
    delay(25); // Increase for slower, decrease for faster sweep
  }
}
