#ifndef TRACK_CONTROLLER_HPP
#define TRACK_CONTROLLER_HPP

#include "track.hpp"

class TrackController {
  struct Gain {
    float replace;
    float overdub;
    float play;
  };

  Track track1;
  Track track2;
  AudioConnection track1ToOutMixLeft;
  AudioConnection track1ToOutMixRight;
  AudioConnection track2ToOutMixLeft;
  AudioConnection track2ToOutMixRight;
  AudioConnection track1ToRecMixLeft;
  AudioConnection track1ToRecMixRight;
  AudioConnection track2ToRecMixLeft;
  AudioConnection track2ToRecMixRight;
  AudioMixer4 recMixLeft;
  AudioMixer4 recMixRight;

  static const Gain gain;
  static const int numTracks = 2;

  bool isRecording = false;
  int baseTrack = -1;
  uint32_t loopLength = 0;
  elapsedMillis ms;
  int selectedTrack = 0;

  float panPos[numTracks] = {0.5, 0.5};
  Track *tracks[numTracks] = {&track1, &track2};
  AudioConnection *patchCords[numTracks * 4] = {
      &track1ToOutMixLeft,  &track1ToOutMixRight, &track2ToOutMixLeft,
      &track2ToOutMixRight, &track1ToRecMixLeft,  &track1ToRecMixRight,
      &track2ToRecMixLeft,  &track2ToRecMixRight};

  void adjustOutput(Mode mode);
  void adjustOutput();
  static float panLeft(float gain, float pos);
  static float panRight(float gain, float pos);
  void patchConnections();
  void printStatus(Status status);
  bool start();
  bool swapBuffers();

public:
#ifdef USE_USB_INPUT
  TrackController(AudioInputUSB &s);
#else
  TrackController(AudioInputI2S &s);
#endif

  AudioMixer4 outMixLeft;
  AudioMixer4 outMixRight;

  bool begin();
  Status checkTracks(Status status);
  void establishLoop();
  uint32_t getPosition() { return tracks[selectedTrack]->getPosition(); };
  int getNumTracks() { return numTracks; };
  int getSelectedTrack() { return selectedTrack; };
  int nextTrack();
  void pan(float panPos, Mode mode);
  bool play();
  bool pause();
  void punchIn(Mode mode);
  void punchOut();
  void punchOut(bool cancel);
  bool record(Mode mode);
  bool startPlaying();
  bool startRecording(Mode mode);
  bool stop(bool cancel = false);
};

#endif
