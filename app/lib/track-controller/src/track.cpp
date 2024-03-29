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
bool Track::checkEnded(uint32_t loopLength) {
  // Track reached its end
  if (loopEstablished && recording.positionMillis() >= loopLength) {
    return true;
  }
  return false;
}

void Track::clear() {
  SD.remove(readFileName);
  SD.remove(writeFileName);
}

// SD audio objects need buffer configuration
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

void Track::load(const char *fileName) {
  AudioNoInterrupts();
  if (!SD.exists(fileName)) {
    Serial.printf("File %s does not exist\n", fileName);
    return;
  }
  File writeFile = SD.open(readFileName, FILE_WRITE_BEGIN);
  File readFile = SD.open(fileName);
  if (!writeFile || !readFile) {
    Serial.println("Failed to load into track");
    return;
  }
  Serial.print("Loading track");
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
  AudioInterrupts();
}

// Pause recording and playback, and disable recording
bool Track::pause() {
  return playback.pause() && feedback.pause() && recording.pause();
}

// Enable recording at the current track position,
// in either replace or overdub mode
void Track::punchIn(Mode mode) {
  if (mode == Mode::Replace) {
    busLeft.gain(Channel::Source, gain.solo);
    busLeft.gain(Channel::Feedback, gain.mute);
    busRight.gain(Channel::Source, gain.solo);
    busRight.gain(Channel::Feedback, gain.mute);
  } else {
    busLeft.gain(Channel::Source, gain.mix);
    busLeft.gain(Channel::Feedback, gain.mix);
    busRight.gain(Channel::Source, gain.mix);
    busRight.gain(Channel::Feedback, gain.mix);
  }
}

// Disable recording immediately
void Track::punchOut() {
  busLeft.gain(Channel::Source, gain.mute);
  busLeft.gain(Channel::Feedback, gain.solo);
  busRight.gain(Channel::Source, gain.mute);
  busRight.gain(Channel::Feedback, gain.solo);
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

// Pause all audio streams, then close them.
// This allows the record buffer to flush to
// its WAV file and update header information.
bool Track::stop(bool cancel) {
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
bool Track::swapBuffers() {
  const char *temp = readFileName;
  readFileName = writeFileName;
  writeFileName = temp;
  return true;
}

#ifdef USE_USB_INPUT
Track::Track(const char *f1, const char *f2, AudioInputUSB &s)
    : sourceToBusLeft(s, 0, busLeft, Channel::Source),
      sourceToBusRight(s, 1, busRight, Channel::Source),
      feedbackToBusLeft(feedback, 0, busLeft, Channel::Feedback),
      feedbackToBusRight(feedback, 1, busRight, Channel::Feedback),
      busLeftToRecording(busLeft, 0, recording, 0),
      busRightToRecording(busRight, 0, recording, 1),
      readFileName(f1),
      writeFileName(f2){};
#else
Track::Track(const char *f1, const char *f2, AudioInputI2S &s)
    : sourceToBusLeft(s, 0, busLeft, Channel::Source),
      sourceToBusRight(s, 1, busRight, Channel::Source),
      feedbackToBusLeft(feedback, 0, busLeft, Channel::Feedback),
      feedbackToBusRight(feedback, 1, busRight, Channel::Feedback),
      busLeftToRecording(busLeft, 0, recording, 0),
      busRightToRecording(busRight, 0, recording, 1),
      readFileName(f1),
      writeFileName(f2){};
#endif
