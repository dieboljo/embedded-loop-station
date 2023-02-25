#include "Audio.h"
#include "FS.h"
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
const RecordGain Track::recordGain = {0.0, 0.5, 1.0};

// Create the read/write files, and configure the audio buffers
bool Track::begin() {
  bool initialized = initializeFiles();

  bool configured = configureBuffers();

  // return initialized && configured;
  return initialized && configured;
}

// Check if the loop has ended, and restart if true
Status Track::checkLoopEnded(Status status) {
  switch (status) {
  case Status::Stop:
  case Status::Pause:
    return status;
  case Status::Play:
    if (!readFile) {
      if (!loopEstablished && recording.positionMillis()) {
        Serial.println("Setting the base loop");
        return swapBuffers();
      }
      // Nothing recorded yet, wait for initial recording
      Serial.println("Nothing to play yet, record something already!");
      return Status::Stop;
    } else if (playback.isPlaying()) {
      // Continue
      if (millis() % 1000 == 0) {
        Serial.print(">");
      }
      return status;
    } else if (!playback.isPlaying() && recording.positionMillis()) {
      // End of loop, switch to recorded audio
      return swapBuffers();
    }
  case Status::Record:
    if (!readFile) {
      // First recording, keep it moving
      if (millis() % 1000 == 0) {
        Serial.print("o");
      }
      return status;
    } else if (!playback.isPlaying()) {
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
      // feedback.createBuffer(playBufferSize, bufferLocation) == ok &&
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
  writeFile = SD.open(writeFileName, FILE_WRITE);
  if (!writeFile) {
    Serial.println("Failed to create write file");
    success = false;
  }
  writeFile.close();
  readFile = SD.open(readFileName, FILE_WRITE);
  if (!readFile) {
    Serial.println("Failed to create read file");
    success = false;
  }
  readFile.close();
  return success;
}

// Pause recording and playback, and disable recording
bool Track::pause() {
  // return playback.pause() && feedback.pause() && recording.pause();
  return recording.pause() && playback.pause();
}

// Resume playing from a paused state
bool Track::play() {
  // punchOut();
  return resume();
}

// Enable recording at the current track position,
// in either replace or overdub mode
// TODO: Implement overdub mode
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
  if (readFile) {
    return recording.record() && playback.play();
  } else {
    return recording.record();
  }
}

// Opens all file streams in a paused state,
// then starts playing them at once.
// This allows play streams to queue their buffers.
bool Track::start() {
  readFile = SD.open(readFileName);
  Serial.printf("File size: %d", readFile.size());
  if (readFile.size()) {
    playback.play(readFile, true);
  } else {
    readFile.close();
  }
  // feedback.playSD(readFileName, true);
  writeFile = SD.open(writeFileName, FILE_WRITE_BEGIN);
  recording.record(writeFile, true);
  return resume();
}

// Start playing from a stopped state
bool Track::startPlaying() {
  // punchOut();
  /* playback.playSD(readFileName, true);
  return playback.play(); */
  return start();
}

// Start recording from a stopped state
bool Track::startRecording() {
  // punchIn();
  /* recording.recordSD(writeFileName, true);
  return recording.record(); */
  return start();
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
  return recording.isStopped() && playback.isStopped();
}

// Rotate read and write files pointers
// whenever a loop reaches its end
Status Track::swapBuffers() {
  if (!stop()) {
    Serial.println("Failed to stop audio streams");
    return Status::Stop;
  }
  AudioNoInterrupts();
  if (loopEstablished) {
    writeFile = SD.open(writeFileName);
    readFile = SD.open(readFileName);
    writeFile.truncate(readFile.size());
    writeFile.close();
    readFile.close();
  } else {
    loopEstablished = true;
  }
  const char *temp = readFileName;
  readFileName = writeFileName;
  writeFileName = temp;
  Serial.printf("Read file: %s, Write file %s\n", readFileName, writeFileName);
  AudioInterrupts();
  Serial.println("Loop ended, restarted from beginning");
  startPlaying();
  return Status::Play;
}

Track::Track(const char *f1, const char *f2, AudioInputI2S *s)
    : loopEstablished(false), source(s),
      sourceToRecording(*source, 0, recording, 0), readFileName(f1),
      writeFileName(f2){};
