#ifndef TRACK_CONTROLLER_HPP
#define TRACK_CONTROLLER_HPP

#include "track.hpp"

class TrackController {
  struct Gain {
    float replace;
    float overdub;
    float play;
  };
  static const Gain gain;

  // static const int numTracks = 3;
  static const int numTracks = 1;
  int selectedTrack = 0;
  int baseTrack = -1;

  Track track1;
  AudioConnection track1ToMixLeft;
  AudioConnection track1ToMixRight;

  Track *tracks[numTracks] = {&track1};

  elapsedMillis ms;

public:
#ifdef USE_USB_INPUT
  TrackController(AudioInputUSB &s);
#else
  TrackController(AudioInputI2S &s);
#endif

  AudioMixer4 mixLeft;
  AudioMixer4 mixRight;

  bool begin();
  Status checkTracks(Status status);
  int getNumTracks() { return numTracks; };
  int getSelectedTrack() { return selectedTrack; };
  void pan(float panPos, Mode mode);
  bool play();
  bool pause();
  void punchIn(Mode mode, float panPos);
  void punchOut();
  bool record(Mode mode, float panPos);
  void setSelectedTrack(int track);
  bool startPlaying();
  bool startRecording(Mode mode, float panPos);
  bool stop(bool cancel = false);
};

#endif
