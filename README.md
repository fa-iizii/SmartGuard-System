# SmartGuard: Integrated Intruder Detection and Environmental Monitoring System

SmartGuard is an Intruder Detection System that also monitors temperature and humidity. The system utilizes the ESP8266 microcontroller and integrates with the Blynk app to provide real-time updates and notifications. It includes a buzzer that sounds an alert when an intruder is detected, and it sends notifications through email and the Blynk app.

## Features

- **Temperature and Humidity Monitoring**: Uses a DHT11 sensor to measure temperature and humidity.
- **Intruder Detection**: Uses an HC-SR04 ultrasonic sensor to detect intruders.
- **Buzzer Alert**: Sounds a buzzer when an intruder is detected.
- **Real-Time Notifications**: Sends notifications via the Blynk app and email.
- **Blynk App Integration**: Displays real-time temperature, humidity, and intruder distance on the Blynk app.
- **Data Logging**: Maintains a history of temperature, humidity, and intruder detection data.
- **Temperature Alert**: Sends an email notification if the temperature exceeds 20°C.

## Hardware Components

- **DHT11**: Temperature and Humidity Sensor
- **HC-SR04**: Ultrasonic Sensor for Intruder Detection
- **ESP8266**: Wi-Fi Microcontroller
- **Buzzer**: For Audible Alerts

## Installation

### Prerequisites

Before you begin, ensure you have the following:

- Arduino IDE installed on your computer.
- Relevant board drivers installed in the Arduino IDE for the ESP8266.
- Libraries installed in the Arduino IDE:
  - `ESP8266WiFi`
  - `BlynkSimpleEsp8266`
  - `DHT`

### Steps

1. **Download the Code**:
   - Copy the code from `smartguard.ino` in this repository.

2. **Open Arduino IDE**:
   - Launch the Arduino IDE on your computer.

3. **Paste the Code**:
   - Create a new sketch in the Arduino IDE.
   - Paste the copied code into the new sketch.

4. **Configure Wi-Fi and Blynk Credentials**:
   - Update the following lines in the code with your Wi-Fi and Blynk credentials:
     ```cpp
     char ssid[] = "your_wifi_ssid";
     char pass[] = "your_wifi_password";
     #define BLYNK_AUTH_TOKEN "your_blynk_auth_token"
     ```

5. **Upload the Code**:
   - Connect your ESP8266 to your computer via USB.
   - Select the correct board and port in the Arduino IDE.
   - Click on the upload button to upload the code to your ESP8266.

## Usage

1. **Power the System**:
   - Connect your ESP8266 to a power source.

2. **Open the Blynk App**:
   - Monitor real-time data on the Blynk app.

3. **Receive Notifications**:
   - Get alerts on the Blynk app and email for temperature and intruder detection events.

## Code Explanation

The main functionality of the code is divided into several functions:

- **`sendPharmacySensorData`**: Reads temperature and humidity from the DHT11 sensor and sends the data to the Blynk app. Triggers a temperature alert if the temperature exceeds 20°C.
- **`measureUltrasonicDistance`**: Measures the distance using the HC-SR04 ultrasonic sensor. Sends the distance data to the Blynk app and triggers an intruder alert if an intruder is detected.
- **`setup`**: Initializes the ESP8266, connects to Wi-Fi, and sets up the sensors and Blynk timer.
- **`loop`**: Runs the Blynk and timer functions continuously to keep the system operational.

## Sample Code

```cpp
//SmartGuard-System 
//(c) faiizii

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

// Function to send sensor data for pharmacy monitoring
void sendPharmacySensorData() {
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
  timer.setInterval(2500L, sendPharmacySensorData);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  }

void loop() {
  Blynk.run();
  timer.run();
  measureUltrasonicDistance();
  }

```
For more details, please refer to the full code in the repository.

### Notes:

- Replace `smartguard.ino` with the actual name of your Arduino sketch file.
- Ensure the Markdown formatting is correct, especially with code blocks and lists.
- Customize the Wi-Fi credentials and other configurations as per your setup.
- Mention any specific details or instructions relevant to your project in the appropriate sections of the `README.md` file.
