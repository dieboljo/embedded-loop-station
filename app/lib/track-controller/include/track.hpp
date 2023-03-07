#ifndef TRACK_HPP
#define TRACK_HPP

#include "types.hpp"
#include <Audio.h>

class Track {

  enum Channel { Source, Feedback };

  // Gains for overdub and replace modes
  struct RecordGain {
    float mute;
    float mix;
    float solo;
  };

  static const AudioBuffer::bufType bufferLocation;
  static const size_t playBufferSize;
  static const size_t recordBufferSize;
  static const RecordGain recordGain;

  bool loopEstablished;

  File playbackFile;
  File recordingFile;
  File feedbackFile;

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
  Status swapBuffers();

protected:
  const char *readFileName;
  const char *writeFileName;

public:
  Track(const char *f1, const char *f2, AudioInputI2S *s);

  AudioPlayWAVstereo playback;

  bool begin();
  Status checkLoopEnded(Status status);
  bool play();
  bool pause();
  void punchIn(Mode mode);
  void punchOut();
  float readPeak() { return peak.available() ? peak.read() : 0.0; };
  bool record(Mode mode);
  bool startPlaying();
  bool startRecording(Mode mode);
  bool stop(bool cancel = false);
};

#endif
