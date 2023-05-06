#include <track-controller.hpp>

const TrackController::Gain TrackController::gain = {0.0, 0.3, 1.0};

size_t TrackController::controllerId = 0;

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

  for (auto track : tracks) {
    if (track->checkEnded(loopLength)) {
      // End of loop, switch to recorded audio
      Serial.println("Looping back to start");
      swapBuffers();
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
  AudioNoInterrupts();
  for (auto track : tracks) {
    success = success && track->pause();
  }
  AudioInterrupts();
  return success;
};

void TrackController::pan(float pos, Mode mode) {
  panPos[selectedTrack] = pos;
  AudioNoInterrupts();
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
  AudioNoInterrupts();
  for (auto track : tracks) {
    success = success && track->resume();
  }
  AudioInterrupts();
  return success;
}

void TrackController::printStatus(Status status) {
  if (ms > 1000) {
    Serial.println(status == Status::Play ? ">" : "o");
    ms = 0;
  }
}

void TrackController::punchIn(Mode mode) {
  isRecording = true;
  AudioNoInterrupts();
  adjustOutput(mode);
  tracks[selectedTrack]->punchIn(mode);
  AudioInterrupts();
}

void TrackController::punchOut() {
  if (isRecording && !loopLength)
    establishLoop();
  isRecording = false;
  AudioNoInterrupts();
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
  AudioNoInterrupts();
  for (auto track : tracks) {
    success = success && track->resume();
  }
  AudioInterrupts();
  return success;
};

bool TrackController::start() {
  bool success = true;
  AudioNoInterrupts();
  for (auto track : tracks) {
    success = success && track->start();
  }
  AudioInterrupts();
  return success;
}

bool TrackController::startPlaying() {
  punchOut();
  return start();
};

bool TrackController::startRecording(Mode mode) {
  punchIn(mode);
  return start();
};

bool TrackController::stop(bool cancel) {
  punchOut(cancel);
  bool success = true;
  AudioNoInterrupts();
  for (auto track : tracks) {
    success = success && track->stop(cancel);
  }
  AudioInterrupts();
  return success;
};

bool TrackController::swapBuffers() {
  if (!stop()) {
    Serial.println("Failed to stop audio streams");
    return false;
  }
  for (auto track : tracks) {
    track->swapBuffers();
  }
  return start();
};

#ifdef USE_USB_INPUT
TrackController::TrackController(AudioInputUSB &s) {
  for (int i = 0; i < numTracks; i++) {
    panPos[i] = 0.5;

    int b1 = i * 2 * 20;
    char *f1 = &filenames[b1];
    snprintf(f1, 20, "track-%d-%d-a.wav", controllerId, i);

    int b2 = b1 + 20;
    char *f2 = &filenames[b2];
    snprintf(f2, 20, "track-%d-%d-b.wav", controllerId, i);

    tracks[i] = new Track(f1, f2, s);

    patchCords[i * 4] =
        new AudioConnection(tracks[i]->playback, 0, outMixLeft, i);
    patchCords[i * 4 + 1] =
        new AudioConnection(tracks[i]->playback, 1, outMixRight, i);
    patchCords[i * 4 + 2] =
        new AudioConnection(tracks[i]->playback, 0, recMixLeft, i);
    patchCords[i * 4 + 3] =
        new AudioConnection(tracks[i]->playback, 1, recMixRight, i);
  }

  controllerId++;
};
#else
TrackController::TrackController(AudioInputI2S &s) {
  for (int i = 0; i < numTracks; i++) {
    panPos[i] = 0.5;

    int b1 = i * 2 * 20;
    char *f1 = &filenames[b1];
    snprintf(f1, 20, "track-%d-%d-a.wav", controllerId, i);

    int b2 = b1 + 20;
    char *f2 = &filenames[b2];
    snprintf(f2, 20, "track-%d-%d-b.wav", controllerId, i);

    tracks[i] = new Track(f1, f2, s);

    patchCords[i * 4] =
        new AudioConnection(tracks[i]->playback, 0, outMixLeft, i);
    patchCords[i * 4 + 1] =
        new AudioConnection(tracks[i]->playback, 1, outMixRight, i);
    patchCords[i * 4 + 2] =
        new AudioConnection(tracks[i]->playback, 0, recMixLeft, i);
    patchCords[i * 4 + 3] =
        new AudioConnection(tracks[i]->playback, 1, recMixRight, i);
  }

  controllerId++;
};
#endif
