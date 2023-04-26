#include <track-controller.hpp>

const TrackController::Gain TrackController::gain = {0.0, 0.3, 1.0};

void TrackController::adjustOutput() {
  outMixLeft.gain(selectedTrack, gain.play);
  outMixRight.gain(selectedTrack, gain.play);
}

void TrackController::adjustOutput(Mode mode) {
  float outputGain = mode == Mode::Overdub ? gain.overdub : gain.replace;
  outMixLeft.gain(selectedTrack, outputGain);
  outMixRight.gain(selectedTrack, outputGain);
}

bool TrackController::begin() {
  bool success = true;
  for (auto track : tracks) {
    success = success && track->begin();
  }
  patchConnections();
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
    break;
  case Status::Record:
    if (baseTrack == -1)
      baseTrack = selectedTrack;
    if (!tracks[selectedTrack]->isRecording)
      return Status::Play;
    break;
  default:
    Serial.println("This shouldn't be reached");
    break;
  }

  printStatus(status);

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

void TrackController::patchConnections() {
  for (auto connection : patchCords) {
    connection->connect();
  }
}

bool TrackController::play() {
  bool success = true;
  for (auto track : tracks) {
    success = success && track->play();
  }
  return success;
}

void TrackController::printStatus(Status status) {
  if (ms > 1000) {
    Serial.println(status == Status::Play ? ">" : "o");
    ms = 0;
  }
}

void TrackController::punchIn(Mode mode, float panPos) {
  adjustOutput(mode);
  tracks[selectedTrack]->punchIn(mode, panPos);
}

void TrackController::punchOut() {
  adjustOutput();
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
    : track1("file-1-a.wav", "file-1-b.wav", s),
      track2("file-2-a.wav", "file-2-b.wav", s),
      track1ToOutMixLeft(track1.playback, 0, outMixLeft, 0),
      track1ToOutMixRight(track1.playback, 1, outMixRight, 0),
      track2ToOutMixLeft(track2.playback, 0, outMixLeft, 1),
      track2ToOutMixRight(track2.playback, 1, outMixRight, 1),
      track1ToRecMixLeft(track1.playback, 0, recMixLeft, 0),
      track1ToRecMixRight(track1.playback, 1, recMixRight, 0),
      track2ToRecMixLeft(track2.playback, 0, recMixLeft, 1),
      track2ToRecMixRight(track2.playback, 1, recMixRight, 1){};
#else
TrackController::TrackController(AudioInputI2S &s)
    : track1("file-1-a.wav", "file-1-b.wav", s),
      track2("file-2-a.wav", "file-2-b.wav", s),
      track1ToOutMixLeft(track1.playback, 0, outMixLeft, 0),
      track1ToOutMixRight(track1.playback, 1, outMixRight, 0),
      track2ToOutMixLeft(track2.playback, 0, outMixLeft, 1),
      track2ToOutMixRight(track2.playback, 1, outMixRight, 1),
      track1ToRecMixLeft(track1.playback, 0, recMixLeft, 0),
      track1ToRecMixRight(track1.playback, 1, recMixRight, 0),
      track2ToRecMixLeft(track2.playback, 0, recMixLeft, 1),
      track2ToRecMixRight(track2.playback, 1, recMixRight, 1){};
#endif
