#include "interface.h"

const int INPUT_DEVICE = AUDIO_INPUT_MIC;

bool Interface::begin() {
  // Configure the pushbutton pins
  /* pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP); */

  // Audio connections require memory, and the record queue
  // uses this memory to buffer incoming audio.
  AudioMemory(60);

  // Enable the audio shield, select input, and enable output
  controller.enable();
  controller.inputSelect(INPUT_DEVICE);
  controller.micGain(30);
  controller.volume(0.5);

  return recorder.begin();
}

void Interface::advance() {
  adjustMicLevel();
  recorder.advance();
}

float Interface::getPeak() {
  if (monitor.available()) {
    return monitor.read();
  } else {
    return -1.0;
  }
}

void Interface::handleButtonPress(Button button) {
  switch (button) {
  case Button::Play:
    recorder.handlePlayPress();
    break;
  case Button::Record:
    recorder.handleRecordPress();
    break;
  case Button::Stop:
    recorder.handleStopPress();
    break;
  default:
    break;
  }
}
