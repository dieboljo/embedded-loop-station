#include <Arduino.h>
#include <SD.h>
#include <track.hpp>

bool Track::advance(Status status) {
  if (status == Status::Play) {
    bool isPlaying = true;
    AudioNoInterrupts();
    if (playback.lengthMillis() == playback.positionMillis()) {
      // Playback reached end, restart from beginning
      isPlaying = playback.play(readFileName);
    } else if (!audio.isPlaying()) {
      // Restart playing from current position
      isPlaying = audio.play(readFileName, position);
    }
    position = audio.getOffset();
    AudioInterrupts();
    return isPlaying;
  } else if (status == Status::Record) {
    AudioNoInterrupts();
    bool isRecording = writeToBuffer();
    position = (uint32_t)writeFileBuffer.position();
    AudioInterrupts();
    return isRecording;
  }
  return true;
}

void Track::begin() {
  initializeFiles();

  configureBuffers();

  // TODO: Move this into a mode toggle handler
  /* bus.gain(Channel::Source, gain.mix);
  bus.gain(Channel::Copy, gain.mix); */
  bus.gain(Channel::Source, gain.solo);
  bus.gain(Channel::Feedback, gain.mute);
}

uint32_t Track::closeWriteBuffer(void) {
  recordQueue.end();
  // copy.stop();
  while (recordQueue.available() > 0) {
    writeFileBuffer.write((byte *)recordQueue.readBuffer(), 256);
    recordQueue.freeBuffer();
  }
  uint32_t filePosition = (uint32_t)writeFileBuffer.position();
  writeFileBuffer.close();
  swapBuffers();
  return filePosition;
}

// SD audio objects need buffers configuring
bool Track::configureBuffers() {
  AudioBuffer::result ok = AudioBuffer::ok;
  return (playback.createBuffer(playBufferSize, bufferLocation) == ok &&
          feedback.createBuffer(playBufferSize, bufferLocation) == ok &&
          recording.createBuffer(recordBufferSize, bufferLocation) == ok);
}

// Delete and recreate the read and write files
void Track::initializeFiles() {
  if (SD.exists(writeFileName)) {
    SD.remove(writeFileName);
  }
  if (SD.exists(readFileName)) {
    SD.remove(readFileName);
  }
  // Create the read file buffers
  File temp = SD.open(writeFileName, FILE_WRITE);
  temp.close();
  temp = SD.open(readFileName, FILE_WRITE);
  temp.close();
}

bool Track::openWriteBuffer() {
  // TODO: Does having this here defeat the purpose?
  swapBuffers();
  writeFileBuffer = SD.open(writeFileName, FILE_WRITE);
  if (writeFileBuffer) {
    writeFileBuffer.seek(position);
    recordQueue.begin();
    return true;
  } else {
    Serial.println("Failed to open buffer for writing!");
    return false;
  }
}

void Track::pausePlayback() {
  AudioNoInterrupts();
  position = audio.getOffset();
  audio.stop();
  AudioInterrupts();
}

void Track::pauseRecording() {
  AudioNoInterrupts();
  position = closeWriteBuffer();
  AudioInterrupts();
}

void Track::startPlayback() {
  if (writeFileBuffer) {
    AudioNoInterrupts();
    closeWriteBuffer();
    AudioInterrupts();
  }
}

void Track::startRecording() {
  AudioNoInterrupts();
  audio.stop();
  openWriteBuffer();
  // copy.play(readFileName, position);
  AudioInterrupts();
}

void Track::stopPlayback() {
  AudioNoInterrupts();
  resetPosition();
  audio.stop();
  AudioInterrupts();
}

void Track::stopRecording() {
  AudioNoInterrupts();
  resetPosition();
  closeWriteBuffer();
  AudioInterrupts();
}

void Track::swapBuffers() {
  const char *temp = readFileName;
  readFileName = writeFileName;
  writeFileName = temp;
}

bool Track::writeToBuffer() {
  if (!writeFileBuffer)
    return false;
  // copy.play(readFileName, position);
  if (recordQueue.available() >= 2) {
    byte buffer[512];
    memcpy(buffer, recordQueue.readBuffer(), 256);
    recordQueue.freeBuffer();
    memcpy(buffer + 256, recordQueue.readBuffer(), 256);
    recordQueue.freeBuffer();
    writeFileBuffer.write(buffer, 512);
  }
  return true;
}
