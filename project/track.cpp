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

void Track::pausePlaying(void) {
  position = readStream.getOffset();
  readStream.stop();
}

void Track::startPlaying(void) {
  readStream.play(fileNames[readFileIdx], position);
}

void Track::startRecording(void) {
  writeStream = SD.open(fileNames[writeFileIdx], FILE_WRITE);
  writeStream.seek(position);
  queue.begin();
}

void Track::stopPlaying(void) {
  position = 0;
  readStream.stop();
}

void Track::stopRecording(void) {
  queue.end();
  while (queue.available() > 0) {
    writeStream.write((byte *)queue.readBuffer(), 256);
    queue.freeBuffer();
  }
  writeStream.close();
  const bool playing = readStream.isPlaying();
  readStream.stop();
  swapFiles();
  if (playing) {
    startPlaying();
  }
}

void Track::swapFiles(void) {
  readFileIdx = !readFileIdx;
  writeFileIdx = !readFileIdx;
}

/* class Track {
  const char *fileNames[2];
  uint64_t position;
  int readFileIdx;
  int writeFileIdx;
  Project::AudioPlaySdRaw readStream;
  File writeStream;
  AudioRecordQueue queue;
  AudioAnalyzePeak peak;

public:
  void pausePlaying();
  void startRecording();
  void stopPlaying();
  void stopRecording();
  void swapStreams();
  Track(const char *fileName0, const char *fileName1);
}; */
