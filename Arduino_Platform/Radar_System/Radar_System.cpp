#include <Servo.h>

// Pin definitions
#define trigPin 8
#define echoPin 9
#define greenLED 3
#define blueLED 4
#define redLED 5
#define buzzer 6
#define servoPin 10

Servo turretServo;
int angle = 0;          // current servo angle
int direction = 1;      // 1 = forward, -1 = backward
bool targetLocked = false;

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(blueLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(buzzer, OUTPUT);

  turretServo.attach(servoPin);
  Serial.begin(9600);
}

// Measure distance with ultrasonic
long getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000); // timeout 30ms (~5m)
  return duration * 0.034 / 2; // distance in cm
}

void loop() {
  if (!targetLocked) {
    digitalWrite(greenLED, HIGH);  // scanning

    turretServo.write(angle);
    delay(20);  // controls sweep speed

    long distance = getDistance();
    Serial.print("Angle: ");
    Serial.print(angle);
    Serial.print("° | Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    // Detect target within 15 cm
    if (distance > 0 && distance <= 15) {
      targetLocked = true;

      // Target locked
      digitalWrite(greenLED, LOW);
      digitalWrite(blueLED, HIGH);
      Serial.println("Target Locked!");
      delay(2000);

      // Missile launch
      digitalWrite(blueLED, LOW);
      digitalWrite(redLED, HIGH);
      digitalWrite(buzzer, HIGH);
      Serial.println("Missile Launched!");
      delay(1000);
      digitalWrite(buzzer, LOW);
      digitalWrite(redLED, LOW);

      // Reset target lock
      targetLocked = false;
    }

    // Update sweep direction
    angle += direction;
    if (angle >= 170 || angle <= 0) {
      direction *= -1;
    }
  }
}