#include <WiFi.h>
#include <Wire.h>

const char* ssid = "Dialog 4G 516";
const char* password = "MjXDdfXT";

WiFiServer server(80);
float temperature = 0.0; // Declare temperature variable globally
float humidity = 0.0;    // Declare humidity variable globally

void setup() {
  Serial.begin(9600);
  Wire.begin(8);               // Join I2C bus as slave with address 8
  Wire.onReceive(receiveEvent); // Register receive event
  Wire.onRequest(requestEvent); // Register request event

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());
}

void loop() {
  WiFiClient client = server.available(); // Check for incoming clients
  if (client) {
    Serial.println("New Client.");
    String currentLine = ""; // make a String to hold incoming data from the client
    while (client.connected()) { // loop while the client's connected
      if (client.available()) {  // if there's bytes to read from the client
        char c = client.read(); // read a byte, then
        Serial.write(c); // print it out the serial monitor
        if (c == '\n') { // if the byte is a newline character
          if (currentLine.length() == 0) { // if the current line is blank, you got two newline characters in a row
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // Send HTML web page with received values, temperature, and humidity
            client.print("<!DOCTYPE html><html><head><title>Arduino Values</title></head><body>");
            client.print("<p>Temperature: ");
            client.print(temperature);
            client.print(" &deg;C</p><p>Humidity: ");
            client.print(humidity);
            client.println("%</p></body></html>");

            break; // Break out of the while loop
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') { // if you got anything else but a carriage return character, add it to the end of the currentLine
          currentLine += c;
        }
      }
    }
    delay(10); // Delay to allow ESP to process incoming data
    client.stop(); // Close the connection
    Serial.println("Client disconnected.");
  }
}

// Function that executes whenever data is received from master
void receiveEvent(int howMany) {
  if (Wire.available() >= 2 * sizeof(float)) { // Check if there's enough data for two floats
    Wire.readBytes((uint8_t*)&temperature, sizeof(float)); // Read temperature as a float
    Wire.readBytes((uint8_t*)&humidity, sizeof(float));    // Read humidity as a float
  }
}

// Function to respond to master's request
void requestEvent() {
  Wire.write("Received"); // Respond with message as expected by master
}
