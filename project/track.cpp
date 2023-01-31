#include "track.h"
#include <SD.h>

Track::Track(const char *fileName0, const char *fileName1) {
  if (SD.exists(fileName0)) {
    SD.remove(fileName0);
  }
  if (SD.exists(fileName1)) {
    SD.remove(fileName1);
  }
  fileNames[0] = fileName0;
  fileNames[1] = fileName1;
  position = 0;
  readFileIdx = 0;
  writeFileIdx = 1;
}

void Track::continuePlaying(void) {
  if (!playback.isPlaying()) {
    position = 0;
    startPlaying();
  }
}

void Track::continueRecording(void) {
  if (record.available() >= 2) {
    byte buffer[512];
    memcpy(buffer, record.readBuffer(), 256);
    record.freeBuffer();
    memcpy(buffer + 256, record.readBuffer(), 256);
    record.freeBuffer();
    fileBuffer.write(buffer, 512);
  }
}

void Track::pausePlaying(void) {
  position = playback.getOffset();
  playback.stop();
}

void Track::startPlaying(void) {
  playback.play(fileNames[readFileIdx], position);
}

boolean Track::startRecording(void) {
  fileBuffer = SD.open(fileNames[writeFileIdx], FILE_WRITE);
  if (fileBuffer) {
    fileBuffer.seek(position);
    record.begin();
    return true;
  }
  return false;
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
  readFileIdx = !readFileIdx;
  writeFileIdx = !readFileIdx;
}
