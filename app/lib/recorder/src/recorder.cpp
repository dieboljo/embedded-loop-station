#include <Arduino.h>
#include <SD.h>
#include <recorder.hpp>
#include <track.hpp>

const int SDCARD_MOSI_PIN = 11;
const int SDCARD_SCK_PIN = 13;
const int SDCARD_CS_PIN = BUILTIN_SDCARD;

void Recorder::advance(Status status, Mode mode) {
  for (Track track : tracks) {
    track.advance(selectedTrack);
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

// Track Recorder::getTrack(int i) { return tracks[i - 1]; }

void Recorder::handlePlayPress(Status status) {
  if (status == Status::Play) {
    status = Status::Pause;
  } else {
    status = Status::Play;
  }
};

void Recorder::handleRecordPress(Status status, Mode mode) {
  if (status == Status::Record) {
    status = Status::Play;
  } else {
    status = Status::Record;
  }
};

void Recorder::handleStopPress(){};

void Recorder::setSelectedTrack(int i) { selectedTrack = i; }
