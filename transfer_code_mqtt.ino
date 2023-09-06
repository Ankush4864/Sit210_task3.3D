//SIT 210
//Task - 3.3D
//Name - Ankush Singla
//Student ID: 2210994864 
// Include necessary libraries for MQTT and Wi-Fi based on the board we're using
#include <ArduinoMqttClient.h>
#if defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_AVR_UNO_WIFI_REV2)
#include <WiFiNINA.h>
#elif defined(ARDUINO_SAMD_MKR1000)
#include <WiFi101.h>
#elif defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#endif

// Define our Wi-Fi network credentials
char ssid[] = "Ankush";    // our network SSID (name)
char pass[] = "12345678";   // our network password (use for WPA, or use as key for WEP)

// Define pins for the ultrasonic sensor
const int trigPin = 2;
const int echoPin = 3;

// Variables for measuring distance
float duration, distance;

// Create a Wi-Fi client and an MQTT client
WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

// Define MQTT broker information
const char broker[] = "broker.mqttdashboard.com";
int port = 1883;
const char topic[] = "SIT210/waves";

// Define the time interval for sending MQTT messages
const long interval = 1000;
unsigned long previousMillis = 0;

// Counter to keep track of the number of messages sent
int count = 0;

void setup() {
  // Initialize the serial communication for debugging
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // Attempt to connect to the Wi-Fi network
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // If Wi-Fi connection fails, keep retrying
    Serial.print(".");
    delay(5000);
  }

  Serial.println("You're connected to the network");
  Serial.println();

  // Attempt to connect to the MQTT broker
  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

  if (!mqttClient.connect(broker, port)) {
    // If MQTT connection fails, print an error message and stop
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
    while (1);
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();
}

void loop() {
  // Poll the MQTT client for incoming messages
  mqttClient.poll();

  // Get the current time
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // Save the last time a message was sent
    previousMillis = currentMillis;

    // Trigger the ultrasonic sensor to measure distance
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // Measure the duration of the pulse from the ultrasonic sensor
    duration = pulseIn(echoPin, HIGH);

    // Calculate the distance based on the duration
    distance = (duration * .0343) / 2;

    // Print the distance to the serial monitor for debugging
    Serial.print("Distance: ");
    Serial.println(distance);

    // If the distance is less than 12 units, send an MQTT message
    if (distance < 12) {
      // Begin composing an MQTT message
      mqttClient.beginMessage(topic);
      mqttClient.print("Ankush Singla : Wave is detected, ");
      mqttClient.print("Distance: ");
      mqttClient.print(distance);
      // End and send the MQTT message
      mqttClient.endMessage();
      delay(1000);
    }

    Serial.println();

    // Increment the message count
    count++;
  }
}
