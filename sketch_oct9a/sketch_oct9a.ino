#include <WiFi.h>
#include <SPI.h>
#include <Wire.h>

// Constants
const char* WIFI_SSID = "YourWiFiSSID";
const char* WIFI_PASSWORD = "YourWiFiPassword";
const char* RASPI_IP = "RasPiIPAddress";
const int RASPI_PORT = 12345;

// Ultrasonic sensor configuration
const int triggerPin = 4; // GPIO4
const int echoPin = 5;    // GPIO5

WiFiClient client;

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to Wi-Fi");

  // Ultrasonic sensor pins
  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  // Measure distance using the ultrasonic sensor
  long duration;
  float distance;

  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2; // Calculate distance in centimeters

  // Convert the float distance value to a byte array
  byte distanceBytes[sizeof(float)];
  memcpy(distanceBytes, &distance, sizeof(float));

  // Create and send the packet
  byte packetType = 0x01;
  int payloadLength = sizeof(float);

  byte checksum = packetType;
  checksum ^= (payloadLength >> 8) & 0xFF;
  checksum ^= payloadLength & 0xFF;
  for (int i = 0; i < payloadLength; i++) {
    checksum ^= distanceBytes[i];
  }

  Serial.print("Distance: ");
  Serial.println(distance);

  if (client.connect(RASPI_IP, RASPI_PORT)) {
    client.write(packetType);
    client.write(payloadLength >> 8);
    client.write(payloadLength & 0xFF);
    client.write(checksum);
    client.write(distanceBytes, payloadLength);
    client.stop();
  }

  delay(1000); // Adjust the data transmission rate as needed
}

