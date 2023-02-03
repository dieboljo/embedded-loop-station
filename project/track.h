#ifndef track_h_
#define track_h_

#include "constants.h"
#include "play-sd-raw.h"
#include <Audio.h>

#define SOURCE_CHANNEL 0
#define AUX_CHANNEL 1

#define SPLIT_GAIN 0.4
#define FULL_GAIN 0.8

class Track {
  uint64_t position = 0;
  const char *readFileName;
  const char *writeFileName;
  File fileBuffer;
  AudioRecordQueue record;
  AudioInputI2S *source;
  AudioConnection auxToMix;
  AudioConnection sourceToMix;
  AudioConnection mixToRecord;
  AudioMixer4 mix;
  void closeBuffer();
  bool openBuffer();
  void patchFeedback();
  void patchReplace();
  void patchOverdub();
  void writeBuffer();

public:
  Track(const char *f1, const char *f2, AudioInputI2S *s)
      : readFileName(f1), writeFileName(f2), source(s),
        auxToMix(playback, 0, mix, AUX_CHANNEL),
        sourceToMix(*source, 0, mix, SOURCE_CHANNEL),
        mixToRecord(mix, 0, record, 0) {
    if (SD.exists(readFileName)) {
      SD.remove(readFileName);
    }
    if (SD.exists(writeFileName)) {
      SD.remove(writeFileName);
    }
    mix.gain(SOURCE_CHANNEL, SPLIT_GAIN);
    mix.gain(AUX_CHANNEL, SPLIT_GAIN);
  };
  Project::AudioPlaySdRaw playback;
  void advance(Status status, Mode mode, boolean selected);
};

#endif
