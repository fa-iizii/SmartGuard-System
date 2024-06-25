//SmartGuard-System 
/*
  SmartGuard: Integrated Intruder Detection and Environmental Monitoring System

  Description:
  This Arduino sketch integrates temperature and humidity monitoring with intruder detection
  using an ESP8266 microcontroller. It communicates sensor data to the Blynk app for real-time
  monitoring and sends alerts via email and the Blynk app and using a buzzer.

  Hardware:
  - DHT11: Temperature and Humidity Sensor
  - HC-SR04: Ultrasonic Sensor for Intruder Detection
  - ESP8266: Wi-Fi Microcontroller
  - Buzzer: For Audible Alerts

  Dependencies:
  - ESP8266 library
  - BlynkSimpleEsp8266 library
  - DHT library

  Author: faiizii
  Date: 2024-06-25
*/

// Define Blynk credentials for the combined system
#define BLYNK_TEMPLATE_ID "YOUR BLYNK TEMPLATE ID"
#define BLYNK_TEMPLATE_NAME "YOUR BLYNK TEMPLATE NAME"
#define BLYNK_AUTH_TOKEN "YOUR BLYNK AUTH TOKEN"

// Include necessary libraries for Wi-Fi, Blynk, and DHT sensor
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>

// Define output stream for serial communication
#define BLYNK_PRINT Serial

// Define constants for DHT sensor pin and type
#define DHTPIN 2
#define DHTTYPE DHT11

// Define constants for ultrasonic sensor pins and sound velocity
#define trigPin 16
#define echoPin 5
#define SOUND_VELOCITY 0.034

// Define buzzer pin (adjust based on your connection)
#define buzzerPin 14


// Define buzzer frequency (adjust for desired beep sound)
int buzzerFreq = 1000;
// Declare variables for ultrasonic sensor measurements (use with caution if modifying other code)
long duration;
int distanceCm;

// Define Wi-Fi network credentials (SSID and password)
char ssid[] = "WIFI NAME";
char pass[] = "PASSWORD";

// Create a DHT sensor object
DHT dht(DHTPIN, DHTTYPE);

// Create a Blynk timer object
BlynkTimer timer;

// Function to send sensor data monitoring temperature and humidity from the DHT sensor
void sendSensorData() {
  // Read temperature and humidity from the DHT sensor
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit

  // Check for sensor reading errors
  if (isnan(h) || isnan(t)) {
    Serial.println("[!] Failed to read from DHT sensor [!]");
    return;
  }

  // Print temperature reading to serial monitor
  Serial.println(t);

  // Send humidity and temperature values to Blynk virtual pins (assuming V6 and V5)
  Blynk.virtualWrite(V6, h);
  Blynk.virtualWrite(V5, t);

  // Print temperature and humidity with formatting to serial monitor
  Serial.print("[!] Temperature: ");
  Serial.print(t);
  Serial.print("°C, Humidity: ");
  Serial.println(h, 1); // Print humidity with 1 decimal place

  // Trigger temperature alert if temperature exceeds 20°C
  if (t > 20) {
    String alertMessage = "[ ! ]  Temperature alert: ";
    alertMessage += String(t);
    alertMessage += "°C (above 20°C) and Humidity: ";
    alertMessage += String(h, 1);
    alertMessage += "%";
    alertMessage += " :)";

    Blynk.logEvent("temp_alert", alertMessage.c_str());
  }
}

// Function to measure and send ultrasonic sensor data
void measureUltrasonicDistance() {
  // Set trigger pin to LOW for 2 microseconds
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Set trigger pin to HIGH for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Measure pulse duration from echo pin (sound wave travel time)
  duration = pulseIn(echoPin, HIGH);

  // Calculate distance using sound velocity
  distanceCm = duration * SOUND_VELOCITY / 2;

  // Send distance measurement to Blynk virtual pin (assuming V1)
  Blynk.virtualWrite(V1, distanceCm);

  // Print distance measurement to serial monitor
  Serial.print("[ ! ]  Distance (cm): ");
  Serial.println(distanceCm);

  // Trigger intruder alert if distance is less than or equal to 10 cm
  if (distanceCm >= 10) {
    Blynk.logEvent("intruder_alert", "[ ! ] Intruder Detected be Alert [ ! ]");
	
    // Generate two beeps with a short pause in between
    for (int i = 0; i < 2; i++) {
      tone(buzzerPin, buzzerFreq);
      delay(250); // Adjust delay for beep duration
      noTone(buzzerPin);
      delay(250); // Adjust delay for pause between beeps
    }
  }
}

// Setup function to initialize communication and sensors
void setup() {
  // Start serial communication for debugging purposes
  Serial.begin(115200);

  // Connect to Blynk using the combined Blynk token, SSID, and password
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  dht.begin();
  timer.setInterval(2500L, sendSensorData);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  }

void loop() {
  Blynk.run();
  timer.run();
  measureUltrasonicDistance();
  }

//SmartGuard-System 
//(c) faiizii
