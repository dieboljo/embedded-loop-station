#ifndef track_h_
#define track_h_

#include "constants.h"
#include "play-sd-raw.h"
#include <Audio.h>

class Track {
  enum Channel { Source, Aux };

  const char *readFileName;
  const char *writeFileName;

  uint64_t position = 0;
  File fileBuffer;

  // Order of signal flow
  AudioInputI2S *source;
  AudioConnection auxToMix; // feedback `playback` back into `mix`
  AudioConnection sourceToMix;
  AudioMixer4 mix;
  AudioConnection mixToRecord;
  AudioRecordQueue record;

  void closeBuffer();
  bool openBuffer();
  void patchFeedback();
  void patchReplace();
  void patchOverdub();
  void writeBuffer();

public:
  Track(const char *f1, const char *f2, AudioInputI2S *s)
      : readFileName(f1), writeFileName(f2), source(s),
        auxToMix(playback, 0, mix, Channel::Aux),
        sourceToMix(*source, 0, mix, Channel::Source),
        mixToRecord(mix, 0, record, 0){};
  Project::AudioPlaySdRaw playback;
  void advance(Status status, Mode mode, boolean selected);
  bool begin();
};

#endif
