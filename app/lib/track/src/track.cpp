#include <Arduino.h>
#include <SD.h>
#include <track.hpp>

Gain gain = {0.0, 0.5, 1.0};

bool Track::advance(Status status) {
  if (status == Status::Play) {
    if (audio.lengthMillis() == audio.positionMillis()) {
      // Playback reached end, restart from beginning
      return play();
    } else if (!audio.isPlaying()) {
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
  if (SD.exists(readFileName)) {
    SD.remove(readFileName);
  }
  // Create the read file buffers
  File temp = SD.open(writeFileName, FILE_WRITE);
  temp.close();
  temp = SD.open(readFileName, FILE_WRITE);
  temp.close();
  // TODO: Move this into a mode toggle handler
  bus.gain(Channel::Source, gain.solo);
  bus.gain(Channel::Copy, gain.mute);
}

void Track::closeWriteBuffer(void) {
  recordQueue.end();
  while (recordQueue.available() > 0) {
    writeFileBuffer.write((byte *)recordQueue.readBuffer(), 256);
    recordQueue.freeBuffer();
  }
  writeFileBuffer.close();
  swapBuffers();
}

bool Track::openWriteBuffer() {
  writeFileBuffer = SD.open(writeFileName, FILE_WRITE);
  if (writeFileBuffer) {
    writeFileBuffer.seek(position);
    recordQueue.begin();
    return true;
  } else {
    Serial.println("Failed to open buffer for writing!");
    return false;
  }
}

void Track::pause() {
  position = audio.getOffset();
  resetPosition();
  audio.stop();
};

bool Track::play(uint32_t offset) {
  if (writeFileBuffer)
    writeFileBuffer.close();
  return audio.play(readFileName, offset);
};

void Track::pausePlayback() {
  position = audio.getOffset();
  audio.stop();
}

void Track::pauseRecording() {
  // position = closeWriteBuffer();
  closeWriteBuffer();
}

bool Track::record() { return writeToBuffer(); };

void Track::startPlayback() {
  if (writeFileBuffer)
    closeWriteBuffer();
}

void Track::startRecording() {
  audio.stop();
  openWriteBuffer();
}

void Track::stop() {
  position = 0;
  resetPosition();
  audio.stop();
};

void Track::stopPlayback() {
  resetPosition();
  audio.stop();
}

void Track::stopRecording() {
  resetPosition();
  closeWriteBuffer();
}
/* void Track::stopPlayback() {
  audio.stop();
  closeWriteBuffer();
} */

void Track::swapBuffers() {
  const char *temp = readFileName;
  readFileName = writeFileName;
  writeFileName = temp;
}

bool Track::writeToBuffer() {
  if (!writeFileBuffer)
    return false;
  if (recordQueue.available() >= 2) {
    byte buffer[512];
    memcpy(buffer, recordQueue.readBuffer(), 256);
    recordQueue.freeBuffer();
    memcpy(buffer + 256, recordQueue.readBuffer(), 256);
    recordQueue.freeBuffer();
    writeFileBuffer.write(buffer, 512);
  }
  return true;
}
