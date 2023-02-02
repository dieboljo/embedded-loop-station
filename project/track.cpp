#include "track.h"
#include "constants.h"
#include <Arduino.h>
#include <SD.h>

void Track::advance(Status status, Mode mode) {
  switch (status) {
  case Status::Play:
    if (!playback.isPlaying()) {
      position = 0;
      fileBuffer.seek(position);
      sourceOutput.gain(AUX_CHANNEL, GAIN_ACTIVE);
      sourceOutput.gain(AUDIO_CHANNEL, GAIN_MUTED);
      startPlaying();
    }
  case Status::Record:
    sourceOutput.gain(AUX_CHANNEL,
                      mode == Mode::Overdub ? GAIN_ACTIVE : GAIN_MUTED);
    sourceOutput.gain(AUDIO_CHANNEL, GAIN_ACTIVE);
  case Status::Stop:
    break;
  }
  writeBuffer();
}

void Track::writeBuffer() {
  if (record.available() >= 2) {
    byte buffer[512];
    memcpy(buffer, record.readBuffer(), 256);
    record.freeBuffer();
    memcpy(buffer + 256, record.readBuffer(), 256);
    record.freeBuffer();
    fileBuffer.write(buffer, 512);
  }
}

bool Track::openBuffer() {
  fileBuffer = SD.open(writeFileName, FILE_WRITE);
  if (fileBuffer) {
    fileBuffer.seek(position);
    record.begin();
    return true;
  } else {
    Serial.println("Failed to open buffer for writing!");
    return false;
  }
}

void Track::pausePlaying(void) {
  position = playback.getOffset();
  playback.stop();
  record.end();
}

bool Track::startPlaying(void) {
  if (playback.lengthMillis()) {
    bool bufferOpen = openBuffer();
    if (!bufferOpen)
      return false;
  }
  playback.play(readFileName, position);
  return true;
}

bool Track::startRecording() {
  if (!fileBuffer) {
    return openBuffer();
  }
  return true;
}

void Track::stopPlaying(void) {
  position = 0;
  playback.stop();
}

void Track::stopRecording(void) {
  record.end();
  while (record.available() > 0) {
    fileBuffer.write((byte *)record.readBuffer(), 256);
    record.freeBuffer();
  }
  fileBuffer.close();
  const bool playing = playback.isPlaying();
  playback.stop();
  swapFiles();
  if (playing) {
    startPlaying();
  }
}

void Track::swapFiles(void) {
  const char *temp = writeFileName;
  writeFileName = readFileName;
  readFileName = temp;
}
