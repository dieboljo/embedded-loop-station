#include "recorder.h"

const int SDCARD_MOSI_PIN = 11;
const int SDCARD_SCK_PIN = 13;
const int SDCARD_CS_PIN = BUILTIN_SDCARD;

bool Recorder::begin() {
  // Initialize the SD card
  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    // stop here if no SD card, but print a message
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }
  return true;
}

Track Recorder::getTrack(int i) { return tracks[i]; }

void Recorder::setSelected(int i) {
  selectedTrackIdx = i;
  selected = tracks[i];
}
