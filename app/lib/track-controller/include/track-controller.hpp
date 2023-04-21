#ifndef TRACK_CONTROLLER_HPP
#define TRACK_CONTROLLER_HPP

#include "track.hpp"

class TrackController {
  static const int numTracks = 3;
  int selectedTrack = 0;
  int baseTrack = -1;

  Track tracks[numTracks];

  AudioConnection trackToMixLeft[numTracks];
  AudioConnection trackToMixRight[numTracks];

public:
  TrackController(AudioInputI2S *s);

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
