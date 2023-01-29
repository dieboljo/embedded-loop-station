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
  Project::AudioPlaySdRaw readStream;
  File writeStream;
  AudioRecordQueue queue;
  AudioAnalyzePeak peak;

public:
  void pausePlaying();
  void startPlaying();
  void startRecording();
  void stopPlaying();
  void stopRecording();
  void swapStreams();
  Track(const char *fileName0, const char *fileName1);

};

#endif
