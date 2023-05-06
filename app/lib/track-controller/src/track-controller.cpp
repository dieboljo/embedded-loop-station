#include <track-controller.hpp>

const TrackController::Gain TrackController::gain = {0.0, 0.3, 1.0};

void TrackController::adjustOutput() {
  float pp = panPos[selectedTrack];
  recMixLeft.gain(selectedTrack, panLeft(gain.play, pp));
  recMixRight.gain(selectedTrack, panRight(gain.play, pp));
  outMixLeft.gain(selectedTrack, panLeft(gain.play, pp));
  outMixRight.gain(selectedTrack, panRight(gain.play, pp));
}

void TrackController::adjustOutput(Mode mode) {
  float pp = panPos[selectedTrack];
  recMixLeft.gain(selectedTrack, panLeft(gain.play, pp));
  recMixRight.gain(selectedTrack, panRight(gain.play, pp));
  if (isRecording) {
    float outputGain = mode == Mode::Overdub ? gain.overdub : gain.replace;
    outMixLeft.gain(selectedTrack, panLeft(outputGain, pp));
    outMixRight.gain(selectedTrack, panRight(outputGain, pp));
  } else {
    outMixLeft.gain(selectedTrack, panLeft(gain.play, pp));
    outMixRight.gain(selectedTrack, panRight(gain.play, pp));
  }
}

bool TrackController::begin() {
  bool success = true;
  for (auto track : tracks) {
    success = success && track->begin();
  }
  patchConnections();
  punchOut();
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
  bool success = true;
  for (auto track : tracks) {
    success = success && track->pause();
  }
  return success;
};

void TrackController::pan(float pos, Mode mode) {
  AudioNoInterrupts();
  panPos[selectedTrack] = pos;
  adjustOutput(mode);
  AudioInterrupts();
}

float TrackController::panLeft(float gain, float pan) {
  return gain * cosf(pan * (M_PI / 2));
}

float TrackController::panRight(float gain, float pan) {
  return gain * sinf(pan * (M_PI / 2));
}

void TrackController::patchConnections() {
  for (auto connection : patchCords) {
    connection->connect();
  }
}

bool TrackController::play() {
  punchOut();
  bool success = true;
  for (auto track : tracks) {
    success = success && track->resume();
  }
  return success;
}

void TrackController::printStatus(Status status) {
  if (ms > 1000) {
    Serial.println(status == Status::Play ? ">" : "o");
    ms = 0;
  }
}

void TrackController::punchIn(Mode mode) {
  AudioNoInterrupts();
  isRecording = true;
  adjustOutput(mode);
  tracks[selectedTrack]->punchIn(mode);
  AudioInterrupts();
}

void TrackController::punchOut() {
  AudioNoInterrupts();
  if (isRecording && !loopLength)
    establishLoop();
  isRecording = false;
  adjustOutput();
  for (auto track : tracks) {
    track->punchOut();
  }
  AudioInterrupts();
}

void TrackController::punchOut(bool cancel) {
  AudioNoInterrupts();
  if (!cancel && !loopLength && isRecording)
    establishLoop();
  isRecording = false;
  adjustOutput();
  for (auto track : tracks) {
    track->punchOut();
  }
  AudioInterrupts();
}

bool TrackController::record(Mode mode) {
  punchIn(mode);
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

bool TrackController::startRecording(Mode mode) {
  punchIn(mode);
  bool success = true;
  for (auto track : tracks) {
    success = success && track->start();
  }
  return success;
};

bool TrackController::stop(bool cancel) {
  punchOut(cancel);
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
