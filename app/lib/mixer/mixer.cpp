#include "mixer.hpp"
#include <Arduino.h>
#include <SD.h>
#include <track.hpp>

const int SDCARD_MOSI_PIN = 11;
const int SDCARD_SCK_PIN = 13;
const int SDCARD_CS_PIN = BUILTIN_SDCARD;

void Mixer::advance(Status status, Mode mode) {
  for (Track track : tracks) {
    track.advance(status, mode, track.id == selectedTrack);
  }
}

bool Mixer::begin() {
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

// Track Mixer::getTrack(int i) { return tracks[i - 1]; }

void Mixer::handlePlayPress(Status status) {
  if (status == Status::Play) {
    status = Status::Pause;
  } else {
    status = Status::Play;
  }
};

void Mixer::handleRecordPress(Status status, Mode mode) {
  if (status == Status::Record) {
    status = Status::Play;
  } else {
    status = Status::Record;
  }
};

void Mixer::handleStopPress(){};

void Mixer::setSelectedTrack(int i) { selectedTrack = i; }
