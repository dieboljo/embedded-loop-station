#ifndef TRACK_HPP
#define TRACK_HPP

#include <Audio.h>
#include <play-sd-raw.hpp>

enum class Status { Stop, Record, Play, Pause };

class Track {
  uint64_t position;
  File fileBuffer;

  // Order of signal flow
  AudioInputI2S *source;
  AudioConnection sourceToRecordQueue;
  AudioRecordQueue recordQueue;

  void closeBuffer();
  bool openBuffer();
  bool writeBuffer();

protected:
  const char *fileName1;
  const char *fileName2;
  void swapBuffers();

public:
  Track(const char *f1, const char *f2, AudioInputI2S *s)
      : position(0), source(s), sourceToRecordQueue(*source, 0, recordQueue, 0),
        fileName1(f1), fileName2(f2){};
  App::AudioPlaySdRaw playback;
  void advance(Status status);
  bool play();
  void stop();
  void pause();
  bool record();
  void begin();
};

#endif
