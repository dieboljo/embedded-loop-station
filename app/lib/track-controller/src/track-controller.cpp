#include <track-controller.hpp>

// Audio buffer size
const size_t TrackController::recordBufferSize = 131072; // 128k

// Location of audio buffer
const AudioBuffer::bufType TrackController::bufferLocation = AudioBuffer::inExt;

// Monitor gain levels
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
  AudioBuffer::result ok = AudioBuffer::ok;

  // Configure loop record buffer
  success =
      success && recording.createBuffer(recordBufferSize, bufferLocation) == ok;
  if (!success) {
    Serial.println("Failed to configure loop record buffer");
  }

  // Delete and recreate the read and write files
  if (SD.exists(writeFileName)) {
    success = SD.remove(writeFileName);
  }
  if (SD.exists(readFileName)) {
    success = SD.remove(readFileName);
  }
  if (!success) {
    Serial.println("Failed to remove existing loop files");
  }

  for (auto track : tracks) {
    success = success && track->begin();
  }

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
      Serial.printf("File size: %d\n", recordingFile.size());
      // End of loop, switch to recorded audio
      Serial.println("Looping back to start");
      swapBuffers();
      return Status::Play;
    }
  }
  return status;
}

void TrackController::clearTrack() {
  stop(true);
  tracks[selectedTrack]->clear();
  Serial.printf("Track %d cleared\n", selectedTrack + 1);
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

void TrackController::save() {
  // For now, discard current changes until a way
  // to quickly merge a partial loop is implemented
  stop(true);
  char label[30];
  snprintf(label, 30, "/loops/%ld.wav", Teensy3Clock.get());
  AudioNoInterrupts();
  if (!SD.exists("/loops")) {
    SD.mkdir("/loops");
  }
  if (SD.exists(label)) {
    SD.remove(label);
  }
  File writeFile = SD.open(label, FILE_WRITE_BEGIN);
  File readFile = SD.open(readFileName);
  if (!writeFile || !readFile) {
    Serial.println("Failed to save track");
    return;
  }
  Serial.print("Saving track");
  byte buf[512];
  int bufSize = sizeof(buf);
  while (readFile.available()) {
    if (ms > 1000) {
      Serial.print(".");
      ms = 0;
    }
    int nbytes = readFile.available();
    if (nbytes > bufSize) {
      readFile.read(buf, bufSize);
      writeFile.write(buf, bufSize);
    } else {
      readFile.read(buf, nbytes);
      writeFile.write(buf, nbytes);
    }
  }
  readFile.close();
  writeFile.close();
  Serial.print("\nSaved track to file: ");
  Serial.println(label);
  AudioInterrupts();
}

bool TrackController::start() {
  bool success = true;
  AudioNoInterrupts();
  recordingFile = SD.open(writeFileName, FILE_WRITE_BEGIN);
  recording.record(recordingFile, true);

  for (auto track : tracks) {
    success = success && track->start();
  }

  success = success && recording.record();
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
  recording.pause();
  for (auto track : tracks) {
    success = success && track->stop(cancel);
  }
  recording.stop();
  if (cancel) {
    SD.remove(writeFileName);
  }
  AudioInterrupts();
  return success;
};

bool TrackController::swapBuffers() {
  if (!stop()) {
    Serial.println("Failed to stop audio streams");
    return false;
  }

  char *temp = readFileName;
  readFileName = writeFileName;
  writeFileName = temp;
  Serial.printf("Read file: %s, Write file: %s\n", readFileName, writeFileName);

  for (auto track : tracks) {
    track->swapBuffers();
  }

  return start();
};

#ifdef USE_USB_INPUT
TrackController::TrackController(AudioInputUSB &s)
    : source(s), recMixLeftToRecording(recMixLeft, 0, recording, 0),
      recMixRightToRecording(recMixRight, 0, recording, 1) {
  readFileName = &recFileNames[0];
  snprintf(readFileName, fileNameSize, "mix-%d-a.wav", controllerId);

  writeFileName = &recFileNames[fileNameSize];
  snprintf(writeFileName, fileNameSize, "mix-%d-b.wav", controllerId);

  for (int i = 0; i < numTracks; i++) {
    createTrack(i);
  }

  controllerId++;
};
#else
TrackController::TrackController(AudioInputI2S &s)
    : source(s), recMixLeftToRecording(recMixLeft, 0, recording, 0),
      recMixRightToRecording(recMixRight, 0, recording, 1) {
  readFileName = &recFileNames[0];
  snprintf(readFileName, fileNameSize, "mix-%d-a.wav", controllerId);

  writeFileName = &recFileNames[fileNameSize];
  snprintf(writeFileName, fileNameSize, "mix-%d-b.wav", controllerId);

  for (int i = 0; i < numTracks; i++) {
    createTrack(i);
  }

  controllerId++;
};
#endif

void TrackController::createTrack(int trackIdx) {
  panPos[trackIdx] = 0.5;

  size_t b1 = trackIdx * 2 * fileNameSize;
  char *f1 = &trackFileNames[b1];
  snprintf(f1, fileNameSize, "track-%d-%d-a.wav", controllerId, trackIdx);

  size_t b2 = b1 + fileNameSize;
  char *f2 = &trackFileNames[b2];
  snprintf(f2, fileNameSize, "track-%d-%d-b.wav", controllerId, trackIdx);

  tracks[trackIdx] = new Track(f1, f2, source);

  trackConnections[trackIdx * 4] =
      new AudioConnection(tracks[trackIdx]->playback, 0, outMixLeft, trackIdx);
  trackConnections[trackIdx * 4 + 1] =
      new AudioConnection(tracks[trackIdx]->playback, 1, outMixRight, trackIdx);
  trackConnections[trackIdx * 4 + 2] =
      new AudioConnection(tracks[trackIdx]->playback, 0, recMixLeft, trackIdx);
  trackConnections[trackIdx * 4 + 3] =
      new AudioConnection(tracks[trackIdx]->playback, 1, recMixRight, trackIdx);
}
