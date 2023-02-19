#include <Arduino.h>
#include <SD.h>
#include <track.hpp>

const float MIX = 0.4;
const float MUTE = 0.0;
const float SOLO = 0.8;

bool Track::advance(Status status) {
  if (status == Status::Play) {
    if (playback.lengthMillis() == playback.positionMillis()) {
      // Playback reached end, restart from beginning
      return play();
    } else if (!playback.isPlaying()) {
      // Restart playing from current position
      return play(position);
    }
  } else if (status == Status::Record) {
    return record();
  }
  return true;
}

void Track::begin() {
  if (SD.exists(writeFileName)) {
    SD.remove(writeFileName);
  }
  /* if (SD.exists(readFileName)) {
    SD.remove(readFileName);
  } */
  // Create the read file buffers
  File temp = SD.open(writeFileName, FILE_WRITE);
  temp.close();
  temp = SD.open(writeFileName, FILE_WRITE);
  temp.close();
}

void Track::closeBuffer(void) {
  recordQueue.end();
  while (recordQueue.available() > 0) {
    fileBuffer.write((byte *)recordQueue.readBuffer(), 256);
    recordQueue.freeBuffer();
  }
  fileBuffer.close();
  // swapBuffers();
}

bool Track::openBuffer() {
  if (fileBuffer)
    return true;
  fileBuffer = SD.open(writeFileName, FILE_WRITE);
  if (fileBuffer) {
    fileBuffer.seek(playback.getOffset());
    recordQueue.begin();
    return true;
  } else {
    Serial.println("Failed to open buffer for writing!");
    return false;
  }
}

void Track::pause() {
  position = playback.getOffset();
  stopPlayback();
};

bool Track::play(uint32_t offset) {
  if (fileBuffer)
    fileBuffer.close();
  return playback.play(writeFileName, offset);
};

bool Track::record() {
  bus.gain(Channel::Source, SOLO);
  return writeBuffer();
};

void Track::stop() {
  position = 0;
  stopPlayback();
};

void Track::stopPlayback() {
  playback.stop();
  closeBuffer();
}

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
