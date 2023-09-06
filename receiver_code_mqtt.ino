// Include necessary libraries based on the board we're using and the network shield
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
char pass[] = "12345678";    // our network password (use for WPA, or use as key for WEP)

int light = 2;  // Pin number for controlling an LED

// Create a Wi-Fi client and an MQTT client
WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

// Define MQTT broker information
const char broker[] = "mqtt-dashboard.com";
int port = 1883;
const char topic[] = "SIT210/waves";  // MQTT topic to subscribe to

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(9600);
  pinMode(light, OUTPUT); // Set the LED pin as an output
  while (!Serial) {
    ; // wait for the serial port to connect. Needed for native USB port only
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

  Serial.print("Subscribing to topic: ");
  Serial.println(topic);
  Serial.println();

  // Subscribe to the specified MQTT topic
  mqttClient.subscribe(topic);

  Serial.print("Waiting for messages on topic: ");
  Serial.println(topic);
  Serial.println();
}

void loop() {
  int messageSize = mqttClient.parseMessage();
  if (messageSize) {
    // We received a message, print out the topic and contents
    Serial.print("Received a message with topic '");
    Serial.print(mqttClient.messageTopic());
    Serial.print("', length ");
    Serial.print(messageSize);
    Serial.println(" bytes:");

    // Use the Stream interface to print the contents of the message
    while (mqttClient.available()) {
      Serial.print((char)mqttClient.read());
    }

    // Flash the LED as a visual indicator of message reception
    digitalWrite(light, HIGH);   // Turn the LED on (HIGH is the voltage level)
    delay(200);                   // Wait for a moment
    digitalWrite(light, LOW);    // Turn the LED off by making the voltage LOW
    delay(200);                   // Wait for a moment

    // Repeat the LED flashing for a total of 3 times
    for (int i = 0; i < 2; i++) {
      digitalWrite(light, HIGH);
      delay(200);
      digitalWrite(light, LOW);
      delay(200);
    }
    
    Serial.println();
  }
}
