#ifndef TRACK_HPP
#define TRACK_HPP

#include <Audio.h>

enum class Mode { Replace, Overdub };
enum class Status { Stop, Record, Play, Pause };
enum Channel { Source, Feedback };

// Gains for overdub and replace modes
const struct Gain {
  float mute;
  float mix;
  float solo;
} gain = {0.0, 0.4, 0.8};

// Audio buffer sizes
const size_t playBufferSize = 65536;
const size_t recordBufferSize = 131072;

// Location of audio buffers
const AudioBuffer::bufType bufferLocation = AudioBuffer::inExt;

class Track {
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

  // uint32_t closeWriteBuffer();
  bool configureBuffers();
  bool initializeFiles();
  bool resume();
  bool start();

protected:
  const char *readFileName;
  const char *writeFileName;
  void swapBuffers();

public:
  Track(const char *f1, const char *f2, AudioInputI2S *s)
      : source(s), sourceToBus(*source, 0, bus, Channel::Source),
        feedbackToBus(feedback, 0, bus, Channel::Feedback),
        busToPeak(bus, peak), busToRecordingLeft(bus, 0, recording, 0),
        busToRecordingRight(bus, 0, recording, 1), readFileName(f1),
        writeFileName(f2){};

  AudioPlayWAVstereo playback;

  // bool advance(Status status);
  bool begin();

  // TODO: Remove
  uint32_t getPosition() { return playback.positionMillis(); };
  uint32_t getLength() { return playback.lengthMillis(); };

  bool play();
  bool pause();
  void punchIn();
  void punchOut();
  float readPeak() { return peak.available() ? peak.read() : 0.0; };
  bool record();
  bool startPlaying();
  bool startRecording();
  // void stop();
};

#endif
