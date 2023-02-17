#include <Arduino.h>
#include <SD.h>
#include <track.hpp>

void Track::advance(Status status) {
  // position = playback.getOffset();
  // Reset position to beginning if at end of track
  if (status == Status::Play) {
    // Restart playing from current position
    if (!playback.isPlaying()) {
      play();
    }
  } else if (status == Status::Record) {
    record();
  }
}

void Track::begin() {
  if (SD.exists(fileName)) {
    SD.remove(fileName);
  }
  File temp = SD.open(fileName, FILE_WRITE);
  temp.close();
}

void Track::closeBuffer(void) {
  recordQueue.end();
  while (recordQueue.available() > 0) {
    fileBuffer.write((byte *)recordQueue.readBuffer(), 256);
    recordQueue.freeBuffer();
  }
  fileBuffer.close();
}

bool Track::openBuffer() {
  if (fileBuffer)
    return true;
  fileBuffer = SD.open(fileName, FILE_WRITE);
  if (fileBuffer) {
    fileBuffer.seek(position);
    recordQueue.begin();
    return true;
  } else {
    Serial.println("Failed to open buffer for writing!");
    return false;
  }
}

void Track::pause() {
  playback.stop();
  closeBuffer();
};

bool Track::play() {
  if (fileBuffer)
    fileBuffer.close();
  return playback.play(fileName, position);
};

bool Track::record() { return writeBuffer(); };

void Track::stop() {
  position = 0;
  pause();
};

bool Track::writeBuffer() {
  bool opened = openBuffer();
  if (!opened) {
    return false;
  }
  if (recordQueue.available() >= 2) {
    byte buffer[512];
    memcpy(buffer, recordQueue.readBuffer(), 256);
    recordQueue.freeBuffer();
    memcpy(buffer + 256, recordQueue.readBuffer(), 256);
    recordQueue.freeBuffer();
    fileBuffer.write(buffer, 512);
  }
  return true;
}
