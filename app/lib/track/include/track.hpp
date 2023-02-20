#ifndef TRACK_HPP
#define TRACK_HPP

#include <Audio.h>
#include <play-sd-raw.hpp>

enum class Mode { Replace, Overdub };
enum class Status { Stop, Record, Play, Pause };
enum Channel { Source, Copy };
struct Gain {
  float mute;
  float mix;
  float solo;
};

class Track {
  uint32_t position;
  File writeFileBuffer;

  // Order of signal flow
  AudioInputI2S *source;
  AudioAnalyzePeak monitor;
  AudioMixer4 bus;
  AudioRecordQueue recordQueue;
  App::AudioPlaySdRaw copy;

  AudioConnection sourceToBus;
  AudioConnection copyToBus;
  AudioConnection busToMonitor;
  AudioConnection busToRecordQueue;

  void closeWriteBuffer();
  bool openWriteBuffer();
  bool play(uint32_t offset = 0);
  bool record();
  bool writeToBuffer();

protected:
  const char *readFileName;
  const char *writeFileName;
  void swapBuffers();

public:
  Track(const char *f1, const char *f2, AudioInputI2S *s)
      : position(0), source(s), sourceToBus(*source, 0, bus, Channel::Source),
        copyToBus(copy, 0, bus, Channel::Copy), busToMonitor(bus, monitor),
        busToRecordQueue(*source, 0, recordQueue, 0), readFileName(f1),
        writeFileName(f2){};

  App::AudioPlaySdRaw audio;

  bool advance(Status status);
  void begin();
  void pause();
  float readPeak() { return monitor.available() ? monitor.read() : 0.0; };
  void resetPosition() { position = 0; };
  void startPlayback();
  void startRecording();
  void stopPlayback();
  void stopRecording();
  void pausePlayback();
  void pauseRecording();
  void stop();
};

#endif
