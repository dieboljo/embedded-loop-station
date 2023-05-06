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
    if (!loopLength) {
      // Nothing recorded yet, wait for initial recording
      Serial.println("Nothing to play yet, record something already!");
      return Status::Stop;
    }
    break;
  case Status::Record:
  default:
    break;
  }

  printStatus(status);

  for (int i = 0; i < numTracks; i++) {
    Status trackStatus = i == selectedTrack ? status : Status::Play;
    bool trackEnded = tracks[i]->checkEnded(trackStatus, loopLength);
    if (i == selectedTrack && trackEnded) {
      punchOut();
      return Status::Play;
    }
  }
  return status;
}

void TrackController::establishLoop() {
  uint32_t position = tracks[selectedTrack]->getPosition();
  if (position) {
    Serial.println("Setting the base loop");
    loopLength = position;
    for (auto track : tracks) {
      track->establishLoop();
    }
  }
}

int TrackController::nextTrack() {
  // If recording, stop
  punchOut();
  selectedTrack = (selectedTrack + 1) % numTracks;
  return selectedTrack;
};

bool TrackController::pause() {
  if (!loopLength)
    establishLoop();
  bool success = true;
  for (auto track : tracks) {
    success = success && track->pause();
  }
  return success;
};

void TrackController::pan(float panPos, Mode mode) {
  if (!isRecording)
    return;
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
  isRecording = true;
  adjustOutput(mode);
  tracks[selectedTrack]->punchIn(mode, panPos);
}

void TrackController::punchOut() {
  isRecording = false;
  adjustOutput();
  if (!loopLength)
    establishLoop();
  for (auto track : tracks) {
    track->punchOut();
  }
}

bool TrackController::record(Mode mode, float panPos) {
  punchIn(mode, panPos);
  bool success = true;
  for (auto track : tracks) {
    success = success && track->resume();
  }
  return success;
};

bool TrackController::startPlaying() {
  punchOut();
  bool success = true;
  for (auto track : tracks) {
    success = success && track->start();
  }
  return success;
};

bool TrackController::startRecording(Mode mode, float panPos) {
  punchIn(mode, panPos);
  bool success = true;
  for (auto track : tracks) {
    success = success && track->start();
  }
  return success;
};

bool TrackController::stop(bool cancel) {
  punchOut();
  if (!loopLength && !cancel)
    establishLoop();
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
