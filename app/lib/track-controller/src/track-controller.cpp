#include <track-controller.hpp>

const TrackController::Gain TrackController::gain = {0.0, 0.8, 1.0};

bool TrackController::begin() {
  bool success = true;
  for (auto track : tracks) {
    success = success && track->begin();
  }
  return success;
};

// Check if the loop has ended, and restart if true
Status TrackController::checkTracks(Status status) {
  switch (status) {
  case Status::Stop:
  case Status::Pause:
    return status;
  case Status::Play:
    if (baseTrack == -1) {
      // Nothing recorded yet, wait for initial recording
      Serial.println("Nothing to play yet, record something already!");
      return Status::Stop;
    }
    if (ms > 1000) {
      Serial.println(tracks[selectedTrack]->playback.isPlaying());
      Serial.println(tracks[selectedTrack]->playback.positionMillis());
      Serial.println(tracks[selectedTrack]->playback.lengthMillis());
      Serial.println(">");
      ms = 0;
    }
    break;
  case Status::Record:
    if (baseTrack == -1)
      baseTrack = selectedTrack;
    if (!tracks[selectedTrack]->isRecording)
      return Status::Play;
    if (ms > 1000) {
      Serial.println("o");
      ms = 0;
    }
    break;
  default:
    Serial.println("This shouldn't be reached");
    break;
  }

  for (int i = 0; i < numTracks; i++) {
    Status trackStatus = i == selectedTrack ? status : Status::Play;
    tracks[i]->checkLoop(trackStatus);
  }
  return status;
}

bool TrackController::pause() {
  bool success = true;
  for (auto track : tracks) {
    success = success && track->pause();
  }
  return success;
};

void TrackController::pan(float panPos, Mode mode) {
  tracks[selectedTrack]->pan(panPos, mode);
}

bool TrackController::play() {
  mixLeft.gain(0, gain.play);
  mixRight.gain(0, gain.play);
  bool success = true;
  for (auto track : tracks) {
    success = success && track->play();
  }
  return success;
}

void TrackController::punchIn(Mode mode, float panPos) {
  tracks[selectedTrack]->punchIn(mode, panPos);
}

void TrackController::punchOut() {
  for (auto track : tracks) {
    track->punchOut();
  }
}

bool TrackController::record(Mode mode, float panPos) {
  bool success = true;
  for (int i = 0; i < numTracks; i++) {
    success = success && (i == selectedTrack ? tracks[i]->record(mode, panPos)
                                             : tracks[i]->play());
  }
  return success;
};

void TrackController::setSelectedTrack(int track) {
  // If recording, stop
  tracks[selectedTrack]->punchOut();
  if (track < 0 || track > numTracks - 1) {
    return;
  }
  selectedTrack = track;
};

bool TrackController::startPlaying() {
  mixLeft.gain(0, gain.play);
  mixRight.gain(0, gain.play);
  bool success = true;
  for (auto track : tracks) {
    success = success && track->startPlaying();
  }
  return success;
};

bool TrackController::startRecording(Mode mode, float panPos) {
  bool success = true;
  for (int i = 0; i < numTracks; i++) {
    success =
        success && (i == selectedTrack ? tracks[i]->startRecording(mode, panPos)
                                       : tracks[i]->startPlaying());
  }
  return success;
};

bool TrackController::stop(bool cancel) {
  bool success = true;
  for (auto track : tracks) {
    success = success && track->stop(cancel);
  }
  return success;
};

#ifdef USE_USB_INPUT
TrackController::TrackController(AudioInputUSB &s)
    : track1("file-1-a.wav", "file-1-b.wav", s) {}
/* : tracks{
      Track("file-1-a.wav", "file-1-b.wav", s),
      Track("file-2-a.wav", "file-2-b.wav", s),
      Track("file-3-a.wav", "file-3-b.wav", s),
  },
  trackToMixLeft{
      AudioConnection(tracks[0].playback, 0, mixLeft, 0),
      AudioConnection(tracks[1].playback, 0, mixLeft, 1),
      AudioConnection(tracks[2].playback, 0, mixLeft, 2),
  },
  trackToMixRight{
      AudioConnection(tracks[0].playback, 1, mixRight, 0),
      AudioConnection(tracks[1].playback, 1, mixRight, 1),
      AudioConnection(tracks[2].playback, 1, mixRight, 2),
  } {}; */
#else
TrackController::TrackController(AudioInputI2S &s)
    : track1("file-1-a.wav", "file-1-b.wav", s),
      track1ToMixLeft(track1.playback, 0, mixLeft, 0),
      track1ToMixRight(track1.playback, 1, mixRight, 0){};
/*    : tracks{
          Track("file-1-a.wav", "file-1-b.wav", s),
          Track("file-2-a.wav", "file-2-b.wav", s),
          Track("file-3-a.wav", "file-3-b.wav", s),
          },
          trackToMixLeft{
              AudioConnection(tracks[0].playback, 0, mixLeft, 0),
              AudioConnection(tracks[1].playback, 0, mixLeft, 1),
              AudioConnection(tracks[2].playback, 0, mixLeft, 2),
          },
          trackToMixRight{
              AudioConnection(tracks[0].playback, 1, mixRight, 0),
              AudioConnection(tracks[1].playback, 1, mixRight, 1),
              AudioConnection(tracks[2].playback, 1, mixRight, 2),
      } {}; */
#endif
