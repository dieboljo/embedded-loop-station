#include <Bounce.h>
#include <config.h>
#include <track.hpp>
#include <types.hpp>
#include <usb_audio.h>
#include <utils.hpp>
#include <display.hpp>
#include <library.hpp>

AudioControlSGTL5000 interface;

AudioInputI2S source;

#ifdef USE_USB_OUTPUT
/* USB output */
AudioOutputAnalog dac;
AudioOutputUSB sink;
#else
/* Audio shield output */
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
AudioConnection sourceToPeakRight(source, 0, sourcePeakRight, 0);
AudioConnection playbackToPeakLeft(track.playback, 0, sinkPeakLeft, 0);
AudioConnection playbackToPeakRight(track.playback, 1, sinkPeakRight, 0);

Display disp;
Library lib;

// define touchscreen
//#define T_CS    41
//#define T_IRQ   2

// touchscreen offset for four corners
//#define TS_MINX 400
//#define TS_MINY 400
//#define TS_MAXX 3879
//#define TS_MAXY 3843

#ifdef USE_USB_OUTPUT
AudioConnection sourceToDac(source, 0, dac, 0);
#endif

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

  // create lib and disp objects
  lib.array();
  // create fileArray containing names of SD card files
  String * fileArray = lib.returnValue();
  //copy fileArray to display array
  disp.setArray(fileArray);

  // boot display up
  disp.Setup();
  disp.Boot();
  delay(4000);
  disp.mainScreen();
  
  track.begin();
}

void loop() {
  // First, read the buttons
  buttons.record.update();
  buttons.stop.update();
  buttons.play.update();
  buttons.mode.update();

  adjustVolume(interface);

  disp.displayVol();
  disp.displayPan();
  disp.showLib();

  if(disp.getNameChange()){
    String name = disp.getFileName();
    Serial.println(name);
    track.setWriteFileName(name);
    disp.setNameChange(false);
  }
  
  track.reverse();

  adjustPan(&pan, track, mode);

  monitorAudioEngine();

  if(disp.getModeChange() == 1){
    if(disp.getMode() == 1){
      mode = Mode::Replace;
      disp.setModeChange(0);
      Serial.println("Mode: REPLACE");
    }
    else{
      mode = Mode::Overdub;
      disp.setModeChange(0);
      Serial.println("Mode: OVERDUB");
    }
  }

  if(disp.getRevBool()){
    //track.reverse();
    Serial.print(disp.getRevBool());
    disp.setRevBool(false);
    Serial.print(disp.getRevBool());
  }
  

  /*
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
  */

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
