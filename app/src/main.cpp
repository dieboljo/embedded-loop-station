#include <Bounce.h>
#include <config.h>
#include <track.hpp>
#include <types.hpp>
#include <usb_audio.h>

void adjustMicLevel();

AudioControlSGTL5000 interface;

AudioInputI2S source;

// The track where data is recorded
Track track("FILE1.RAW", "FILE2.RAW", &source);

/* USB output */
// change this to AudioOutputI2S for boards without DAC output
// AudioOutputAnalog dac;
// AudioOutputI2S dac;
// AudioOutputUSB sink;

/* Audio shield output */
AudioOutputI2S sink;

AudioConnection playbackToSinkLeft(track.playback, 0, sink, 0);
AudioConnection playbackToSinkRight(track.playback, 0, sink, 1);

/* USB output */
// AudioConnection patchCord3(source, 0, dac, 0);

// which input on the audio shield will be used?
const int input = audioInput;

// Remember which mode we're doing
Status status = Status::Stop; // 0=stopped, 1=recording, 2=playing

// Bounce objects to easily and reliably read the buttons
// 8 = 8 ms debounce time
Buttons buttons = {
    Bounce(buttonStopPin, 8),
    Bounce(buttonRecordPin, 8),
    Bounce(buttonPlayPin, 8),
};

elapsedMillis msecs;

void setup() {
  Serial.begin(9600);

  // Configure the pushbutton pins
  pinMode(buttonRecordPin, INPUT_PULLUP);
  pinMode(buttonStopPin, INPUT_PULLUP);
  pinMode(buttonPlayPin, INPUT_PULLUP);

  // Audio connections require memory, and the record queue
  // uses this memory to buffer incoming audio.
  AudioMemory(60);

  // Enable the audio shield, select input, and enable output
  interface.enable();
  interface.inputSelect(input);
  interface.micGain(25);
  interface.volume(0.5);

  // Initialize the SD card
  SPI.setMOSI(sdCardMosiPin);
  SPI.setSCK(sdCardSckPin);
  if (!(SD.begin(sdCardCsPin))) {
    // stop here if no SD card, but print a message
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }
  track.begin();
}

void loop() {
  // First, read the buttons
  buttons.record.update();
  buttons.stop.update();
  buttons.play.update();

  // Read the knob position (analog input A1)
  int knob = analogRead(A1);
  float vol = (float)knob / 1280.0;
  interface.volume(vol);
  if (msecs > 1000) {
    Serial.print("volume = ");
    Serial.println(vol);
    msecs = 0;
    Serial.print(track.readPeak());
    Serial.println();
  }

  // Respond to button presses

  if (buttons.record.fallingEdge()) {
    Serial.println("Record Button Press");
    if (status == Status::Record) {
      status = Status::Play;
    } else {
      status = Status::Record;
    }
  }

  if (buttons.stop.fallingEdge()) {
    Serial.println("Stop Button Press");
    status = Status::Stop;
    track.stop();
  }

  if (buttons.play.fallingEdge()) {
    Serial.println("Play Button Press");
    if (status == Status::Play) {
      status = Status::Pause;
      track.pause();
    } else {
      status = Status::Play;
    }
  }

  track.advance(status);

  // when using a microphone, continuously adjust gain
  if (input == AUDIO_INPUT_MIC)
    adjustMicLevel();
}

void adjustMicLevel() {
  // TODO: read the peak1 object and adjust sgtl5000_1.micGain()
  // if anyone gets this working, please submit a github pull request :-)
}
