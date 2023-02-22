#ifndef TRACK_HPP
#define TRACK_HPP

#include <Audio.h>

enum class Mode { Replace, Overdub };
enum class Status { Stop, Record, Play, Pause };
enum Channel { Source, Feedback };
struct Gain {
  float mute;
  float mix;
  float solo;
};

class Track {
  uint32_t position;

  // Order of signal flow
  AudioInputI2S *source;
  AudioAnalyzePeak peak;
  AudioMixer4 bus;
  AudioRecordWAVstereo recording;
  AudioPlayWAVstereo feedback;

  AudioConnection sourceToBus;
  AudioConnection feedbackToBus;
  AudioConnection busToPeak;
  AudioConnection busToRecordingLeft;
  AudioConnection busToRecordingRight;

  uint32_t closeWriteBuffer();
  bool openWriteBuffer();
  bool writeToBuffer();

protected:
  const char *readFileName;
  const char *writeFileName;
  void swapBuffers();

public:
  Track(const char *f1, const char *f2, AudioInputI2S *s)
      : position(0), source(s), sourceToBus(*source, 0, bus, Channel::Source),
        feedbackToBus(feedback, 0, bus, Channel::Feedback),
        busToPeak(bus, peak), busToRecordingLeft(bus, 0, recording, 0),
        busToRecordingRight(bus, 0, recording, 1), readFileName(f1),
        writeFileName(f2){};

  AudioPlayWAVstereo playback;

  bool advance(Status status);
  void begin();

  // TODO: Remove
  uint32_t getPosition() { return playback.positionMillis(); };
  uint32_t getLength() { return playback.lengthMillis(); };

  void pause();
  float readPeak() { return peak.available() ? peak.read() : 0.0; };
  void resetPosition() { position = 0; };
  void startPlayback();
  void startRecording();
  void stop();
  void stopPlayback();
  void stopRecording();
  void pausePlayback();
  void pauseRecording();
};

#endif
