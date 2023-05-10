#include <config.h>
#include <display.hpp>
#include <library.hpp>
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

// Controls the tracks where data is recorded
TrackController controller(source);
// Display instance
Display disp;
// Library instance
Library lib;
// String myString;

AudioConnection playbackToSinkLeft(controller.outMixLeft, 0, sink, 0);
AudioConnection playbackToSinkRight(controller.outMixRight, 0, sink, 1);
AudioConnection sourceToPeakLeft(source, 0, sourcePeakLeft, 0);
AudioConnection sourceToPeakRight(source, 1, sourcePeakRight, 0);
AudioConnection playbackToPeakLeft(controller.outMixLeft, 0, sinkPeakLeft, 0);
AudioConnection playbackToPeakRight(controller.outMixRight, 0, sinkPeakRight,
                                    0);

// Global variables
Status status = Status::Stop;
Mode mode = Mode::Overdub;
float fade = 0.0;
float pan = 0.0;
float volume = 0.0;
uint32_t position;
uint32_t length;

Buttons buttons = {
    Bounce(buttonStopPin, 8),       Bounce(buttonRecordPin, 8),
    Bounce(buttonPlayPin, 8),       Bounce(buttonModePin, 8),
    Bounce(buttonSavePin, 8),       Bounce(buttonNextTrackPin, 8),
    Bounce(buttonClearTrackPin, 8),
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

  // Create file array for track selections
  lib.array();

  // boot display up
  disp.setup();

  // myString = String(track.getTrackName());
}

void loop() {

  /*
  ## 1. Read
  */

  readButtons(buttons);
  disp.readTouch();
  pan = readPan(pan);
  fade = readFade(fade);
#ifndef USE_USB_OUTPUT
  volume = readVolume(volume);
#endif

  /*
  ## 2. Respond
  */

  // Next track button clicked
  if (buttons.nextTrack.fallingEdge()) {
    if (status == Status::Record) {
      status = Status::Play;
    }
    int selectedTrack = controller.nextTrack();
    Serial.print("Current track: ");
    Serial.println(selectedTrack + 1);
  }

  // Clear track button clicked
  if (buttons.clearTrack.fallingEdge()) {
    status = Status::Stop;
    controller.clearTrack();
  }

  // Record button clicked
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

  // Stop button clicked
  if (buttons.stop.fallingEdge()) {
    Serial.println("Stop Button Pressed");
    if (controller.stop(true)) {
      Serial.println("Loop stopped");
    }
    status = Status::Stop;
  }

  // Play button clicked
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

  // Mode button clicked
  if (disp.clickedMode()) {
    if (mode == Mode::Overdub) {
      Serial.println("Mode: REPLACE");
      mode = Mode::Replace;
    } else {
      Serial.println("Mode: OVERDUB");
      mode = Mode::Overdub;
    }
  }

  // Save button clicked
  if (disp.clickedSave()) {
    controller.stop(true);
    status = Status::Stop;
    disp.drawSaveButton(true);
    controller.save();
    disp.drawSaveButton(false);
  }

  if (disp.clickedMain()) {
    disp.showMainScreen();
  }

  if (disp.clickedLibrary()) {
    disp.showLibraryScreen();
  }

  /*
  ## 3. Update audio
  */

  interface.volume(volume);
  if (input == AUDIO_INPUT_MIC) {
    adjustMicLevel();
  }

  controller.pan(pan, mode);
  controller.fade(fade, mode);
  status = controller.checkTracks(status);

  /*
  ## 4. Update display
  */

  disp.drawPosition(controller.getPosition(), controller.getLength());
  disp.drawModeButton(mode);
  disp.drawPan(pan);
  disp.drawStatus(status);
  disp.drawSaveButton(false);
  disp.drawVolume(volume);
  disp.drawTrackName(controller.getSelectedTrack());

  // Get name change from library selection
  /* if (disp.getNameChange()) {
    myString = disp.getFileName();
    disp.setNameChange(false);
  } */

  /*
  ## 5. Log
  */

  // Print audio engine usage
  monitorAudioEngine();

  // Print input or output levels to the serial monitor.
  if (monitorInput) {
    showInputLevels(&sourcePeakLeft, &sourcePeakRight);
  }
  if (monitorOutput) {
    showOutputLevels(&sinkPeakLeft, &sinkPeakRight);
  }
}
