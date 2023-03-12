#include <config.h>
#include <track.hpp>
#include <types.hpp>
#include <utils.hpp>

AudioControlSGTL5000 interface;

#ifdef USE_USB_INPUT
AudioInputUSB source;
// Audio library needs at least one non-USB input to update properly
AudioInputI2S dummy;
#else
AudioInputI2S source;
#endif

#ifdef USE_USB_OUTPUT
AudioOutputUSB sink;
#else
AudioOutputI2S sink;
#endif

AudioAnalyzePeak sourcePeakLeft;
AudioAnalyzePeak sourcePeakRight;
AudioAnalyzePeak sinkPeakLeft;
AudioAnalyzePeak sinkPeakRight;

// The track where data is recorded
Track track("file1.wav", "file2.wav", &source);

AudioConnection playbackToSinkLeft(track.playback, 0, sink, 0);
AudioConnection playbackToSinkRight(track.playback, 1, sink, 1);
AudioConnection sourceToPeakLeft(source, 0, sourcePeakLeft, 0);
AudioConnection sourceToPeakRight(source, 1, sourcePeakRight, 0);
AudioConnection playbackToPeakLeft(track.playback, 0, sinkPeakLeft, 0);
AudioConnection playbackToPeakRight(track.playback, 1, sinkPeakRight, 0);

Status status = Status::Stop;

Mode mode = Mode::Overdub;

float pan = 0.0;

Buttons buttons = {
    Bounce(buttonStopPin, 8),
    Bounce(buttonRecordPin, 8),
    Bounce(buttonPlayPin, 8),
    Bounce(buttonModePin, 8),
};

void setup() {
  initializeSerialCommunication();

  // Configure the pushbutton pins
  configureButtons();

  // Audio connections require memory
  AudioMemory(20);

  // Initialize processor and memory measurements
  AudioProcessorUsageMaxReset();
  AudioMemoryUsageMaxReset();

  // Enable the audio shield, select input, and enable output
  initializeInterface(interface);

  initializeSdCard();

  track.begin();
}

void loop() {
  // First, read the buttons
  buttons.record.update();
  buttons.stop.update();
  buttons.play.update();
  buttons.mode.update();

#ifndef USE_USB_OUTPUT
  adjustVolume(interface);
#endif

  adjustPan(&pan, track, mode);

  monitorAudioEngine();

  // Respond to button presses
  if (buttons.mode.fallingEdge()) {
    if (mode == Mode::Overdub) {
      mode = Mode::Replace;
      Serial.println("Mode: REPLACE");
    } else {
      mode = Mode::Overdub;
      Serial.println("Mode: OVERDUB");
    }
  }

  if (buttons.record.fallingEdge()) {
    Serial.println("Record Button Pressed");
    switch (status) {
    case Status::Record:
      track.punchOut();
      status = Status::Play;
      break;
    case Status::Play:
      track.punchIn(mode, pan);
      status = Status::Record;
      break;
    case Status::Pause:
      if (track.record(mode, pan)) {
        Serial.println("Resumed recording");
      }
      status = Status::Record;
      break;
    case Status::Stop:
      if (track.startRecording(mode, pan)) {
        Serial.println("Recording started");
      }
      status = Status::Record;
      break;
    default:
      break;
    }
  }

  if (buttons.stop.fallingEdge()) {
    Serial.println("Stop Button Pressed");
    if (track.stop(true)) {
      Serial.println("Loop stopped");
    }
    status = Status::Stop;
  }

  if (buttons.play.fallingEdge()) {
    Serial.println("Play Button Pressed");
    switch (status) {
    case Status::Play:
    case Status::Record:
      if (track.pause()) {
        Serial.println("Paused");
      }
      status = Status::Pause;
      break;
    case Status::Pause:
      if (track.play()) {
        Serial.println("Resumed playback");
      }
      status = Status::Play;
      break;
    case Status::Stop:
      if (track.startPlaying()) {
        Serial.println("Playback started");
      }
      status = Status::Play;
      break;
    default:
      break;
    }
  }

  status = track.checkLoopEnded(status);

  // Print input or output levels to the serial monitor.
  if (monitorInput) {
    showInputLevels(&sourcePeakLeft, &sourcePeakRight);
  }
  if (monitorOutput) {
    showOutputLevels(&sinkPeakLeft, &sinkPeakRight);
  }

  // when using a microphone, continuously adjust gain
  if (input == AUDIO_INPUT_MIC) {
    adjustMicLevel();
  }
}
