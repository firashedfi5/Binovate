#include <WiFi.h>
#include <WebServer.h>

// Replace with your network credentials
const char* ssid = "POCO X3 Pro";
const char* password = "firasHEDFI03267";

WebServer server(80);  // Start the server on port 80

// Function to handle the float value sent to the ESP32
void handleData() {
  if (server.hasArg("value")) {
    String receivedValue = server.arg("value");
    //float receivedValue = floatStr.toFloat();  // Convert to float
    
    // Print the received value in the Serial Monitor
    Serial.print("Received Value: ");
    Serial.println(receivedValue);
  }
  server.send(200, "text/plain", "Value received");
}

void setup() {
  Serial.begin(115200);  // Initialize serial communication
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
  Serial.println(WiFi.localIP());

  server.on("/send", handleData);  // Define route
  server.begin();
}

void loop() {
  server.handleClient();
}
