#ifndef recorder_h
#define recorder_h

#include "constants.h"
#include "track.h"

class Recorder {
  Status status = Status::Stop;
  Mode mode = Mode::Replace;
  Track *selected = &tracks[0];

  AudioInputI2S *source;
  Track tracks[4];

  AudioConnection track1ToMix;
  AudioConnection track2ToMix;
  AudioConnection track3ToMix;
  AudioConnection track4ToMix;

public:
  Recorder(AudioInputI2S *s)
      : source(s), tracks{Track("0A.WAV", "0B.WAV", source),
                          Track("1A.WAV", "1B.WAV", source),
                          Track("2A.WAV", "2B.WAV", source),
                          Track("3A.WAV", "3B.WAV", source)},
        track1ToMix(tracks[0].playback, 0, mix, 0),
        track2ToMix(tracks[1].playback, 0, mix, 1),
        track3ToMix(tracks[2].playback, 0, mix, 2),
        track4ToMix(tracks[3].playback, 0, mix, 3){};
  int selectedTrackIdx = 0;
  AudioMixer4 mix;
  bool begin();
  void advance();
  Track getTrack(int i);
  void setSelected(int i);
  void handleRecordPress();
  void handlePlayPress();
  void handleStopPress();
};

#endif
