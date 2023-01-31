#ifndef track_h_
#define track_h_

#include "play-sd-raw.h"
#include <Arduino.h>
#include <Audio.h>

class Track {
  const char *readFileName;
  const char *writeFileName;
  File fileBuffer;
  uint64_t position;
  void swapFiles();

public:
  AudioRecordQueue record;
  Project::AudioPlaySdRaw playback;
  void continuePlaying();
  void continueRecording();
  void pausePlaying();
  void startPlaying();
  boolean startRecording();
  void stopPlaying();
  void stopRecording();
  Track(const char *fileName1, const char *fileName2);
};

#endif
