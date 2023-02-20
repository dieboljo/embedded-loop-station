#include <Arduino.h>
#include <SD.h>
#include <track.hpp>

Gain gain = {0.0, 0.5, 1.0};

bool Track::advance(Status status) {
  if (status == Status::Play) {
    bool isPlaying = true;
    if (audio.lengthMillis() == audio.positionMillis()) {
      // Playback reached end, restart from beginning
      isPlaying = audio.play(readFileName);
    } else if (!audio.isPlaying()) {
      // Restart playing from current position
      isPlaying = audio.play(readFileName, position);
    }
    position = audio.getOffset();
    return isPlaying;
  } else if (status == Status::Record) {
    bool isRecording = writeToBuffer();
    position = (uint32_t)writeFileBuffer.position();
    return isRecording;
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

uint32_t Track::closeWriteBuffer(void) {
  AudioNoInterrupts();
  recordQueue.end();
  while (recordQueue.available() > 0) {
    writeFileBuffer.write((byte *)recordQueue.readBuffer(), 256);
    recordQueue.freeBuffer();
  }
  uint32_t filePosition = (uint32_t)writeFileBuffer.position();
  writeFileBuffer.close();
  swapBuffers();
  AudioInterrupts();
  return filePosition;
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

bool Track::play(uint32_t offset) { return audio.play(readFileName, offset); };

void Track::pausePlayback() {
  AudioNoInterrupts();
  position = audio.getOffset();
  audio.stop();
  AudioInterrupts();
}

void Track::pauseRecording() { position = closeWriteBuffer(); }

bool Track::record() { return writeToBuffer(); };

void Track::startPlayback() {
  if (writeFileBuffer) {
    closeWriteBuffer();
  }
}

void Track::startRecording() {
  AudioNoInterrupts();
  audio.stop();
  openWriteBuffer();
  AudioInterrupts();
}

void Track::stop() {
  position = 0;
  resetPosition();
  audio.stop();
};

void Track::stopPlayback() {
  AudioNoInterrupts();
  resetPosition();
  audio.stop();
  AudioInterrupts();
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
