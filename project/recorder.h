#ifndef recorder_h
#define recorder_h

#include "track.h"

#define SDCARD_MOSI_PIN 11
#define SDCARD_SCK_PIN 13
#define SDCARD_CS_PIN BUILTIN_SDCARD

class Recorder {
  int selectedTrackIdx = 0;

  AudioInputI2S *source;
  Track tracks[4];

public:
  Recorder(AudioInputI2S *s)
      : source(s), tracks{Track("0A.WAV", "0B.WAV", source),
                          Track("1A.WAV", "1B.WAV", source),
                          Track("2A.WAV", "2B.WAV", source),
                          Track("3A.WAV", "3B.WAV", source)} {};
  Track selected = tracks[0];
  bool begin();
  Track getTrack(int i);
  void setSelected(int i);
};

#endif
