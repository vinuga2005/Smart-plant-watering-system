// --- Blynk Credentials, Libraries, WiFi, Pins
#define BLYNK_TEMPLATE_ID "TMPL6SSYUUkfQ"
#define BLYNK_TEMPLATE_NAME "Smart Plant Watering System"
#define BLYNK_AUTH_TOKEN "xClEUnLaLKk0BUpAIBba1m2JEPNkuFSS"
#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <ESP32Servo.h>

char ssid[] = "POCO F4";
char pass[] = "123456788";

#define TRIG_RAIN 25
#define ECHO_RAIN 33
#define TRIG_FERT 27
#define ECHO_FERT 26
#define SOIL_PIN 34 
#define RAIN_PIN 35 
#define RELAY_RAIN 4
#define RELAY_MAIN 2
#define RELAY_FERT 15
#define SERVO_PIN 32

// This is the total time in seconds it takes our pump to empty the full fertilizer tank.
const float TOTAL_FERT_EMPTY_TIME_SECONDS = 25.96; 


// System Configuration & Thresholds
const float RAIN_TANK_HEIGHT_MM = 70.0;
const float FERT_TANK_HEIGHT_MM = 70.0;
const float SENSOR_BLIND_SPOT_MM = 20.0;
const int SOIL_DRY_THRESHOLD = 3000;
const int RAIN_DETECT_THRESHOLD = 2000;
const int LID_OPEN_ANGLE = 135;
const int LID_CLOSE_ANGLE = 0;
const int TANK_LOW_ALERT_PCT = 15;

Servo lidServo;

// Default values
int fertIntervalMins = 30;
int fertMixPct = 5;
int loopCounter = 0;
bool manualWaterSwitchIsOn = false;

// Blynk Virtual Pin Handlers
BLYNK_WRITE(V5) { 
  fertIntervalMins = param.asInt(); 
}
BLYNK_WRITE(V6) { 
  fertMixPct = param.asInt(); 
}
BLYNK_WRITE(V1) { 
  manualWaterSwitchIsOn = param.asInt(); 
}

// Manual fertilization process (time based)
BLYNK_WRITE(V2) {
  if (param.asInt() == 1) { // When switch is turned ON
    Serial.println(">> Manual Fertilize Button Pressed: Starting calibrated time-based cycle.");

    // Calculate the required pump duration based on the calibration value (TOTAL_FERT_EMPTY_TIME_SECONDS)
    // (Total time in ms / 100) gives the time for 1%. Then multiply by desired percent.
    long fertPumpDuration = (TOTAL_FERT_EMPTY_TIME_SECONDS * 1000 / 100.0) * fertMixPct;

    digitalWrite(RELAY_FERT, LOW);
    Blynk.logEvent("fert_added", "Manual fertilizer dose started..."); // gives notification from our blynk app
    delay(fertPumpDuration); // Pause for the injection
    digitalWrite(RELAY_FERT, HIGH);
    Serial.println(">> Manual Fertilize Cycle Complete.");

    Blynk.virtualWrite(V2, 0); // Turn the switch back off in the app
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("\n--- Smart Irrigation System (Calibrated) Booting Up ---");

  pinMode(TRIG_RAIN, OUTPUT);
  pinMode(ECHO_RAIN, INPUT);
  pinMode(TRIG_FERT, OUTPUT); 
  pinMode(ECHO_FERT, INPUT);
  pinMode(RELAY_RAIN, OUTPUT); 
  pinMode(RELAY_MAIN, OUTPUT);
  pinMode(RELAY_FERT, OUTPUT);

  // turn off all the output in the begining
  digitalWrite(RELAY_RAIN, HIGH); 
  digitalWrite(RELAY_MAIN, HIGH); 
  digitalWrite(RELAY_FERT, HIGH);
  // turn servo angle to lid close angle in the begining
  lidServo.attach(SERVO_PIN);
  lidServo.write(LID_CLOSE_ANGLE);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

void loop() {
  Blynk.run();
  
  // SENSOR READINGS

  // Rain tank ultra sonic sensor 
  const long SENSOR_TIMEOUT_US = 50000;
  digitalWrite(TRIG_RAIN, LOW); 
  delayMicroseconds(2); 
  digitalWrite(TRIG_RAIN, HIGH); 
  delayMicroseconds(10); 
  digitalWrite(TRIG_RAIN, LOW);
  long durationRain = pulseIn(ECHO_RAIN, HIGH, SENSOR_TIMEOUT_US);
  int rainTankLevelPct = 0;

  if (durationRain > 0) {
    float distanceRainMm = durationRain * 0.343 / 2.0;
    float totalSensorDistanceRain = RAIN_TANK_HEIGHT_MM + SENSOR_BLIND_SPOT_MM;
    float waterDepthRainMm = totalSensorDistanceRain - distanceRainMm;
    rainTankLevelPct = (waterDepthRainMm / RAIN_TANK_HEIGHT_MM) * 100;
    rainTankLevelPct = constrain(rainTankLevelPct, 0, 100);
  }

  // Fertilizer tank ultra sonic sensor
  digitalWrite(TRIG_FERT, LOW); 
  delayMicroseconds(2); 
  digitalWrite(TRIG_FERT, HIGH); 
  delayMicroseconds(10); 
  digitalWrite(TRIG_FERT, LOW);
  long durationFert = pulseIn(ECHO_FERT, HIGH, SENSOR_TIMEOUT_US);
  int fertTankLevelPct = 0;

  if (durationFert > 0) {
    float distanceFertMm = durationFert * 0.343 / 2.0;
    float totalSensorDistanceFert = FERT_TANK_HEIGHT_MM + 68.0;
    float waterDepthFertMm = totalSensorDistanceFert - distanceFertMm;
    fertTankLevelPct = (waterDepthFertMm / FERT_TANK_HEIGHT_MM) * 100;
    fertTankLevelPct = constrain(fertTankLevelPct, 0, 100);
  }
  
  // BLYNK APP AND SERIAL MONITOR SYNC

  int soilValue = analogRead(SOIL_PIN);
  int rainValue = analogRead(RAIN_PIN);

  bool isRaining = (rainValue < RAIN_DETECT_THRESHOLD);
  Serial.print("Rain Tank: " + String(rainTankLevelPct) + "%");
  Serial.print(" | Fert Tank: " + String(fertTankLevelPct) + "%");
  Serial.print(" | Soil: " + String(soilValue));
  Serial.print(" | Rain Sensor: " + String(rainValue));
  Serial.println(" | Raining?: " + String(isRaining ? "Yes" : "No"));
  Blynk.virtualWrite(V8, rainTankLevelPct);
  Blynk.virtualWrite(V7, fertTankLevelPct);
  Blynk.virtualWrite(V3, soilValue);
  Blynk.virtualWrite(V4, rainValue);

  // SYSTEM LOGIC AND ACTIONS 

  // Lid Control Logic 
  bool lidOpen = (lidServo.read() > 90);
  //lid will open only when is raining and rain water level is below 95%
  bool shouldOpenLid = (isRaining && rainTankLevelPct < 95);
  //lid will close if not raining or rain water level is above 95% (to avoid overflow)
  bool shouldCloseLid = (!isRaining || rainTankLevelPct >= 95);

  if (shouldOpenLid && !lidOpen) {
    lidServo.write(LID_OPEN_ANGLE);
    Blynk.logEvent("lid_status", "Rain detected. Lid opened.");
  } else if (shouldCloseLid && lidOpen) {
    lidServo.write(LID_CLOSE_ANGLE);
    Blynk.logEvent("lid_status", "Lid closed.");
  }

  // Watering Logic 
  if (manualWaterSwitchIsOn) {
    if (rainTankLevelPct > TANK_LOW_ALERT_PCT) {
      digitalWrite(RELAY_RAIN, LOW); 
      digitalWrite(RELAY_MAIN, HIGH);
    } else {
      digitalWrite(RELAY_RAIN, HIGH); 
      digitalWrite(RELAY_MAIN, LOW);
    }
  } 
  else {
    if (soilValue > SOIL_DRY_THRESHOLD) {
      if (rainTankLevelPct > TANK_LOW_ALERT_PCT) {
        digitalWrite(RELAY_RAIN, LOW); 
        digitalWrite(RELAY_MAIN, HIGH);
      } else {
        digitalWrite(RELAY_RAIN, HIGH); 
        digitalWrite(RELAY_MAIN, LOW);
      }
    } else {
      digitalWrite(RELAY_RAIN, HIGH); 
      digitalWrite(RELAY_MAIN, HIGH);
    }
  }

  // Automatic Fertilizer Timer
  loopCounter = loopCounter + 1;
  int loopsNeeded = (fertIntervalMins * 60) / 2;
  if (loopCounter >= loopsNeeded) {
    loopCounter = 0; // Reset the timer
    
    // Check if the tank is empty before starting the automatic cycle
    if (fertTankLevelPct > TANK_LOW_ALERT_PCT) {
      Serial.println(">> Automatic fertilizer timer is ready. Starting calibrated time-based cycle.");
      long fertPumpDuration = (TOTAL_FERT_EMPTY_TIME_SECONDS * 1000 / 100.0) * fertMixPct;
      
      digitalWrite(RELAY_FERT, LOW);
      Blynk.logEvent("fert_added", "Automatic fertilizer dose started...");
      delay(fertPumpDuration); 
      digitalWrite(RELAY_FERT, HIGH);
      Serial.println(">> Automatic fertilizer cycle complete.");
    } else {
      Serial.println(">> Automatic fertilizer cycle SKIPPED: Tank is empty.");
      Blynk.logEvent("fert_error", "Auto fertilize skipped: Fertilizer tank is empty!");
    }
  }

  // Low Level Alert Logic 
  static bool lowRainAlertSent = false, lowFertAlertSent = false;
  if (rainTankLevelPct < TANK_LOW_ALERT_PCT && !lowRainAlertSent) { 
    Blynk.logEvent("low_rainwater", "Warning: Rainwater tank is low!"); 
    lowRainAlertSent = true; 
  } 
  else if (rainTankLevelPct > TANK_LOW_ALERT_PCT) { 
    lowRainAlertSent = false; 
  }
  if (fertTankLevelPct < TANK_LOW_ALERT_PCT && !lowFertAlertSent) { 
    Blynk.logEvent("low_fertilizer", "Warning: Fertilizer tank is low!"); 
    lowFertAlertSent = true; 
  } 
  else if (fertTankLevelPct > TANK_LOW_ALERT_PCT) { 
    lowFertAlertSent = false; 
  }
  
  
  delay(2000); 
}