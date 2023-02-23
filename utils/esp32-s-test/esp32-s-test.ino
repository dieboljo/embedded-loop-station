/*
    This sketch interfaces to the Teensy 4.1 via serial port 2.
    It demonstrates how to receive a request for a scan for WiFi networks and
    report the results back via serial port.

    This is a simple variation of the ESP32 WiFiScan example program
*/
#include "WiFi.h"

#define RX2 16 // Teensy 4.1 is connected to serial port #2
#define TX2 17

const int LED_PIN = 2; // We will light the LED when Scan is in process.
//===============================================================================
//  Initialization
//===============================================================================
void setup() {
  Serial.begin(115200);                        // USB port
  Serial2.begin(115200, SERIAL_8N1, RX2, TX2); // Port connected to Teensy 4.1

  // Set WiFi to station mode and disconnect from an AP if previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  pinMode(LED_PIN, OUTPUT);
  Serial.println("Setup done");
}
//===============================================================================
//  Main
//===============================================================================
void loop() {
  if (Serial2.available()) {
    char command = Serial2.read();
    Serial.println(command);
    if (command == '?') { // Are you there?
      Serial.println("Y");
      Serial2.print("Y"); // Acknowledge I'm attached
    }
    if (command == 'S') {
      Serial.println("scan start");
      digitalWrite(LED_PIN, HIGH); // turn the LED on
      // WiFi.scanNetworks will return the number of networks found
      int n = WiFi.scanNetworks();
      Serial.println("scan done");
      if (n == 0) {
        Serial.println("no networks found");
        Serial2.println("No networks found");
      } else {
        Serial.print(n);
        Serial2.print(n);
        Serial.println(" Networks Found");
        Serial2.println(" Networks Found");
        for (int i = 0; i < n; ++i) {
          // Print SSID and RSSI for each network found to both USB and
          // out Serial2 to attached Teensy 4.1
          Serial.print(i + 1);
          Serial2.print(i + 1);
          Serial.print(": ");
          Serial2.print(": ");
          Serial.print(WiFi.SSID(i));
          Serial2.print(WiFi.SSID(i));
          Serial.print(" (");
          Serial2.print(" (");
          Serial.print(WiFi.RSSI(i));
          Serial2.print(WiFi.RSSI(i));
          Serial.print(")");
          Serial2.print(")");
          Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " "
                                                                    : "*");
          Serial2.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " "
                                                                     : "*");
        }
      }
    }
    digitalWrite(LED_PIN, LOW); // turn the LED off
  }
}
