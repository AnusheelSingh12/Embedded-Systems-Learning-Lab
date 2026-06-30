# Wi-Fi Controlled Robot Car

A Wi-Fi-enabled robotic vehicle built using the ESP32 that can be controlled through a browser-based interface without requiring an external router or mobile application.

---

# Overview

The **Wi-Fi Controlled Robot Car** is an embedded systems project that demonstrates wireless robot control using the ESP32's built-in Wi-Fi capabilities.

The ESP32 operates as a standalone Wi-Fi Access Point and hosts an embedded web server. Users can connect directly to the robot using any smartphone, tablet, or computer and control its movement through a responsive web interface.

This project demonstrates wireless communication, embedded web servers, motor control, and hardware-software integration.

---

# Demonstration

Project demonstration video:

**Coming Soon**

---

# Hardware Used

* ESP32 Development Board
* L298N Motor Driver
* DC Gear Motors
* Robot Chassis
* Li-ion Battery
* Jumper Wires

---

# Features

* ESP32 Wi-Fi Access Point
* Embedded Web Server
* Browser-Based Control Interface
* Wireless Robot Control
* Keyboard (WASD) Support
* Touch Button Support
* Forward Movement
* Reverse Movement
* Left Turn
* Right Turn
* Stop Function
* Responsive Web Interface

---

# System Architecture

```text
         Smartphone / Laptop

                 │

          Wi-Fi Connection

                 │

                 ▼

          ESP32 Access Point

                 │

          Embedded Web Server

                 │

                 ▼

          Motor Driver (L298N)

                 │

                 ▼

          Differential Drive Robot
```

---

# Working Principle

1. The ESP32 creates its own Wi-Fi network.
2. A browser connects directly to the ESP32.
3. The embedded web server serves the control interface.
4. User commands are sent to the ESP32.
5. The ESP32 controls the motor driver.
6. The robot executes the requested movement in real time.

---

# Repository Structure

```text
WiFi-Controlled-Robot-Car/

│
├── Firmware/
│   └── Robot_Car.cpp
│
├── Images/
│
├── Videos/
│
└── README.md
```

---

# Software Used

* Arduino IDE
* ESP32 Arduino Core
* HTML
* CSS
* JavaScript
* Embedded C++

---

# Embedded Concepts Demonstrated

* Embedded Systems
* Wi-Fi Communication
* Access Point Mode
* Embedded Web Server
* HTTP Request Handling
* Motor Driver Control
* Differential Drive Robotics
* Hardware–Software Integration

---

# Future Improvements

* Speed Control
* Live Camera Streaming
* Obstacle Avoidance
* Autonomous Navigation
* Mobile Application
* Battery Monitoring
* MQTT Integration

---

# Project Status

Completed

---

# Author

Anusheel Singh

Electronics and Communication Engineering

Areas of Interest

* Embedded Systems
* IoT
* Robotics
* Hardware Integration
* Control Systems
