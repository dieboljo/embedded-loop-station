#include "track.hpp"
#include "constants.h"
#include <Arduino.h>
#include <SD.h>

const float splitGain = 0.4;
const float fullGain = 0.8;

void Track::advance(int selectedTrack) {
  position = playback.getOffset();
  // Reset position to beginning if at end of track
  if (playback.lengthMillis() == playback.positionMillis()) {
    position = 0;
  }
  // Restart playing from current position
  if (!playback.isPlaying()) {
    playback.play(readFileName, position);
  }
  if (selectedTrack == id) {
    if (fileBuffer.position() != position)
      fileBuffer.seek(position);
    writeBuffer();
  }
}

bool Track::begin() {
  if (SD.exists(readFileName)) {
    SD.remove(readFileName);
  }
  if (SD.exists(writeFileName)) {
    SD.remove(writeFileName);
  }
  bus.gain(Channel::Source, splitGain);
  bus.gain(Channel::Aux, splitGain);
  return true;
}

void Track::closeBuffer(void) {
  while (recordQueue.available() > 0) {
    fileBuffer.write((byte *)recordQueue.readBuffer(), 256);
    recordQueue.freeBuffer();
  }
  fileBuffer.close();
  const char *temp = writeFileName;
  writeFileName = readFileName;
  readFileName = temp;
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

void Track::patchCopy() {
  sourceToBus.disconnect();
  auxToBus.connect();
  bus.gain(Channel::Aux, fullGain);
}

void Track::patchOverdub() {
  auxToBus.connect();
  sourceToBus.connect();
  bus.gain(Channel::Aux, fullGain);
  bus.gain(Channel::Source, splitGain);
}

void Track::patchReplace() {
  auxToBus.disconnect();
  sourceToBus.connect();
  bus.gain(Channel::Source, fullGain);
}

void Track::pause() {
  playback.stop();
  recordQueue.end();
  closeBuffer();
};

void Track::play() {
  patchCopy();
  playback.play(readFileName, position);
};

void Track::record(Mode mode) {
  if (mode == Mode::Overdub) {
    patchOverdub();
  } else {
    patchReplace();
  }
};

void Track::stop() {
  position = 0;
  pause();
};

void Track::writeBuffer() {
  openBuffer();
  if (recordQueue.available() >= 2) {
    byte buffer[512];
    memcpy(buffer, recordQueue.readBuffer(), 256);
    recordQueue.freeBuffer();
    memcpy(buffer + 256, recordQueue.readBuffer(), 256);
    recordQueue.freeBuffer();
    fileBuffer.write(buffer, 512);
  }
}
