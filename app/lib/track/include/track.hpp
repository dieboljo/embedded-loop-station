#ifndef TRACK_HPP
#define TRACK_HPP

#include <Audio.h>
#include <play-sd-raw.hpp>

enum class Mode { Replace, Overdub };
enum class Status { Stop, Record, Play, Pause };
enum Channel { Source, Feedback };

class Track {
  uint32_t position;
  File fileBuffer;

  // Order of signal flow
  AudioInputI2S *source;
  AudioAnalyzePeak monitor;
  AudioMixer4 bus;
  AudioRecordQueue recordQueue;

  AudioConnection sourceToBus;
  AudioConnection playbackToBus;
  AudioConnection busToMonitor;
  AudioConnection busToRecordQueue;
  // AudioConnection sourceToRecordQueue;

  void closeBuffer();
  bool openBuffer();
  bool play(uint32_t offset = 0);
  bool record();
  bool writeBuffer();
  void stopPlayback();

protected:
  const char *readFileName;
  const char *writeFileName;
  void swapBuffers();

public:
  Track(const char *f1, const char *f2, AudioInputI2S *s)
      : position(0), source(s), sourceToBus(*source, 0, bus, Channel::Source),
        playbackToBus(playback, 0, bus, Channel::Feedback),
        busToMonitor(bus, monitor),
        busToRecordQueue(*source, 0, recordQueue, 0), readFileName(f1),
        writeFileName(f2){};

  App::AudioPlaySdRaw playback;

  void advance(Status status);
  void begin();
  void pause();
  float readPeak() { return monitor.available() ? monitor.read() : 0.0; };
  void stop();
};

#endif
