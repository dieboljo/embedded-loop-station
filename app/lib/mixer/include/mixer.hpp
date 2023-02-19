#ifndef MIXER_H
#define MIXER_H

#include <Audio.h>
#include <track.hpp>

class Mixer {
  AudioInputI2S *source;
  Track tracks[4];

  AudioConnection track1ToMix;
  AudioConnection track2ToMix;
  AudioConnection track3ToMix;
  AudioConnection track4ToMix;

public:
  Mixer(AudioInputI2S *source)
      : source(source), tracks{Track("0A.RAW", "0B.RAW", source, 1),
                               Track("1A.RAW", "1B.RAW", source, 2),
                               Track("2A.RAW", "2B.RAW", source, 3),
                               Track("3A.RAW", "3B.RAW", source, 4)},
        track1ToMix(tracks[0].playback, 0, mix, 0),
        track2ToMix(tracks[1].playback, 0, mix, 1),
        track3ToMix(tracks[2].playback, 0, mix, 2),
        track4ToMix(tracks[3].playback, 0, mix, 3){};
  int selectedTrack = 1;
  AudioMixer4 mix;
  bool begin();
  void advance(Status status, Mode mode);
  // Track getTrack(int i);
  void setSelectedTrack(int i);
  void handleRecordPress(Status stats, Mode mode);
  void handlePlayPress(Status status);
  void handleStopPress();
};

#endif
