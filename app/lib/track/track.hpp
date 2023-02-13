#ifndef TRACK_HPP
#define TRACK_HPP

#include "constants.hpp"
#include "play-sd-raw.hpp"
#include <Audio.h>

class Track {
  enum Channel { Source, Aux };

  const char *readFileName;
  const char *writeFileName;

  uint64_t position;
  File fileBuffer;

  // Order of signal flow
  AudioInputI2S *source;
  AudioConnection auxToBus; // feedback audio bus back into input audio bus
  AudioConnection sourceToBus;
  AudioMixer4 bus;
  AudioConnection busToRecordQueue;
  AudioRecordQueue recordQueue;

  void closeBuffer();
  bool openBuffer();
  void patchCopy();
  void patchReplace();
  void patchOverdub();
  void writeBuffer();

public:
  Track(const char *f1, const char *f2, AudioInputI2S *s, int i)
      : readFileName(f1), writeFileName(f2), position(0), source(s),
        auxToBus(playback, 0, bus, Channel::Aux),
        sourceToMix(*source, 0, bus, Channel::Source),
        busToRecordQueue(bus, 0, recordQueue, 0), id(i){};
  App::AudioPlaySdRaw playback;
  void advance(int selected);
  void play();
  void stop();
  void pause();
  void record(Mode mode);
  bool begin();
  int id;
};

#endif
