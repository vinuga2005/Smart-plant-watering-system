# Smart-plant-watering-system
An automated plant watering system using an ESP32, various sensors, and the Blynk IoT platform. 

This project was developed by a team of first-year students for the Fundamentals of Computing module at the Sri Lanka Institute of Information Technology (SLIIT).

📋 About The Project
The goal of this project was to create a fully autonomous system that handles all aspects of plant care: watering, fertilizing, and even collecting rain water. It solves the common problems of over/under-watering and ensures plants receive consistent care without manual intervention.

The system is designed to be both "smart" and "sustainable" by using real-time sensor data to make decisions and prioritizing the use of collected rainwater to conserve water. All system functions and sensor readings can be monitored and controlled remotely from anywhere in the world using a custom-built Blynk mobile dashboard.

## ✨ Key Features

Automated Watering: The system uses a capacitive soil moisture sensor to water the plant only when necessary.

Smart Water Source Selection: It intelligently checks the rainwater tank level with an ultrasonic sensor and prioritizes it over the main tap line.

Scheduled Fertilizing: A dedicated pump dispenses liquid fertilizer at user-defined intervals set via the Blynk app.

Automated Rainwater Collection: A rain sensor detects rainfall and triggers a servo motor to open the lid of the collection tank. The lid closes when the rain stops or the tank is full.

Remote Monitoring & Control: A complete Blynk dashboard provides live sensor data (soil moisture, tank levels) and allows for manual override of all pumps and motors.

System Alerts: The system sends notifications to the mobile app for important events, such as low rainwater or fertilizer levels.

## 🛠️ Hardware & Software

**Hardware Components**

Microcontroller: ESP32

**Sensors:**

Capacitive Soil Moisture Sensor

HC-SR04 Ultrasonic Sensor (x2)

Rain Drop Sensor

**Actuators:**

SG90 Servo Motor (for the lid)

5V Relay Modules (x3)

5V Water Pumps (x3 for Rainwater, Main Line, Fertilizer)

Other: Connecting wires, breadboard, power supply.

**Software & Platforms**

Programming Language: C++

Development Environment: Arduino IDE

IoT Platform: Blynk

## ⚙️ System Logic Flow
The ESP32 continuously reads data from the soil, ultrasonic, and rain sensors.

This data is sent to the Blynk server and displayed on the mobile dashboard.

If the soil moisture value drops below a predefined dry threshold, the watering sequence is initiated.

The system first checks the rainwater tank's ultrasonic sensor. If the water level is sufficient, it activates the rainwater pump relay.

If the rainwater level is too low, the system bypasses it and activates the main tap line pump relay instead.

The fertilizer system runs on a separate, non-blocking timer. When the interval is reached, it activates the fertilizer pump for a short, calculated duration.

The rain sensor's input is constantly monitored. If rain is detected, the servo motor opens the collection lid.

## How to Use

1.  **Automatic Mode:** The system will operate automatically based on sensor readings.
2.  **Manual Watering:** The "Water Now" switch in the Blynk app will run the watering system continuously until turned off.
3.  **Manual Fertilizing:** The "Fertilize Now" button in the Blynk app will dispense a single, calibrated dose of fertilizer.


### 🧑‍💻 Team Members

P.D Vinuga Janandith - http://www.linkedin.com/in/vinuga-janandith-873143244

Shaduskar Visvanathan - https://www.linkedin.com/in/shaduskar-visvanathan

Methul Hivin Peiris - https://www.linkedin.com/in/methul-hivin-991005345

Senethma Amavindi - https://www.linkedin.com/in/senethma-amavindi-914b15358

Karunarathna K.N.W - https://www.linkedin.com/in/kavindi-nethsadi-wickramanayaka-karunarathna-89959235a

Ahmed A L R - 







