#include "track.h"
#include "constants.h"
#include <Arduino.h>
#include <SD.h>

const float SPLIT_GAIN = 0.4;
const float FULL_GAIN = 0.8;

void Track::advance(Status status, Mode mode, bool selected) {
  position = playback.getOffset();
  switch (status) {
  case Status::Play:
    if (playback.lengthMillis() == playback.positionMillis()) {
      position = 0;
    }
    if (!playback.isPlaying()) {
      playback.play(readFileName, position);
    }
    if (playback.lengthMillis()) {
      patchFeedback();
    }
    break;
  case Status::Record:
    if (mode == Mode::Overdub) {
      patchOverdub();
    } else {
      patchReplace();
    }
    break;
  case Status::Pause:
    playback.stop();
    record.end();
    closeBuffer();
    return;
  case Status::Stop:
    position = 0;
    playback.stop();
    record.end();
    closeBuffer();
    return;
  }
  if (selected) {
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
  mix.gain(Channel::Source, SPLIT_GAIN);
  mix.gain(Channel::Aux, SPLIT_GAIN);
  return true;
}

bool Track::openBuffer() {
  if (fileBuffer)
    return true;
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

void Track::patchFeedback() {
  sourceToMix.disconnect();
  auxToMix.connect();
  mix.gain(Channel::Aux, FULL_GAIN);
}

void Track::patchOverdub() {
  auxToMix.connect();
  sourceToMix.connect();
  mix.gain(Channel::Aux, FULL_GAIN);
  mix.gain(Channel::Source, SPLIT_GAIN);
}

void Track::patchReplace() {
  auxToMix.disconnect();
  sourceToMix.connect();
  mix.gain(Channel::Source, FULL_GAIN);
}

void Track::closeBuffer(void) {
  while (record.available() > 0) {
    fileBuffer.write((byte *)record.readBuffer(), 256);
    record.freeBuffer();
  }
  fileBuffer.close();
  const char *temp = writeFileName;
  writeFileName = readFileName;
  readFileName = temp;
}

void Track::writeBuffer() {
  openBuffer();
  if (record.available() >= 2) {
    byte buffer[512];
    memcpy(buffer, record.readBuffer(), 256);
    record.freeBuffer();
    memcpy(buffer + 256, record.readBuffer(), 256);
    record.freeBuffer();
    fileBuffer.write(buffer, 512);
  }
}
