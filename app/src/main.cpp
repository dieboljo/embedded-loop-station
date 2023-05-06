#include <config.h>
#include <track-controller.hpp>
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
TrackController controller(source);

AudioConnection playbackToSinkLeft(controller.outMixLeft, 0, sink, 0);
AudioConnection playbackToSinkRight(controller.outMixRight, 0, sink, 1);
AudioConnection sourceToPeakLeft(source, 0, sourcePeakLeft, 0);
AudioConnection sourceToPeakRight(source, 1, sourcePeakRight, 0);
AudioConnection playbackToPeakLeft(controller.outMixLeft, 0, sinkPeakLeft, 0);
AudioConnection playbackToPeakRight(controller.outMixRight, 0, sinkPeakRight,
                                    0);

Status status = Status::Stop;

Mode mode = Mode::Overdub;

float pan = 0.0;

Buttons buttons = {
    Bounce(buttonStopPin, 8), Bounce(buttonRecordPin, 8),
    Bounce(buttonPlayPin, 8), Bounce(buttonModePin, 8),
    Bounce(buttonSavePin, 8), Bounce(buttonNextTrackPin, 8),
};

void setup() {
  initializeSerialCommunication();

  // Configure the pushbutton pins
  configureButtons();

  // Audio connections require memory
  AudioMemory(30);

  // Initialize processor and memory measurements
  AudioProcessorUsageMaxReset();
  AudioMemoryUsageMaxReset();

  // Enable the audio shield, select input, and enable output
  initializeInterface(interface);

  initializeSdCard();

  controller.begin();
}

void loop() {
  // First, read the buttons
  readButtons(buttons);

#ifndef USE_USB_OUTPUT
  adjustVolume(interface);
#endif

  adjustPan(&pan, controller, mode);

  monitorAudioEngine();

  // Respond to button presses
  if (buttons.save.fallingEdge()) {
    // TODO: implement save() in TrackController
    // status = Status::Stop;
    // track.save();
  }

  if (buttons.mode.fallingEdge()) {
    if (mode == Mode::Overdub) {
      mode = Mode::Replace;
      Serial.println("Mode: REPLACE");
    } else {
      mode = Mode::Overdub;
      Serial.println("Mode: OVERDUB");
    }
  }

  if (buttons.nextTrack.fallingEdge()) {
    if (status == Status::Record) {
      status = Status::Play;
    }
    int selectedTrack = controller.nextTrack();
    Serial.print("Current track: ");
    Serial.println(selectedTrack + 1);
  }

  if (buttons.record.fallingEdge()) {
    Serial.println("Record Button Pressed");
    switch (status) {
    case Status::Record:
      controller.punchOut();
      status = Status::Play;
      break;
    case Status::Play:
      controller.punchIn(mode);
      status = Status::Record;
      break;
    case Status::Pause:
      if (controller.record(mode)) {
        Serial.println("Resumed recording");
      }
      status = Status::Record;
      break;
    case Status::Stop:
      if (controller.startRecording(mode)) {
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
    if (controller.stop(true)) {
      Serial.println("Loop stopped");
    }
    status = Status::Stop;
  }

  if (buttons.play.fallingEdge()) {
    Serial.println("Play Button Pressed");
    switch (status) {
    case Status::Play:
    case Status::Record:
      if (controller.pause()) {
        Serial.println("Paused");
      }
      status = Status::Pause;
      break;
    case Status::Pause:
      if (controller.play()) {
        Serial.println("Resumed playback");
      }
      status = Status::Play;
      break;
    case Status::Stop:
      if (controller.startPlaying()) {
        Serial.println("Playback started");
      }
      status = Status::Play;
      break;
    default:
      break;
    }
  }

  status = controller.checkTracks(status);

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
