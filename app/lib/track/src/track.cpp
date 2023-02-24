#include "Audio.h"
#include <Arduino.h>
#include <SD.h>
#include <track.hpp>

// Audio buffer sizes
// const size_t Track::playBufferSize = 65536;
const size_t Track::playBufferSize = 32768;
// const size_t Track::recordBufferSize = 131072;
const size_t Track::recordBufferSize = 32768;

// Location of audio buffers
const AudioBuffer::bufType Track::bufferLocation = AudioBuffer::inExt;

// Input gains for recording (overdub or replace) vs playback
const RecordGain Track::recordGain = {0.0, 0.4, 0.8};

// Create the read/write files, and configure the audio buffers
bool Track::begin() {
  // bool initialized = initializeFiles();

  bool configured = configureBuffers();

  // return initialized && configured;
  return configured;
}

// Check if the loop has ended, and restart if true
bool Track::checkLoopEnded(Status status) {
  if (status == Status::Stop) {
    return false;
  }
  if (!playback.isPlaying()) {
    swapBuffers();
    return startPlaying();
  }
  return false;
}

// SD audio objects need buffers configuring
bool Track::configureBuffers() {
  AudioBuffer::result ok = AudioBuffer::ok;
  bool configured =
      playback.createBuffer(playBufferSize, bufferLocation) == ok &&
      // feedback.createBuffer(playBufferSize, bufferLocation) == ok &&
      recording.createBuffer(recordBufferSize, bufferLocation) == ok;
  if (!configured) {
    Serial.println("Failed to configure audio buffers");
  }
  return configured;
}

// Delete and recreate the read and write files
bool Track::initializeFiles() {
  bool removed = true;
  if (SD.exists(writeFileName)) {
    removed = SD.remove(writeFileName);
  }
  if (SD.exists(readFileName)) {
    removed = SD.remove(readFileName);
  }
  if (!removed) {
    Serial.println("Failed to remove existing track files");
  }
  // Create the read file buffers
  File temp = SD.open(writeFileName, FILE_WRITE);
  temp.close();
  temp = SD.open(readFileName, FILE_WRITE);
  temp.close();
  return removed;
}

// Pause recording and playback, and disable recording
bool Track::pause() {
  // return playback.pause() && feedback.pause() && recording.pause();
  return recording.pause();
}

// Resume playing from a paused state
bool Track::play() {
  // punchOut();
  return resume();
}

// Enable recording at the current track position,
// in either replace or overdub mode
// TODO: Impement overdub mode
void Track::punchIn() {
  bus.gain(Channel::Source, recordGain.solo);
  // bus.gain(Channel::Feedback, recordGain.mute);
}

// Disable recording immediately
void Track::punchOut() {
  bus.gain(Channel::Source, recordGain.mute);
  // bus.gain(Channel::Feedback, recordGain.solo);
}

// Resume recording from a paused state
bool Track::record() {
  // punchIn();
  return resume();
}

// Utility to start all audio streams
bool Track::resume() {
  // return playback.play() && feedback.play() && recording.record();
  recording.record();
  playback.play();
  return true;
}

// Opens all file streams in a paused state,
// then starts playing them at once.
// This allows play streams to queue their buffers.
bool Track::start() {
  playback.playSD(readFileName, true);
  // feedback.playSD(readFileName, true);
  recording.recordSD(writeFileName, true);
  return resume();
}

// Start playing from a stopped state
bool Track::startPlaying() {
  // punchOut();
  playback.playSD(readFileName, true);
  return playback.play();
  // return start();
}

// Start recording from a stopped state
bool Track::startRecording() {
  // punchIn();
  recording.recordSD(writeFileName, true);
  return recording.record();
  // return start();
}

// Pause all audio streams, then close them.
// This allows the record buffer to flush to
// its WAV file and update header information.
bool Track::stop() {
  // punchOut();
  recording.pause();
  playback.pause();
  // feedback.pause();
  recording.stop();
  playback.stop();
  // feedback.stop();
  // return recording.isStopped() && playback.isStopped() &&
  // feedback.isStopped();
  // return recording.isStopped();
  return true;
}

// Rotate read and write files pointers
// whenever a loop reaches its end
void Track::swapBuffers() {
  const char *temp = readFileName;
  readFileName = writeFileName;
  writeFileName = temp;
}
Track::Track(const char *f1, const char *f2, AudioInputI2S *s)
    : source(s), sourceToRecording(*source, 0, recording, 0), readFileName(f1),
      writeFileName(f2){};
