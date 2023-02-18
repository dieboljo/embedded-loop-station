#ifndef TRACK_HPP
#define TRACK_HPP

#include <Audio.h>
#include <play-sd-raw.hpp>

enum class Status { Stop, Record, Play, Pause };
enum Channel { Source, Feedback };

class Track {
  uint64_t position;
  File fileBuffer;

  // Order of signal flow
  AudioInputI2S *source;

  AudioMixer4 bus;

  AudioConnection sourceToBus;
  // AudioConnection playbackToBus;
  AudioConnection busToRecordQueue;
  AudioRecordQueue recordQueue;

  void closeBuffer();
  bool openBuffer();
  bool writeBuffer();

protected:
  const char *readFileName;
  const char *writeFileName;
  void swapBuffers();

public:
  Track(const char *f1, const char *f2, AudioInputI2S *s)
      : position(0), source(s), sourceToBus(*source, 0, bus, Channel::Source),
        // playbackToBus(playback, 0, bus, Channel::Feedback),
        busToRecordQueue(*source, 0, recordQueue, 0), readFileName(f1),
        writeFileName(f2){};
  App::AudioPlaySdRaw playback;
  void advance(Status status);
  bool play();
  void stop();
  void pause();
  bool record();
  void begin();
};

#endif
