/*
  Teensy 4.1 Prototyping System Board Example

  This program draws 2 buttons on the LCD screen - Audio and Scan

  If Audio adapter is attached with an SD card installed, the Audio
  button plays the wave file "SDTEST2.WAV" from the Teensy audio tutorial
  https://www.pjrc.com/teensy/td_libs_AudioDataFiles.html

  The Scan button sends a command to the ESP32S if one is attached requesting
  scan of available WiFi networks.  When the ESP32S returns the scan results,
  the Teensy 4.1 updates those results on the LCD screen.
  This requires an ESP32S to be installed and the sample program
  ESP32_Teensy_41_Demo be loaded on it, which is just a modified version
  of the ESP32 WiFiScan example program.
*/
#include "LittleFS.h"
#include <Audio.h>
#include <ILI9341_t3.h>
#include <SD.h>
#include <SPI.h>
#include <SerialFlash.h>
#include <Wire.h>
#include <XPT2046_Touchscreen.h>
#include <font_Arial.h>
extern "C" uint8_t external_psram_size;

AudioPlaySdWav playSdWav1;
AudioOutputI2S i2s1;
AudioConnection patchCord1(playSdWav1, 0, i2s1, 0);
AudioConnection patchCord2(playSdWav1, 1, i2s1, 1);
AudioControlSGTL5000 sgtl5000_1;

// Pins used with the Teensy Audio Shield
#define SDCARD_CS_PIN 10
#define SDCARD_MOSI_PIN 11 // 7
#define SDCARD_SCK_PIN 13  // 14

// touchscreen offset for four corners
#define TS_MINX 400
#define TS_MINY 400
#define TS_MAXX 3879
#define TS_MAXY 3843

// LCD control pins defined by board
#define TFT_CS 40
#define TFT_DC 9

// Use main SPI bus MOSI=11, MISO=12, SCK=13 with different control pins
ILI9341_t3 tft = ILI9341_t3(TFT_CS, TFT_DC);

// Touch screen control pins defined by board
// TIRQ interrupt if used is on pin 2
#define TS_CS 41
// #define TIRQ_PIN  2
XPT2046_Touchscreen ts(TS_CS); // Param 2 = NULL - No interrupts

// Define Audio button location and size
#define AUDIO_X 10
#define AUDIO_Y 10
#define AUDIO_W 105
#define AUDIO_H 32

// Define Scan button location and size
#define SCAN_X 10
#define SCAN_Y 50
#define SCAN_W 105
#define SCAN_H 32
#define BUTTON_FONT Arial_14

#define ESP32SERIAL Serial1 // ESP32 is attached to Serial1 port

// Subroutine prototypes
void SetScanButton(boolean);  // Handles Scan button when touched
void SetAudioButton(boolean); // Handles Audio button when touched

// Misc flags to keep track of things
boolean isTouched = false;            // Flag if a touch is in process
boolean scanRequested = false;        // Flag if WiFi scan is in process
boolean audioAdapterAttached = false; // Flag if audio bd with SD card attached
boolean audioPlaying = false;         // Flag if audio is currently playing
boolean esp32SAttached = false;       // Flag if ESP32S is attached

//===============================================================================
//  Initialization
//===============================================================================
void setup() {
  Serial.begin(115200); // USB serial port

  // Check for PSRAM chip installed
  uint8_t size = external_psram_size;
  Serial.printf("PSRAM Memory Size = %d Mbyte\n", size);
  if (size == 0)
    Serial.println("No PSRAM Installed");

  LittleFS_QSPIFlash myfs_NOR; // NOR FLASH
  LittleFS_QPINAND myfs_NAND;  // NAND FLASH 1Gb

  // Check for NOR Flash chip installed
  if (myfs_NOR.begin()) {
    Serial.printf("NOR Flash Memory Size = %d Mbyte / ",
                  myfs_NOR.totalSize() / 1048576);
    Serial.printf("%d Mbit\n", myfs_NOR.totalSize() / 131072);
  }
  // Check for NAND Flash chip installed
  else if (myfs_NAND.begin()) {
    Serial.printf("NAND Flash Memory Size =  %d bytes / ",
                  myfs_NAND.totalSize());
    Serial.printf("%d Mbyte / ", myfs_NAND.totalSize() / 1048576);
    Serial.printf("%d Gbit\n", myfs_NAND.totalSize() * 8 / 1000000000);
  } else {
    Serial.printf("No Flash Installed\n");
  }

  ESP32SERIAL.begin(115200); // Seria1 1 connected to ESP32S

  // Setup LCD screen
  tft.begin();
  tft.setRotation(3);
  ts.begin();
  ts.setRotation(1);
  tft.fillScreen(ILI9341_BLUE);

  // Draw buttons
  SetAudioButton(false);
  SetScanButton(false);

  // Setup audio
  AudioMemory(8);
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.5);
  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  // Check if SD card is found on an Audio adapter.  Gray out button if not
  // found
  if (!(SD.begin(SDCARD_CS_PIN))) { // No Audio, grey out Audio button
    Serial.println("Audio board with SD card not found");
    audioAdapterAttached = false;
    tft.setCursor(AUDIO_X + 8, AUDIO_Y + 8);
    tft.setFont(BUTTON_FONT);
    tft.setTextColor(ILI9341_WHITE);
    tft.fillRoundRect(AUDIO_X, AUDIO_Y, AUDIO_W, AUDIO_H, 4, ILI9341_DARKGREY);
    tft.print("No Audio");
  } else { // We found an SD card in the audio adapter
    Serial.println("Audio board with SD card is attached");
    audioAdapterAttached = true;
  }

  ESP32SERIAL.print("?"); // Check if there is an ESP32S attached
  delay(100);             // Wait a bit for a return
  if (ESP32SERIAL.available()) {
    String returnData = ESP32SERIAL.readString();
    if (returnData == 'Y') { // ESP32S responded with Y for Yes, I'm here
      esp32SAttached = true;
      Serial.println("ESP32S was found");
    }
  } // Grey out button if ESP32S did not respond.
  if (!esp32SAttached) {
    Serial.println("ESP32S not found");
    tft.setCursor(SCAN_X + 8, SCAN_Y + 8);
    tft.setFont(BUTTON_FONT);
    tft.setTextColor(ILI9341_WHITE);
    tft.fillRoundRect(SCAN_X, SCAN_Y, SCAN_W, SCAN_H, 4, ILI9341_DARKGREY);
    tft.print("No Scan");
    esp32SAttached = false;
  }
  delay(1000);
}
//===============================================================================
//  Main
//===============================================================================
void loop() {
  // Keep an eye on any audio that may be playing and reset button when it ends
  if (playSdWav1.isStopped() && audioPlaying) { // Audio finished playing
    SetAudioButton(false);
    Serial.println("Audio finished playing");
  }
  // Check to see if the touch screen has been touched
  if (ts.touched() && isTouched == false) {
    TS_Point p = ts.getPoint();
    // Map the touch point to the LCD screen
    p.x = map(p.x, TS_MINY, TS_MAXY, 0, tft.width());
    p.y = map(p.y, TS_MINX, TS_MAXX, 0, tft.height());
    isTouched = true;

    // Look for a Scan Button Hit
    if ((p.x > SCAN_X) && (p.x < (SCAN_X + SCAN_W))) {
      if ((p.y > SCAN_Y) && (p.y <= (SCAN_Y + SCAN_H))) {
        Serial.println("Scan Button Hit");
        if (esp32SAttached)
          SetScanButton(true);
      }
    }
    // Look for an Audio Button Hit
    if ((p.x > AUDIO_X) && (p.x < (AUDIO_X + AUDIO_W))) {
      if ((p.y > AUDIO_Y) && (p.y <= (AUDIO_Y + AUDIO_H))) {
        Serial.println("Audio Button Hit");
        if (audioAdapterAttached && !audioPlaying) {
          SetAudioButton(true);
        } else if (audioAdapterAttached && audioPlaying) {
          SetAudioButton(false);
        }
      }
    }
    Serial.print("x = "); // Show our touch coordinates for each touch
    Serial.print(p.x);
    Serial.print(", y = ");
    Serial.print(p.y);
    Serial.println();
    delay(100); // Debounce touchscreen a bit
  }
  if (!ts.touched() && isTouched) {
    isTouched = false; // touchscreen is no longer being touched, reset flag
  }
  // If we requested a scan, look for serial data coming back from the ESP32S
  if (scanRequested && ESP32SERIAL.available()) {
    Serial.print("Read incoming data");
    tft.setCursor(10, 90);
    tft.setFont(Arial_10);
    while (ESP32SERIAL.available()) { // Print the scan data to the LCD & USB
      String returnData = ESP32SERIAL.readString();
      tft.println(returnData);
      Serial.println(returnData);
    }
    scanRequested = false; // Reset the scan flag and button
    SetScanButton(false);
  }
}
//===============================================================================
//  Routine to draw Audio button current state and control audio playback
//===============================================================================
void SetAudioButton(boolean audio) {
  tft.setCursor(AUDIO_X + 8, AUDIO_Y + 8);
  tft.setFont(BUTTON_FONT);
  tft.setTextColor(ILI9341_WHITE);

  if (!audio) { // button is set inactive, redraw button inactive
    tft.fillRoundRect(AUDIO_X, AUDIO_Y, AUDIO_W, AUDIO_H, 4, ILI9341_RED);
    tft.print("Play Audio");
    audioPlaying = false;
    if (playSdWav1.isPlaying()) { // Stop any audio that is playing
      playSdWav1.stop();
      Serial.println("Audio being stopped");
    }
  } else { // button is active, redraw button active
    tft.fillRoundRect(AUDIO_X, AUDIO_Y, AUDIO_W, AUDIO_H, 4, ILI9341_GREEN);
    tft.print("Playing");
    audioPlaying = true;
    if (audioAdapterAttached && !playSdWav1.isPlaying()) { // Play audio file
      Serial.println("Audio being played");
      playSdWav1.play("SDTEST2.WAV");
      delay(10); // wait for library to parse WAV info
    }
  }
}
//===============================================================================
//  Routine to draw scan button current state and initiate scan request
//===============================================================================
void SetScanButton(boolean scanning) {
  tft.setCursor(SCAN_X + 8, SCAN_Y + 8);
  tft.setFont(BUTTON_FONT);
  tft.setTextColor(ILI9341_WHITE);

  if (!scanning) { // Button is inactive, redraw button
    tft.fillRoundRect(SCAN_X, SCAN_Y, SCAN_W, SCAN_H, 4, ILI9341_RED);
    tft.print("Scan WiFi");
  } else { // Button is active, redraw button
    tft.fillRect(1, SCAN_Y + SCAN_H, 360, 240,
                 ILI9341_BLUE); // Clear previous scan
    tft.fillRoundRect(SCAN_X, SCAN_Y, SCAN_W, SCAN_H, 4, ILI9341_GREEN);
    tft.print("Scanning");
    ESP32SERIAL.println("S"); // Send command to ESP32 to start scan
    scanRequested = true;     // Set flag that we requested scan
    Serial.println("Scan being requested");
  }
}
