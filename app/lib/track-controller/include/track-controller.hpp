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
  void swapBuffers();

public:
  TrackController(AudioInputI2S *s);

  AudioMixer4 mixLeft;
  AudioMixer4 mixRight;

  bool begin();
  Status checkLoopEnded(Status status);
  int getNumTracks() { return numTracks; };
  int getSelectedTrack() { return selectedTrack; };
  bool play();
  bool pause();
  void punchIn(Mode mode);
  void punchOut();
  float readPeak() { return tracks[selectedTrack].readPeak(); };
  bool record(Mode mode);
  void setSelectedTrack(int track);
  bool startPlaying();
  bool startRecording(Mode mode);
  bool stop(bool cancel = false);
};

#endif
