/*
 * Simple SinricPro Switch Example:
 * - Connects to WiFi and SinricPro server
 * - Controls an LED with a button and SinricPro app
 */

#include <Arduino.h>               // Include core Arduino library
#include <WiFi.h>                  // Include WiFi library for ESP32 
#include "SinricPro.h"             // Include SinricPro library for cloud-based IoT control
#include "SinricProSwitch.h"       // Include SinricPro Switch device library

// WiFi and SinricPro credentials
#define WIFI_SSID     " "
#define WIFI_PASS     " "
#define APP_KEY       "2627c705"
#define APP_SECRET    "3f6a77"
#define SWITCH_ID     "67"
#define BAUD_RATE     115200

// Pin definitions
#define BUTTON_PIN 0                         // Define button pin (inverted: LOW = pressed)
#define LED_PIN   2                          // Define LED pin (inverted: LOW = ON)

bool deviceState = false;                    // Boolean to store device power state (on/off)
unsigned long lastPress = 0;                 // Variable to store last button press time

// Callback to handle power state requests from SinricPro
bool onPowerState(const String &deviceId, bool &state) {  
  Serial.printf("Device %s turned %s\r\n", deviceId.c_str(), !state ? "on" : "off"); // Print the device ID and new state
  deviceState = !state;                        // Update local device state
  digitalWrite(LED_PIN, deviceState ? LOW : HIGH); // Set LED based on state (LOW = on, HIGH = off)
  return true;                                // Return true to indicate successful handling
}

// Function to check if the button was pressed and send state to SinricPro
void handleButtonPress() {
  unsigned long now = millis();               // Get current time in milliseconds
  if (digitalRead(BUTTON_PIN) == LOW && (now - lastPress > 1000)) {  // If button is pressed and debounce time has passed
    deviceState = !deviceState;               // Toggle device state (on/off)
    digitalWrite(LED_PIN, deviceState ? LOW : HIGH); // Update LED based on new state

    SinricProSwitch& mySwitch = SinricPro[SWITCH_ID]; // Get the SinricPro Switch device by its ID
    if (!mySwitch.sendPowerStateEvent(deviceState)) { // Send updated state to SinricPro server
      Serial.println("Failed to send event");  // Print error if unable to send
    }

    Serial.printf("Device manually turned %s\r\n", deviceState ? "on" : "off"); // Print manual toggle state
    lastPress = now;                         // Update last button press time
  }
}

// Function to connect to WiFi
void setupWiFi() {
  Serial.print("[WiFi] Connecting");          // Print connecting message
  WiFi.begin(WIFI_SSID, WIFI_PASS);           // Begin WiFi connection with SSID and password
  while (WiFi.status() != WL_CONNECTED) {     // Wait until WiFi is connected
    Serial.print(".");                        // Print dots while connecting
    delay(250);                               // Short delay to avoid rapid serial prints
  }
  Serial.printf("Connected! IP: %s\r\n", WiFi.localIP().toString().c_str()); // Print IP address when connected
}

// Function to set up SinricPro connection and events
void setupSinricPro() {
  SinricProSwitch& mySwitch = SinricPro[SWITCH_ID]; // Get the SinricPro Switch device
  mySwitch.onPowerState(onPowerState);             // Set the onPowerState callback for handling power requests
  SinricPro.onConnected([](){ Serial.println("Connected to SinricPro"); });  // Print message when connected to SinricPro
  SinricPro.onDisconnected([](){ Serial.println("Disconnected from SinricPro"); }); // Print message when disconnected
  SinricPro.begin(APP_KEY, APP_SECRET);           // Initialize SinricPro with app key and secret
}

// Main setup function runs once on startup
void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);              // Set button pin as input with pull-up resistor
  pinMode(LED_PIN, OUTPUT);                       // Set LED pin as output
  digitalWrite(LED_PIN, HIGH);                    // Turn off LED initially (inverted logic)

  Serial.begin(BAUD_RATE);                        // Start serial communication at defined baud rate
  setupWiFi();                                    // Call WiFi setup function
  setupSinricPro();                               // Call SinricPro setup function
}

// Main loop function runs continuously
void loop() {
  handleButtonPress();                            // Check if button was pressed and handle if so
  SinricPro.handle();                             // Process SinricPro events
}
