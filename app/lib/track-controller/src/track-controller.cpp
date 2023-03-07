#include <track-controller.hpp>

bool TrackController::begin() {
  bool beginSuccess = true;
  for (auto track : tracks) {
    beginSuccess = beginSuccess && track.begin();
  }
  return beginSuccess;
};

void TrackController::setSelectedTrack(int track) {
  // If recording, stop
  tracks[selectedTrack].punchOut();
  if (track < 0 || track > numTracks - 1) {
    return;
  }
  selectedTrack = track;
};

bool TrackController::startPlaying() {
  bool startSuccess = true;
  for (auto track : tracks) {
    startSuccess = startSuccess && track.startPlaying();
  }
  return startSuccess;
};

bool TrackController::startRecording(Mode mode) {
  bool startSuccess = true;
  for (int i = 0; i < numTracks; i++) {
    if (i == selectedTrack) {
      tracks[i].startRecording(mode);
    } else {
      tracks[i].startPlaying();
    }
  }
  return startSuccess;
};

TrackController::TrackController(AudioInputI2S *s)
    : tracks{Track("file-1-a.wav", "file-1-b.wav", s),
             Track("file-2-a.wav", "file-2-b.wav", s),
             Track("file-3-a.wav", "file-3-b.wav", s)},
      trackToMixLeft{AudioConnection(tracks[0].playback, 0, mixLeft, 0),
                     AudioConnection(tracks[1].playback, 0, mixLeft, 1),
                     AudioConnection(tracks[2].playback, 0, mixLeft, 2)},
      trackToMixRight{
          AudioConnection(tracks[0].playback, 1, mixRight, 0),
          AudioConnection(tracks[1].playback, 1, mixRight, 1),
          AudioConnection(tracks[2].playback, 1, mixRight, 2),
      } {};
