#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "VM7604111";  // Replace with your Wi-Fi credentials
const char* password = "c5hFmNnrmfhn";
const char* mqtt_server = "192.168.0.43";  // Replace with your MQTT broker's IP
const char* deviceID = "ESP32_6"; 

const char* distanceTopic = "ultrasonic_distance_6";  // MQTT topic for distance data
const char* motionTopic = "Esp32_6_triggered";        // MQTT topic for motion detection
const char* topic = "sensor_data";                  // MQTT topic for other sensor data
const int trigPin = 10;
const int echoPin = 11;
const int MAX_DISTANCE = 90;

// Define sound speed in cm/uS
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

long duration;
float distanceCm;
float distanceInch;
bool beamBroken = false;  // Variable to track the beam state

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long previousMillis = 0;
const long interval = 1;  // Set the interval for sensor readings (10 microseconds)

void setup() {
  Serial.begin(115200);      // Starts the serial communication
  pinMode(trigPin, OUTPUT);  // Sets the trigPin as an Output
  pinMode(echoPin, INPUT);   // Sets the echoPin as an Inut
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void setup_wifi() {
  delay(100);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

String clientID = "ESP32Client_" + String(deviceID);

    if (client.connect(clientID.c_str())) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  unsigned long currentMillis = millis();

  // Check if it's time to read the sensor and publish data
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Clears the trigPin
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    // Sets the trigPin on HIGH state for 10 microseconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);

    // Calculate the distance
    distanceCm = duration * SOUND_SPEED / 2;

    // Convert to inches
    distanceInch = distanceCm * CM_TO_INCH;

    // Prints the distance in the Serial Monitor
    Serial.print("Distance (cm): ");
    Serial.println(distanceCm);

    // Publish distance data to MQTT
    if (client.publish(distanceTopic, String(distanceCm).c_str())) {
      Serial.println("Distance data published.");
    } else {
      Serial.println("Failed to publish distance data.");
    }

    // Check if the beam is broken
    if (distanceCm < MAX_DISTANCE) {
      if (!beamBroken) {
        // Publish a "Motion detected" message to MQTT when the beam is first broken
        if (client.publish(motionTopic, "A3")) {
          Serial.println("note_6_detected, Message sent.");
          beamBroken = true;
        } else {
          Serial.println("Motion detection message failed to send.");
        }
      }
    } else {
      beamBroken = false;  // Reset the beam state when it's not broken
    }
  }

  delay(500);
}
