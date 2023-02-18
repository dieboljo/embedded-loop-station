#include <Arduino.h>
#include <SD.h>
#include <track.hpp>

const float MIX = 0.4;
const float MUTE = 0.0;
const float SOLO = 0.8;

void Track::advance(Status status) {
  position = playback.getOffset();
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
  if (SD.exists(writeFileName)) {
    SD.remove(writeFileName);
  }
  if (SD.exists(readFileName)) {
    SD.remove(readFileName);
  }
  // Create the read file buffers
  File temp = SD.open(readFileName, FILE_WRITE);
  temp.close();
  temp = SD.open(readFileName, FILE_WRITE);
  temp.close();
}

void Track::closeBuffer(void) {
  recordQueue.end();
  while (recordQueue.available() > 0) {
    fileBuffer.write((byte *)recordQueue.readBuffer(), 256);
    recordQueue.freeBuffer();
  }
  fileBuffer.close();
  swapBuffers();
}

bool Track::openBuffer() {
  if (fileBuffer)
    return true;
  fileBuffer = SD.open(writeFileName, FILE_WRITE);
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
  return playback.play(readFileName, position);
};

bool Track::record() {
  bus.gain(Channel::Source, SOLO);
  return writeBuffer();
};

void Track::stop() {
  position = 0;
  pause();
};

void Track::swapBuffers() {
  const char *temp = readFileName;
  readFileName = writeFileName;
  writeFileName = temp;
}

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
