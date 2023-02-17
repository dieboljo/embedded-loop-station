#ifndef TRACK_HPP
#define TRACK_HPP

#include <Audio.h>
#include <play-sd-raw.hpp>

enum class Status { Stop, Record, Play, Pause };

class Track {
  const char *fileName;

  uint64_t position;
  File fileBuffer;

  // Order of signal flow
  AudioInputI2S *source;
  AudioConnection sourceToRecordQueue;
  AudioRecordQueue recordQueue;

  void closeBuffer();
  bool openBuffer();
  bool writeBuffer();

public:
  Track(const char *f, AudioInputI2S *s)
      : fileName(f), position(0), source(s),
        sourceToRecordQueue(*source, 0, recordQueue, 0){};
  App::AudioPlaySdRaw playback;
  void advance(Status status);
  bool play();
  void stop();
  void pause();
  bool record();
  void begin();
};

#endif
