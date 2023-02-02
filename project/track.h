#ifndef track_h_
#define track_h_

#include "constants.h"
#include "play-sd-raw.h"
#include <Audio.h>

#define AUDIO_CHANNEL 0
#define AUX_CHANNEL 1

#define GAIN_ACTIVE 0.4
#define GAIN_MUTED 0.0

class Track {
  File fileBuffer;
  const char *readFileName;
  const char *writeFileName;
  AudioRecordQueue record;
  AudioInputI2S *source;
  uint64_t position = 0;
  AudioConnection auxToSourceOutput;
  AudioConnection sourceToSourceOutput;
  AudioConnection sourceOutputToRecord;
  void swapFiles();
  bool openBuffer();

public:
  Track(const char *f1, const char *f2, AudioInputI2S *s)
      : readFileName(f1), writeFileName(f2), source(s),
        auxToSourceOutput(playback, 0, sourceOutput, AUX_CHANNEL),
        sourceToSourceOutput(*source, 0, sourceOutput, AUDIO_CHANNEL),
        sourceOutputToRecord(sourceOutput, 0, record, 0) {
    if (SD.exists(readFileName)) {
      SD.remove(readFileName);
    }
    if (SD.exists(writeFileName)) {
      SD.remove(writeFileName);
    }
    sourceOutput.gain(AUDIO_CHANNEL, GAIN_ACTIVE);
    sourceOutput.gain(AUX_CHANNEL, GAIN_MUTED);
  };
  AudioMixer4 sourceOutput;
  Project::AudioPlaySdRaw playback;
  void advance(Status status, Mode mode);
  void pausePlaying();
  bool startPlaying();
  bool startRecording();
  void stopPlaying();
  void stopRecording();
  void writeBuffer();
};

#endif
