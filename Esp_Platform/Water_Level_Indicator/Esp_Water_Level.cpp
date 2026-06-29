// Water Level Indicator
// Author: Anusheel Singh

const int sensorPin = A0;

const int lowLED = 8;
const int mediumLED = 9;
const int highLED = 10;

int sensorValue = 0;

void setup() {
  pinMode(lowLED, OUTPUT);
  pinMode(mediumLED, OUTPUT);
  pinMode(highLED, OUTPUT);

  Serial.begin(9600);
}

void loop() {

  sensorValue = analogRead(sensorPin);

  Serial.print("Water Level: ");
  Serial.println(sensorValue);

  if (sensorValue < 300) {
    // Low Water Level
    digitalWrite(lowLED, HIGH);
    digitalWrite(mediumLED, LOW);
    digitalWrite(highLED, LOW);
  }

  else if (sensorValue >= 300 && sensorValue < 600) {
    // Medium Water Level
    digitalWrite(lowLED, LOW);
    digitalWrite(mediumLED, HIGH);
    digitalWrite(highLED, LOW);
  }

  else {
    // High Water Level
    digitalWrite(lowLED, LOW);
    digitalWrite(mediumLED, LOW);
    digitalWrite(highLED, HIGH);
  }

  delay(500);
}