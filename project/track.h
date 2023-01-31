#ifndef track_h_
#define track_h_

#include "play-sd-raw.h"
#include <Arduino.h>
#include <Audio.h>

class Track {
  const char *fileNames[2];
  uint64_t position;
  int readFileIdx;
  int writeFileIdx;
  File fileBuffer;
  AudioRecordQueue record;
  Project::AudioPlaySdRaw playback;
  void swapFiles();

public:
  AudioAnalyzePeak monitor;
  void continuePlaying();
  void continueRecording();
  void pausePlaying();
  void startPlaying();
  boolean startRecording();
  void stopPlaying();
  void stopRecording();
  Track(const char *fileName0, const char *fileName1);
};

#endif
