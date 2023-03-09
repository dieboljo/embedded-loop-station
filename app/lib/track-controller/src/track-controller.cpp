#include <track-controller.hpp>

bool TrackController::begin() {
  bool success = true;
  for (auto track : tracks) {
    success = success && track.begin();
  }
  return success;
};

// Check if the loop has ended, and restart if true
Status TrackController::checkLoopEnded(Status status) {
  switch (status) {
  case Status::Stop:
  case Status::Pause:
    return status;
  case Status::Play:
    if (baseTrack == -1) {
      // Nothing recorded yet, wait for initial recording
      Serial.println("Nothing to play yet, record something already!");
      return Status::Stop;
    } else {
      if (millis() % 1000 == 0) {
        Serial.println(">");
      }
    }
    break;
  case Status::Record:
    if (baseTrack == -1) {
      baseTrack = selectedTrack;
    }
    if (millis() % 1000 == 0) {
      Serial.println("o");
    }
    break;
  default:
    Serial.println("This shouldn't be reached");
    break;
  }

  bool loopEnded = tracks[baseTrack].checkLoopEnded(status);
  if (loopEnded) {
    stop();
    swapBuffers();
    startPlaying();
    return Status::Play;
  } else {
    for (int i = 0; i < numTracks; i++) {
      Status trackStatus = i == baseTrack ? status : Status::Play;
      if (tracks[i].checkLoopEnded(trackStatus)) {
        tracks[i].stop();
        tracks[i].swapBuffers();
        tracks[i].startPlaying();
      }
    }
    return status;
  }
}

bool TrackController::pause() {
  bool success = true;
  for (auto track : tracks) {
    success = success && track.pause();
  }
  return success;
};

bool TrackController::play() {
  bool success = true;
  for (auto track : tracks) {
    success = success && track.play();
  }
  return success;
}

void TrackController::punchIn(Mode mode) {
  for (int i = 0; i < numTracks; i++) {
    if (i == selectedTrack) {
      tracks[i].punchIn(mode);
    } else {
      tracks[i].punchOut();
    }
  }
}

void TrackController::punchOut() {
  for (auto track : tracks) {
    track.punchOut();
  }
}

bool TrackController::record(Mode mode) {
  bool success = true;
  for (int i = 0; i < numTracks; i++) {
    success = success &&
              (i == selectedTrack ? tracks[i].record(mode) : tracks[i].play());
  }
  return success;
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
  bool success = true;
  for (auto track : tracks) {
    success = success && track.startPlaying();
  }
  return success;
};

bool TrackController::startRecording(Mode mode) {
  bool success = true;
  for (int i = 0; i < numTracks; i++) {
    success = success && (i == selectedTrack ? tracks[i].startRecording(mode)
                                             : tracks[i].startPlaying());
  }
  return success;
};

bool TrackController::stop(bool cancel) {
  bool success = true;
  for (auto track : tracks) {
    success = success && track.stop(cancel);
  }
  return success;
};

void TrackController::swapBuffers() {
  for (auto track : tracks) {
    track.swapBuffers();
  }
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
