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
// Library instance
Library lib;
// Display instance
Display disp(lib);

AudioConnection playbackToSinkLeft(controller.outMixLeft, 0, sink, 0);
AudioConnection playbackToSinkRight(controller.outMixRight, 0, sink, 1);
AudioConnection sourceToPeakLeft(source, 0, sourcePeakLeft, 0);
AudioConnection sourceToPeakRight(source, 1, sourcePeakRight, 0);
AudioConnection playbackToPeakLeft(controller.outMixLeft, 0, sinkPeakLeft, 0);
AudioConnection
    playbackToPeakRight(controller.outMixRight, 0, sinkPeakRight, 0);

// Global state
AppState state = {
    0., 0, false, Mode::Overdub, 0., 0, false, Status::Stop, 0, 0.,
};

Buttons buttons = {
    Bounce(buttonStopPin, 8),
    Bounce(buttonRecordPin, 8),
    Bounce(buttonPlayPin, 8),
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
  readPan(state);
  readFade(state);
#ifndef USE_USB_OUTPUT
  readVolume(state);
#endif

  /*
  ## 2. Respond
  */

  // Next track button clicked
  if (disp.clickedNextTrack()) {
    if (state.status == Status::Record)
      state.status = Status::Play;
    state.track = controller.nextTrack();
    Serial.print("Current track: ");
    Serial.println(state.track + 1);
  }

  // Clear track button clicked
  /* if (buttons.clearTrack.fallingEdge()) {
    state.status = Status::Stop;
    controller.clearTrack();
  } */

  // Record button clicked
  if (buttons.record.fallingEdge()) {
    Serial.println("Record Button Pressed");
    switch (state.status) {
    case Status::Record:
      controller.punchOut();
      state.status = Status::Play;
      break;
    case Status::Play:
      controller.punchIn(state.mode);
      state.status = Status::Record;
      break;
    case Status::Pause:
      if (controller.record(state.mode))
        Serial.println("Resumed recording");
      state.status = Status::Record;
      break;
    case Status::Stop:
      if (controller.startRecording(state.mode))
        Serial.println("Recording started");
      state.status = Status::Record;
      break;
    default:
      break;
    }
  }

  // Stop button clicked
  if (buttons.stop.fallingEdge()) {
    Serial.println("Stop Button Pressed");
    if (controller.stop(true))
      Serial.println("Loop stopped");
    state.status = Status::Stop;
  }

  // Play button clicked
  if (buttons.play.fallingEdge()) {
    Serial.println("Play Button Pressed");
    switch (state.status) {
    case Status::Play:
    case Status::Record:
      if (controller.pause())
        Serial.println("Paused");
      state.status = Status::Pause;
      break;
    case Status::Pause:
      if (controller.play())
        Serial.println("Resumed playback");
      state.status = Status::Play;
      break;
    case Status::Stop:
      if (controller.startPlaying())
        Serial.println("Playback started");
      state.status = Status::Play;
      break;
    default:
      break;
    }
  }

  // Mode button clicked
  if (disp.clickedMode()) {
    if (state.mode == Mode::Overdub) {
      Serial.println("Mode: REPLACE");
      state.mode = Mode::Replace;
    } else {
      Serial.println("Mode: OVERDUB");
      state.mode = Mode::Overdub;
    }
  }

  // Save button clicked
  if (disp.clickedSave()) {
    controller.stop(true);
    state.status = Status::Stop;
    state.saving = true;
  }

  if (disp.clickedLibraryEntry()) {
    controller.stop(true);
    state.status = Status::Stop;
    state.loading = true;
  }

  /*
  ## 3. Update audio
  */

  interface.volume(state.volume);
  if (input == AUDIO_INPUT_MIC)
    adjustMicLevel();

  controller.pan(state.pan, state.mode);
  controller.fade(state.fade, state.mode);
  state.status = controller.checkTracks(state.status);
  state.position = controller.getPosition();
  state.length = controller.getLength();

  /*
  ## 4. Update display
  */

  disp.update(state);

  // Let display draw save status first, since saving will block
  if (state.saving) {
    controller.saveLoop();
    state.saving = false;
  }

  if (state.loading) {
    const char *fileName = disp.getSelectedEntry();
    if (strcmp(fileName, "") != 0)
      controller.loadLoop(fileName);
    state.loading = false;
    disp.showMainScreen();
  }

  /*
  ## 5. Log
  */

  // Print audio engine usage
  monitorAudioEngine();

  // Print input or output levels to the serial monitor.
  if (monitorInput)
    showInputLevels(&sourcePeakLeft, &sourcePeakRight);
  if (monitorOutput)
    showOutputLevels(&sinkPeakLeft, &sinkPeakRight);
}
