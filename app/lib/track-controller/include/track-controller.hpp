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
  AudioConnection track1ToMixLeft;
  AudioConnection track1ToMixRight;
  AudioConnection track2ToMixLeft;
  AudioConnection track2ToMixRight;

  static const Gain gain;
  static const int numTracks = 2;

  int baseTrack = -1;
  elapsedMillis ms;
  int selectedTrack = 0;
  Track *tracks[numTracks] = {&track1, &track2};

  void patchMixer();
  void printStatus(Status status);

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
