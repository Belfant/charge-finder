#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino.h>
#include "esp_sleep.h"

// Battery PIN - used to read voltage in the LiPo battery
#define BATTERY_PIN A13

// Network credentials
const char* ssid = "Your WiFi SSID";
const char* password = "WiFI SSID Password";
const char* serverName = "Flask server end point";

// defines pin numbers on the ESP32 board
const int trigPin1 = 17;
const int trigPin2 = 21; 
const int SensorOneEchoPin = 27;
const int SensorTwoEchoPin = 12;

const int ledPin = LED_BUILTIN;

// Global variables to save states during sleep
RTC_DATA_ATTR int lastStateSensor1 = -1;
RTC_DATA_ATTR int lastStateSensor2 = -1;

// defines variables
long duration1, duration2;
int distance1, distance2;
const int carPresenceThreshold = 40; // Distance in cm to check for a car

float readBattery() {
  // returns the battery percetage - sort of, the relationship is not enterily linear 
  float maxVoltage = 4.2;
  float minVoltage = 3.1;
  int raw = analogRead(BATTERY_PIN);
  float battery_voltage = (raw / 4095.0) * 3.3 * 2; // Multiply by 2 because of the inbuilt voltage divider on the battery pin

  // convert the voltage to a percentage 
  float percentage = (battery_voltage - minVoltage) / (maxVoltage - minVoltage) * 100.0;
  percentage = constrain(percentage, 0, 100);

  return percentage;
}

void initWifi() {
  unsigned long startAttemptTime = millis();
  const unsigned long wifiTimeout = 45000; // 45 seconds in milliseconds before timout

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - startAttemptTime > wifiTimeout) {
      Serial.println("Failed to connect to Wi-Fi: Timout reach. Restarting...");
      ESP.restart();
      return; // Ensure that no further code is run after the restart command
    }
    Serial.print(".");
    delay(1000);
  }
  // Use this for debugging
  Serial.println();
  Serial.println("Wi-Fi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void updateUtils(float batteryPercentage) {

  // For layout purpose on the app
  int roundedBatteryPercentage = round(batteryPercentage);

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("Flask web server URL"); // Replace with your server's actual URL
    http.addHeader("Content-Type", "application/json");

    // Compose the JSON payload. Signal strength is set to a fixed value of 3, temporarily.
    String payload = "{\"battery\": " + String(roundedBatteryPercentage) + ", \"signalStrength\": 3}";

    // Debug
    Serial.println("Sending payload: " + payload);

    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0) {
      // For debugging: 
      // String response = http.getString();
      // Serial.println(response);
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}

void setup() {
  
  // Set pin modes
  pinMode(trigPin1, OUTPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(SensorOneEchoPin, INPUT);
  pinMode(SensorTwoEchoPin, INPUT);

  pinMode(ledPin, OUTPUT); // Initialize the LED pin as an output
  // Set baud rate
  Serial.begin(9600);

  bool stateChanged1 = false;
  bool stateChanged2 = false;
  
  // Check the cause of the wakeup
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }

  // Sensor reading - check if a state change is registered
  stateChanged1 |= sensor_reading_1();
  delay(50); // Wait for 50 milliseconds before reading the next sensor

  // Sensor reading - check if a state change is registered
  stateChanged2 |= sensor_reading_2();
   delay(50); // Additional delay to prevent any possible interference

  if (stateChanged1 || stateChanged2) {
    digitalWrite(ledPin, HIGH); // Turn on the LED - used as a signal to show if any changes were registrered
    Serial.println("LED is on"); // Log to the serial monitor
    delay(2000); // Wait for a second
    
    digitalWrite(ledPin, LOW); // Turn off the LED
    Serial.println("LED is off"); // Log to the serial monitor
    delay(2000); // Wait for a second
    // Connect to Wi-Fi
     initWifi();
    if (stateChanged1) { // If sensor 1 state was changed
      sendStatus(1, distance1 <= carPresenceThreshold);
      }
    
    if (stateChanged2) { // If sensor 2 state was changed
      sendStatus(2, distance2 <= carPresenceThreshold);
    
  }

  // update on the battery level as well
  float batteryPercentage = readBattery();
  updateUtils(batteryPercentage);
  Serial.print("Battery level: ");
  Serial.print(batteryPercentage);
  Serial.println("%");

  // Disconnect the WiFI before going to sleep, otherwise the board will be woken up by it
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  }

  // Go to deep sleep regardless of state change - Change this interval to fit your need
  ESP.deepSleep(10e6);
}

bool sensor_reading_1() {
  long totalDistance = 0;
  const int numReadings = 5;

  for (int i = 0; i < numReadings; i++) {
    digitalWrite(trigPin1, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin1, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin1, LOW);
    duration1 = pulseIn(SensorOneEchoPin, HIGH);
    totalDistance += duration1 * 0.034 / 2;
    delay(50); // Short delay between readings
  }

  // Calculate the average distance
  distance1 = totalDistance / numReadings;
  Serial.println("");
  Serial.print("Sensor 1 Distance: ");
  Serial.println(distance1);

  // Determine if there's a change in the state
  bool isCarPresent = (distance1 <= carPresenceThreshold);
  bool stateChanged = isCarPresent != (lastStateSensor1 > 0); // Check if state changed
  lastStateSensor1 = isCarPresent ? 1 : 0; // Update last state
  return stateChanged; // Return true if state changed, false otherwise
}


bool sensor_reading_2() {
  long totalDistance = 0;
  const int numReadings = 5;

  for (int i = 0; i < numReadings; i++) {
    digitalWrite(trigPin2, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin2, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin2, LOW);
    duration2 = pulseIn(SensorTwoEchoPin, HIGH);
    totalDistance += duration2 * 0.034 / 2;
    delay(50); // Short delay between readings
  }

  // Calculate the average distance
  distance2 = totalDistance / numReadings;
  Serial.println("");
  Serial.print("Sensor 2 Distance: ");
  Serial.println(distance2);

  // Determine if there's a change in the state
  bool isCarPresent = (distance2 <= carPresenceThreshold);
  bool stateChanged = isCarPresent != (lastStateSensor2 > 0); // Check if state changed
  lastStateSensor2 = isCarPresent ? 1 : 0; // Update last state
  return stateChanged; // Return true if state changed, false otherwise
}

void sendStatus(int sensorNumber, bool isWithinThreshold) {
  if(WiFi.status() == WL_CONNECTED) {
    
    HTTPClient http;
    http.begin(serverName);
    http.addHeader("Content-Type", "application/json");

    // Compose the JSON payload based on the distance measured by the sensor
    // The car is considered "occupied" if it is within the threshold distance
    String payload = "{\"sensor\": \"sensor" + String(sensorNumber) + "\", \"occupied\": " + (isWithinThreshold ? "true" : "false") + "}";

    // Debug
    Serial.println("Sending payload: " + payload);

    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0) {
      
      // For debugging: 
      // String response = http.getString();
      // Serial.println(response);
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    }
    else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  }
  else {
    Serial.println("WiFi Disconnected");
  }
}

void loop() {
 // Empty, everything is done in the setup function
 // ToDo: Wifi signal strenght 
}
