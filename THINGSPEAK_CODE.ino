#include <DHT11.h>
#include <WiFi.h>
#include "ThingSpeak.h"

// Wi-Fi credentials
const char* ssid = " ";        // Replace with your Wi-Fi SSID
const char* password = " "; // Replace with your Wi-Fi password

// ThingSpeak channel details
unsigned long channelID = 2XXXXXX;     // Replace with your ThingSpeak Channel ID
const char* writeAPIKey = "2Rxxxxxxxxxx"; // Replace with your ThingSpeak Write API Key

// Create an instance of the DHT11 class.
DHT11 dht11(15); // Connect the sensor to Digital I/O Pin 2 (or adjust according to your pin)

// WiFi client
WiFiClient client;

// Pin for the Red and Green LEDs
const int redLedPin = 2;   // GPIO 13 for Red LED (use an appropriate pin)
const int greenLedPin = 4; // GPIO 12 for Green LED (use an appropriate pin)

// Variable to store the temperature and humidity
int temperature = 0;
int humidity = 0;

void setup() {
  // Initialize serial communication for debugging and monitoring.
  Serial.begin(9600);

  // Set Red and Green LED pins as outputs
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);

  // Connect to Wi-Fi network
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");

  // Initialize ThingSpeak
  ThingSpeak.begin(client);
}

void loop() {
  // Attempt to read the temperature and humidity from the DHT11 sensor
  int result = dht11.readTemperatureHumidity(temperature, humidity);

  // Check if the read was successful
  if (result == 0) {
    // Print the temperature and humidity to the serial monitor
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print(" °C\tHumidity: ");
    Serial.print(humidity);
    Serial.println(" %");

    // Send data to ThingSpeak
    ThingSpeak.setField(2, temperature);  // Send temperature to field 1
    ThingSpeak.setField(3, humidity);     // Send humidity to field 2

    // Write the data to ThingSpeak
    int responseCode = ThingSpeak.writeFields(channelID, writeAPIKey);

    // Check if the data was successfully sent
    if (responseCode == 200) {
      Serial.println("Data sent to ThingSpeak successfully");
    } else {
      Serial.println("Error sending data to ThingSpeak");
    }

    // Temperature check to control LED state
    if (temperature > 30) {
      // Temperature is greater than 30°C, turn on the Red LED, turn off the Green LED
      digitalWrite(redLedPin, HIGH);
      digitalWrite(greenLedPin, LOW);
    } if (temperature < 30) {
      // Temperature is less than or equal to 30°C, turn on the Green LED, turn off the Red LED
      digitalWrite(redLedPin, LOW);
      digitalWrite(greenLedPin, HIGH);
    }
  } else {
    // Print error message if the sensor reading failed
    Serial.println(DHT11::getErrorString(result));
  }

  // Wait for 20 seconds before sending the next reading
  delay(15000);  // Delay of 20 seconds
}