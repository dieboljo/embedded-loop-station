#ifndef TRACK_HPP
#define TRACK_HPP

#include "types.hpp"
#include <Audio.h>

class Track {

  enum Channel { Source, Feedback };

  // Gains for overdub and replace modes
  struct Gain {
    float mute;
    float mix;
    float solo;
  };

  static const AudioBuffer::bufType bufferLocation;
  static const size_t playBufferSize;
  static const size_t recordBufferSize;
  static const Gain gain;

  bool isRecording = false;
  bool loopEstablished = false;

  File playbackFile;
  File recordingFile;
  File feedbackFile;

  // Order of signal flow
  AudioInputI2S *source;
  AudioMixer4 busLeft;
  AudioMixer4 busRight;
  AudioRecordWAVstereo recording;
  AudioPlayWAVstereo feedback;

  AudioConnection sourceToBusLeft;
  AudioConnection sourceToBusRight;
  AudioConnection feedbackToBusLeft;
  AudioConnection feedbackToBusRight;
  AudioConnection busLeftToRecording;
  AudioConnection busRightToRecording;

  bool configureBuffers();
  bool initializeFiles();
  float panLeft(float gain, float panPos);
  float panRight(float gain, float panPos);
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
  void pan(float panPos, Mode mode);
  bool play();
  bool pause();
  void punchIn(Mode mode, float pan);
  void punchOut();
  bool record(Mode mode, float pan);
  bool startPlaying();
  bool startRecording(Mode mode, float pan);
  bool stop(bool cancel = false);
};

#endif
