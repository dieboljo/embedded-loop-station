#include <Arduino.h>
#include <SD.h>
#include <track.hpp>

bool Track::advance(Status status) {
  if (status == Status::Stop) {
    return true;
  }
  if (playback.lengthMillis() == playback.positionMillis()) {
    if (stop()) {
      swapBuffers();
      return startPlaying();
    }
    return false;
  }
  return true;
}

bool Track::begin() {
  bool initialized = initializeFiles();

  bool configured = configureBuffers();

  return initialized && configured;
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

bool Track::pause() {
  punchOut();
  return playback.pause() && feedback.pause() && recording.pause();
}

bool Track::play() {
  punchOut();
  return resume();
}

void Track::punchIn() {
  bus.gain(Channel::Source, gain.solo);
  bus.gain(Channel::Feedback, gain.mute);
}

void Track::punchOut() {
  bus.gain(Channel::Source, gain.mute);
  bus.gain(Channel::Feedback, gain.solo);
}

bool Track::record() {
  punchIn();
  return resume();
}

bool Track::resume() {
  return playback.play() && feedback.play() && recording.record();
}

bool Track::start() {
  playback.playSD(readFileName, true);
  feedback.playSD(readFileName, true);
  recording.recordSD(writeFileName, true);
  return resume();
}

bool Track::startPlaying() {
  punchOut();
  return start();
}

bool Track::startRecording() {
  punchIn();
  return start();
}

bool Track::stop() {
  punchOut();
  recording.pause();
  playback.pause();
  feedback.pause();
  recording.stop();
  playback.stop();
  feedback.stop();
  return recording.isStopped() && playback.isStopped() && feedback.isStopped();
}

void Track::swapBuffers() {
  const char *temp = readFileName;
  readFileName = writeFileName;
  writeFileName = temp;
}
