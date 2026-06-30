# Gesture Controlled Robotic Car

A wireless gesture-controlled robotic car built using an ESP8266-based handheld controller and an ESP32-powered robot. The system uses an MPU6050 accelerometer to detect hand movements and ESP-NOW for low-latency wireless communication, enabling intuitive real-time vehicle control.

---

## Project Overview

This project demonstrates wireless embedded communication, sensor interfacing, motor control, and robotics using the ESP ecosystem.

Instead of traditional joysticks, the robot is controlled by simply tilting a handheld remote. The MPU6050 senses the orientation of the controller, converts it into movement commands, and transmits them wirelessly using ESP-NOW. The ESP32 mounted on the robot receives these commands and drives the motors accordingly.

---

## Demonstration

Project demonstration video:

**Coming Soon**

---

## Hardware

### Gesture Remote

- ESP8266
- MPU6050 Accelerometer/Gyroscope
- Li-ion Battery / USB Power

### Robotic Car

- ESP32 Development Board
- L298N Motor Driver
- DC Gear Motors
- Robot Chassis
- Li-ion Battery Pack
- Passive Buzzer

---

## Features

- Gesture-based wireless control
- ESP-NOW communication
- Real-time movement response
- Forward
- Reverse
- Left Turn
- Right Turn
- Automatic emergency stop on signal loss
- Startup buzzer sequence
- Low-latency communication

---

## System Architecture

```text
           Hand Gesture

                 │

                 ▼

        MPU6050 Accelerometer

                 │

                 ▼

          ESP8266 Controller

                 │

          ESP-NOW Wireless

                 │

                 ▼

            ESP32 Receiver

                 │

           Motor Driver

                 │

                 ▼

          Differential Drive Robot
```

---

## Control Mapping

| Gesture | Command |
|----------|----------|
| Tilt Forward | Move Forward |
| Tilt Backward | Reverse |
| Tilt Left | Turn Left |
| Tilt Right | Turn Right |
| Neutral Position | Stop |

---

## Repository Structure

```
Gesture-Controlled-Car/

│
├── Car/
│   └── ESP32 Firmware
│
├── Remote/
│   └── ESP8266 Firmware
│
├── Images/
│
├── Videos/
│
└── README.md
```

---

## Software Used

- Arduino IDE
- ESP32 Arduino Core
- ESP8266 Arduino Core
- ESP-NOW
- MPU6050 Library

---

## Embedded Concepts Demonstrated

- Wireless Embedded Systems
- ESP-NOW Communication
- Sensor Interfacing
- I2C Communication
- Motion Detection
- Differential Drive Control
- PWM Motor Control
- Robotics
- Real-Time Embedded Programming
- Safety Fail-safe Mechanism

---

## Future Improvements

- Adjustable speed control
- OLED status display
- Battery monitoring
- Obstacle avoidance
- PID motor control
- Autonomous mode
- Mobile application
- Long-range communication

---

## Project Status

Completed

---

## Author

Anusheel Singh

Electronics and Communication Engineering

Areas of Interest

- Embedded Systems
- Robotics
- IoT
- Control Systems
- Hardware Integration