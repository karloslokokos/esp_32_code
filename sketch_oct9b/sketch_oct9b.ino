// ESP32 code to read ultrasonic sensor and send data to Arduino

const int triggerPin = 11; // Trigger pin of the ultrasonic sensor
const int echoPin = 10;    // Echo pin of the ultrasonic sensor

void setup() {
  Serial.begin(9600);  // Initialize serial communication
  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  long duration;
  int distance;

  // Trigger the ultrasonic sensor
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);

  // Read the echo pulse duration
  duration = pulseIn(echoPin, HIGH);

  // Calculate distance in centimeters
  distance = duration * 0.034 / 2;

  // Send distance data to Arduino over Serial
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  delay(1000); // Delay for 1 second before the next reading
}
