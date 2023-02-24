#ifndef TRACK_HPP
#define TRACK_HPP

#include <Audio.h>

enum class Mode { Replace, Overdub };
enum class Status { Stop, Record, Play, Pause };
enum Channel { Source, Feedback };

// Gains for overdub and replace modes
struct RecordGain {
  float mute;
  float mix;
  float solo;
};

class Track {

  static const AudioBuffer::bufType bufferLocation;
  static const size_t playBufferSize;
  static const size_t recordBufferSize;
  static const RecordGain recordGain;

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

  bool configureBuffers();
  bool initializeFiles();
  bool resume();
  bool start();
  void swapBuffers();

protected:
  const char *readFileName;
  const char *writeFileName;

public:
  Track(const char *f1, const char *f2, AudioInputI2S *s)
      : source(s), sourceToBus(*source, 0, bus, Channel::Source),
        feedbackToBus(feedback, 0, bus, Channel::Feedback),
        busToPeak(bus, peak), busToRecordingLeft(bus, 0, recording, 0),
        busToRecordingRight(bus, 0, recording, 1), readFileName(f1),
        writeFileName(f2){};

  AudioPlayWAVstereo playback;

  bool advance(Status status);
  bool begin();
  bool play();
  bool pause();
  void punchIn();
  void punchOut();
  float readPeak() { return peak.available() ? peak.read() : 0.0; };
  bool record();
  bool startPlaying();
  bool startRecording();
  bool stop();
};

#endif
