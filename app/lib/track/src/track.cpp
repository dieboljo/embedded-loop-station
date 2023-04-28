#include <Arduino.h>
#include <SD.h>
#include <math.h>
#include <track.hpp>

// Audio buffer sizes
const size_t Track::playBufferSize = 65536;    // 64k
const size_t Track::recordBufferSize = 131072; // 128k

// Location of audio buffers
const AudioBuffer::bufType Track::bufferLocation = AudioBuffer::inExt;

// Input gains for recording (overdub or replace) vs playback
const Track::Gain Track::gain = {0.0, 0.8, 1.0};

// Create the read/write files, and configure the audio buffers
bool Track::begin() {
  bool initialized = initializeFiles();

  bool configured = configureBuffers();

  return initialized && configured;
}

// Check if the loop has ended, and restart if true
Status Track::checkLoopEnded(Status status) {
  switch (status) {
  case Status::Stop:
  case Status::Pause:
    return status;
  case Status::Play:
    if (playback.isPlaying()) {
      // Continue
      if (ms > 1000) {
        Serial.println(">");
        ms = 0;
      }
      return status;
    } else if (!playback.isPlaying() && recording.positionMillis()) {
      Serial.println("Looping back from play");
      // End of loop, switch to recorded audio
      return swapBuffers();
    } else if (!playbackFile) {
      if (!loopEstablished && recording.positionMillis()) {
        Serial.println("Setting the base loop");
        return swapBuffers();
      }
      // Nothing recorded yet, wait for initial recording
      Serial.println("Nothing to play yet, record something already!");
      return Status::Stop;
    }
  case Status::Record:
    if (ms > 1000) {
      Serial.println("o");
      ms = 0;
    }
    if (!loopEstablished) {
      // First recording, keep it moving
      return status;
    } else if (!playback.isPlaying()) {
      Serial.println("Looping back from record");
      return swapBuffers();
    }
    return status;
  default:
    Serial.println("This shouldn't be reached");
    return Status::Stop;
  }
}

// SD audio objects need buffers configuring
bool Track::configureBuffers() {
  AudioBuffer::result ok = AudioBuffer::ok;
  bool configured =
      playback.createBuffer(playBufferSize, bufferLocation) == ok &&
      feedback.createBuffer(playBufferSize, bufferLocation) == ok &&
      recording.createBuffer(recordBufferSize, bufferLocation) == ok;
  if (!configured) {
    Serial.println("Failed to configure audio buffers");
  }
  return configured;
}

// Delete and recreate the read and write files
bool Track::initializeFiles() {
  bool success = true;
  if (SD.exists(writeFileName)) {
    success = SD.remove(writeFileName);
  }
  if (SD.exists(readFileName)) {
    success = SD.remove(readFileName);
  }
  if (!success) {
    Serial.println("Failed to remove existing track files");
  }
  // Create the file buffers
  recordingFile = SD.open(writeFileName, FILE_WRITE);
  if (!recordingFile) {
    Serial.println("Failed to create write file");
    success = false;
  }
  recordingFile.close();
  playbackFile = SD.open(readFileName, FILE_WRITE);
  if (!playbackFile) {
    Serial.println("Failed to create read file");
    success = false;
  }
  playbackFile.close();
  return success;
}

void Track::pan(float panPos, Mode mode) {
  if (!isRecording) {
    return;
  }
  if (mode == Mode::Replace) {
    busLeft.gain(Channel::Source, panLeft(gain.solo, panPos));
    busRight.gain(Channel::Source, panRight(gain.solo, panPos));
  } else {
    busLeft.gain(Channel::Source, panLeft(gain.mix, panPos));
    busRight.gain(Channel::Source, panRight(gain.mix, panPos));
  }
}

float Track::panLeft(float gain, float panPos) {
  return gain * cosf(panPos * (M_PI / 2));
}

float Track::panRight(float gain, float panPos) {
  return gain * sinf(panPos * (M_PI / 2));
}

// Pause recording and playback, and disable recording
bool Track::pause() {
  return playback.pause() && feedback.pause() && recording.pause();
}

// Resume playing from a paused state
bool Track::play() {
  punchOut();
  return resume();
}

// Enable recording at the current track position,
// in either replace or overdub mode
void Track::punchIn(Mode mode, float panPos) {
  if (mode == Mode::Replace) {
    busLeft.gain(Channel::Source, panLeft(gain.solo, panPos));
    busLeft.gain(Channel::Feedback, gain.mute);
    busRight.gain(Channel::Source, panRight(gain.solo, panPos));
    busRight.gain(Channel::Feedback, gain.mute);
  } else {
    busLeft.gain(Channel::Source, panLeft(gain.mix, panPos));
    busLeft.gain(Channel::Feedback, gain.mix);
    busRight.gain(Channel::Source, panRight(gain.mix, panPos));
    busRight.gain(Channel::Feedback, gain.mix);
  }
  isRecording = true;
}

// Disable recording immediately
void Track::punchOut() {
  busLeft.gain(Channel::Source, gain.mute);
  busLeft.gain(Channel::Feedback, gain.solo);
  busRight.gain(Channel::Source, gain.mute);
  busRight.gain(Channel::Feedback, gain.solo);
  isRecording = false;
}

// Resume recording from a paused state
bool Track::record(Mode mode, float panPos) {
  punchIn(mode, panPos);
  return resume();
}

// Utility to start all audio streams
bool Track::resume() {
  if (playbackFile && feedbackFile) {
    return playback.play() && feedback.play() && recording.record();
  } else {
    return recording.record();
  }
}

// Opens all file streams in a paused state,
// then starts playing them at once.
// This allows play streams to queue their buffers.
bool Track::start() {
  playbackFile = SD.open(readFileName);
  feedbackFile = SD.open(readFileName);
  Serial.printf("File size: %d\n", playbackFile.size());
  if (playbackFile.size() && feedbackFile.size()) {
    playback.play(playbackFile, true);
    feedback.play(feedbackFile, true);
  } else {
    playbackFile.close();
    feedbackFile.close();
  }
  recordingFile = SD.open(writeFileName, FILE_WRITE_BEGIN);
  recording.record(recordingFile, true);
  return resume();
}

// Start playing from a stopped state
bool Track::startPlaying() {
  punchOut();
  return start();
}

// Start recording from a stopped state
bool Track::startRecording(Mode mode, float panPos) {
  punchIn(mode, panPos);
  return start();
}

// Pause all audio streams, then close them.
// This allows the record buffer to flush to
// its WAV file and update header information.
bool Track::stop(bool cancel) {
  punchOut();
  recording.pause();
  playback.pause();
  feedback.pause();
  recording.stop();
  playback.stop();
  feedback.stop();
  if (cancel) {
    SD.remove(writeFileName);
  }
  return recording.isStopped() && playback.isStopped() && feedback.isStopped();
}

// Rotate read and write file pointers
// whenever a loop reaches its end
Status Track::swapBuffers() {
  if (!stop()) {
    Serial.println("Failed to stop audio streams");
    return Status::Stop;
  }
  AudioNoInterrupts();
  if (!loopEstablished) {
    loopEstablished = true;
  }
  const char *temp = readFileName;
  readFileName = writeFileName;
  writeFileName = temp;
  Serial.printf("Read file: %s, Write file: %s\n", readFileName, writeFileName);
  AudioInterrupts();
  startPlaying();
  return Status::Play;
}

#ifdef USE_USB_INPUT
Track::Track(const char *f1, const char *f2, AudioInputUSB *s)
    : source(s), sourceToBusLeft(*source, 0, busLeft, Channel::Source),
      sourceToBusRight(*source, 1, busRight, Channel::Source),
      feedbackToBusLeft(feedback, 0, busLeft, Channel::Feedback),
      feedbackToBusRight(feedback, 1, busRight, Channel::Feedback),
      busLeftToRecording(busLeft, 0, recording, 0),
      busRightToRecording(busRight, 0, recording, 1), readFileName(f1),
      writeFileName(f2){};
#else
Track::Track(const char *f1, const char *f2, AudioInputI2S *s)
    : source(s), sourceToBusLeft(*source, 0, busLeft, Channel::Source),
      sourceToBusRight(*source, 1, busRight, Channel::Source),
      feedbackToBusLeft(feedback, 0, busLeft, Channel::Feedback),
      feedbackToBusRight(feedback, 1, busRight, Channel::Feedback),
      busLeftToRecording(busLeft, 0, recording, 0),
      busRightToRecording(busRight, 0, recording, 1), readFileName(f1),
      writeFileName(f2){};
#endif

uint32_t Track::getPosition(){
  return playback.positionMillis();
}

uint32_t Track::getLegnth(){
  return playback.lengthMillis();
}