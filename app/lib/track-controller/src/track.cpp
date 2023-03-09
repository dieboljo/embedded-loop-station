#include <Arduino.h>
#include <SD.h>
#include <track.hpp>

// Audio buffer sizes
const size_t Track::playBufferSize = 65536;    // 64k
const size_t Track::recordBufferSize = 131072; // 128k

// Location of audio buffers
const AudioBuffer::bufType Track::bufferLocation = AudioBuffer::inExt;

// Input gains for recording (overdub or replace) vs playback
const Track::RecordGain Track::recordGain = {0.0, 0.5, 1.0};

// Create the read/write files, and configure the audio buffers
bool Track::begin() {
  bool initialized = initializeFiles();

  bool configured = configureBuffers();

  return initialized && configured;
}

// Check if the loop has ended, and restart if true
bool Track::checkLoopEnded(Status status) {
  switch (status) {
  case Status::Stop:
  case Status::Pause:
    return false;
  case Status::Play:
    if (playback.isPlaying()) {
      // Continue
      if (millis() % 1000 == 0) {
        Serial.println(">");
      }
      return false;
    } else if (!playback.isPlaying() && recording.positionMillis()) {
      Serial.println("Looping back from play");
      // End of loop, switch to recorded audio
      swapBuffers();
    } else if (!playbackFile) {
      if (!loopEstablished && recording.positionMillis()) {
        Serial.println("Setting the base loop");
        swapBuffers();
      }
      // Nothing recorded yet, wait for initial recording
      Serial.println("Nothing to play yet, record something already!");
    }
    return true;
  case Status::Record:
    if (millis() % 1000 == 0) {
      Serial.println("o");
    }
    if (!playbackFile) {
      // First recording, keep it moving
      return false;
    } else if (!playback.isPlaying()) {
      swapBuffers();
      return true;
    }
    return false;
  default:
    Serial.println("This shouldn't be reached");
    return false;
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
void Track::punchIn(Mode mode) {
  if (mode == Mode::Replace) {
    bus.gain(Channel::Source, recordGain.solo);
    bus.gain(Channel::Feedback, recordGain.mute);
  } else {
    bus.gain(Channel::Source, recordGain.mix);
    bus.gain(Channel::Feedback, recordGain.mix);
  }
}

// Disable recording immediately
void Track::punchOut() {
  bus.gain(Channel::Source, recordGain.mute);
  bus.gain(Channel::Feedback, recordGain.solo);
}

// Resume recording from a paused state
bool Track::record(Mode mode) {
  punchIn(mode);
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
bool Track::startRecording(Mode mode) {
  punchIn(mode);
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
void Track::swapBuffers() {
  if (!stop()) {
    Serial.println("Failed to stop audio streams");
  }
  AudioNoInterrupts();
  if (!loopEstablished) {
    loopEstablished = true;
  }
  const char *temp = readFileName;
  readFileName = writeFileName;
  writeFileName = temp;
  Serial.printf("Read file: %s, Write file %s\n", readFileName, writeFileName);
  AudioInterrupts();
  startPlaying();
}

Track::Track(const char *f1, const char *f2, AudioInputI2S *s)
    : loopEstablished(false), source(s),
      sourceToBus(*source, 0, bus, Channel::Source),
      feedbackToBus(feedback, 0, bus, Channel::Feedback),
      busToPeak(bus, 0, peak, 0), busToRecordingLeft(bus, 0, recording, 0),
      busToRecordingRight(bus, 0, recording, 1), readFileName(f1),
      writeFileName(f2){};
