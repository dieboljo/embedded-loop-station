#include "recorder.h"
#include <Arduino.h>
#include <SD.h>

const int SDCARD_MOSI_PIN = 11;
const int SDCARD_SCK_PIN = 13;
const int SDCARD_CS_PIN = BUILTIN_SDCARD;

void Recorder::advance() {
  for (Track track : tracks) {
    track.advance(status, mode, track.id == selectedTrack);
  }
}

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
  for (Track track : tracks) {
    track.begin();
  }
  return true;
}

Track Recorder::getTrack(int i) { return tracks[i - 1]; }

void Recorder::handlePlayPress() {
  if (status == Status::Play) {
    status = Status::Pause;
  } else {
    status = Status::Play;
  }
};

void Recorder::handleRecordPress() {
  if (status == Status::Record) {
    status = Status::Play;
  } else {
    status = Status::Record;
  }
};

void Recorder::handleStopPress() { status = Status::Stop; };

void Recorder::setSelected(int i) { selectedTrack = i; }
