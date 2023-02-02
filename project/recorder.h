#ifndef recorder_h
#define recorder_h

#include "track.h"

class Recorder {
  AudioInputI2S *source;
  Track tracks[4];

public:
  int selectedTrackIdx = 0;
  Track selected = tracks[0];
  void setSelected(int i);
  Track getTrack(int i);
  Recorder(AudioInputI2S *s)
      : source(s), tracks{Track("0A.WAV", "0B.WAV", source),
                          Track("1A.WAV", "1B.WAV", source),
                          Track("2A.WAV", "2B.WAV", source),
                          Track("3A.WAV", "3B.WAV", source)} {};
};

#endif
