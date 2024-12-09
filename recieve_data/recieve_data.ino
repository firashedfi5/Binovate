#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

// Servo motors configuration
Servo servo1;
Servo servo2;
const int servo1Pin = 13;
const int servo2Pin = 27;

// GPIO pins for LEDs
const int GreenLedPin = 15;
const int RedLedPin = 16;

// Stepper motor configuration
const int DIR_PIN = 12;
const int STEP_PIN = 14;
const int steps_per_rev = 200;  // 200 steps for a full revolution (1 full rotation = 360 degrees)
int currentPosition = 0;  // This will keep track of the current position of the stepper motor (relative to initial)

// Variable to track if the stepper motor has rotated
bool hasRotated = false;

// Wi-Fi credentials
const char* ssid = "POCO X3 Pro";
const char* password = "firasHEDFI03267";

// Web server setup
WebServer server(80);
String receivedValue = "";

// Function to handle incoming data via the server
void handleData() {
  if (server.hasArg("value")) {
    receivedValue = server.arg("value");

    // Print the received value in the Serial Monitor
    Serial.print("Received Value: ");
    Serial.println(receivedValue);
  }
  server.send(200, "text/plain", "Value received");
}

// Function to rotate the stepper motor
void rotateStepper(int steps, bool direction) {
  digitalWrite(DIR_PIN, direction); // Set rotation direction
  for (int i = 0; i < steps; i++) {
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(800); // Adjust delay for speed control
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(800); 
  }
  // Update the current position
  currentPosition = (currentPosition + (direction ? steps : -steps)) % steps_per_rev;
  if (currentPosition < 0) {
    currentPosition += steps_per_rev;  // To keep the position in a positive range
  }
  Serial.print("Current Position: ");
  Serial.println(currentPosition);
}

// Function to rotate a servo motor to a specific angle
void rotateServo(Servo &servo, int angle) {
  servo.write(angle);
}

void setup() {
  Serial.begin(115200); // Initialize serial communication

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
  Serial.println("IP Address: " + WiFi.localIP().toString());

  // Setup web server
  server.on("/send", handleData);
  server.begin();

  // Initialize servo motors
  servo1.attach(servo1Pin);
  servo2.attach(servo2Pin);
  servo1.write(180); // Set initial position to 180°
  servo2.write(180); // Set initial position to 180°

  // Initialize LEDs
  pinMode(GreenLedPin, OUTPUT);
  pinMode(RedLedPin, OUTPUT);
  digitalWrite(GreenLedPin, HIGH); // Green LED off by default
  digitalWrite(RedLedPin, LOW); // Red LED on by default

  // Initialize stepper motor
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);

  // Set initial position to "0"
  currentPosition = 0;  // We set the initial position as 0
}

void loop() {
  server.handleClient(); // Handle web server requests

  // Determine steps and direction based on received value
  int steps = 0;
  bool direction = true; // Default to clockwise rotation

  // Assign specific rotation based on received value
  if (receivedValue == "glass") {
    steps = 50; // Rotate to 0° (Glass bin)
  } else if (receivedValue == "paper") {
    steps = 100; // Rotate to 90° (Paper bin)
  } else if (receivedValue == "plastic") {
    steps = 150; // Rotate to 180° (Plastic bin)
  } else if (receivedValue == "trash") {
    steps = 200; // Rotate to 270° (Trash bin)
  }

  // Rotate the stepper motor if steps are determined
  if (steps > 0) {
    // First, rotate the stepper motor to the initial position (0°)
    if (currentPosition != 0) {
      int initialSteps = (0 - currentPosition + steps_per_rev) % steps_per_rev;
      direction = initialSteps > 0; // If steps > 0, rotate clockwise
      rotateStepper(initialSteps, direction);
      delay(500);  // Wait a bit before moving to the next position
    }

    // Then, rotate to the desired position
    int targetSteps = (steps - currentPosition + steps_per_rev) % steps_per_rev;
    direction = targetSteps > 0;  // If steps > 0, rotate clockwise
    rotateStepper(targetSteps, direction);
    hasRotated = true; // Set flag to true if the motor rotates
    receivedValue = ""; // Reset received value to prevent multiple triggers
  }

  // Trigger servo motors after stepper motor has moved
  if (hasRotated) {
    delay(5000); // Wait 5 seconds before rotating 90°
    rotateServo(servo1, 90); // Rotate back to 90° for both servos
    rotateServo(servo2, 90);
    digitalWrite(GreenLedPin, LOW); // Green LED off by default
    digitalWrite(RedLedPin, HIGH);

    delay(5000); // Wait 5 seconds before rotating back to initial position
    rotateServo(servo1, 180); // Rotate back to 180° for both servos
    rotateServo(servo2, 180);
    digitalWrite(GreenLedPin, HIGH); // Green LED off by default
    digitalWrite(RedLedPin, LOW);

    hasRotated = false; // Reset flag
  }
}
