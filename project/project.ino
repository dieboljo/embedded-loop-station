#include "play-sd-raw.h"
#include "track.h"
#include "tracks.h"
#include <Audio.h>
#include <Bounce.h>
#include <SD.h>
#include <SPI.h>
#include <SerialFlash.h>
#include <Wire.h>

#define SDCARD_MOSI_PIN 11
#define SDCARD_SCK_PIN 13
#define SDCARD_CS_PIN BUILTIN_SDCARD

enum class Status { Stop, Record, Play };

const int input = AUDIO_INPUT_MIC;

AudioControlSGTL5000 sgtl5000_1;
AudioInputI2S i2s2;
AudioOutputI2S i2s1;
AudioAnalyzePeak monitor;

/* Tracks tracks; */
Track track1 = Track("0A.WAV", "0B.WAV");

AudioConnection patchCord1(i2s2, 0, track1.record, 0);
AudioConnection patchCord2(i2s2, 0, monitor, 0);
AudioConnection patchCord3(track1.playback, 0, i2s1, 0);
AudioConnection patchCord4(track1.playback, 0, i2s1, 1);

// Remember which mode we're doing
Status mode = Status::Stop; // 0=stopped, 1=recording, 2=playing

// Bounce objects to easily and reliably read the buttons
Bounce buttonRecord = Bounce(0, 8);
Bounce buttonStop = Bounce(1, 8); // 8 = 8 ms debounce time
Bounce buttonPlay = Bounce(2, 8);

elapsedMillis msecs;

void setup() {
  // Configure the pushbutton pins
  pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);

  // Audio connections require memory, and the record queue
  // uses this memory to buffer incoming audio.
  AudioMemory(60);

  // Enable the audio shield, select input, and enable output
  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(input);
  sgtl5000_1.micGain(30);
  sgtl5000_1.volume(0.5);

  // Initialize the SD card
  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    // stop here if no SD card, but print a message
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }
}

void loop() {
  // First, read the buttons
  buttonRecord.update();
  buttonStop.update();
  buttonPlay.update();

  // read the knob position (analog input A1)
  int knob = analogRead(A1);
  float vol = (float)knob / 1280.0;
  sgtl5000_1.volume(vol);

  if (msecs > 1000) {
    Serial.print("volume = ");
    Serial.println(vol);
    if (monitor.available()) {
      msecs = 0;
      float leftNumber = monitor.read();
      Serial.print(leftNumber);
      Serial.println();
    }
  }

  // Respond to button presses
  if (buttonRecord.fallingEdge()) {
    Serial.println("Record Button Press");
    if (mode == Status::Play)
      stopPlaying();
    if (mode == Status::Stop)
      startRecording();
  }
  if (buttonStop.fallingEdge()) {
    Serial.println("Stop Button Press");
    if (mode == Status::Record)
      stopRecording();
    if (mode == Status::Play)
      stopPlaying();
  }
  if (buttonPlay.fallingEdge()) {
    Serial.println("Play Button Press");
    if (mode == Status::Record)
      stopRecording();
    if (mode == Status::Stop)
      startPlaying();
  }

  // If we're playing or recording, carry on...
  if (mode == Status::Record) {
    continueRecording();
  }
  if (mode == Status::Play) {
    continuePlaying();
  }

  // when using a microphone, continuously adjust gain
  if (input == AUDIO_INPUT_MIC)
    adjustMicLevel();
}

void startRecording() {
  Serial.println("startRecording");
  const boolean recording = track1.startRecording();
  if (recording) {
    mode = Status::Record;
  } else {
    Serial.println("Failed to start recording");
  }
}

void continueRecording() { track1.continueRecording(); }

void stopRecording() {
  Serial.println("stopRecording");
  track1.stopRecording();
  mode = Status::Stop;
}

void startPlaying() {
  Serial.println("startPlaying");
  track1.startPlaying();
  mode = Status::Play;
}

void continuePlaying() { track1.continuePlaying(); }

void pausePlaying() {
  Serial.println("pausePlaying");
  track1.pausePlaying();
  mode = Status::Stop;
}

void stopPlaying() {
  Serial.println("stopPlaying");
  track1.stopPlaying();
  mode = Status::Stop;
}

void adjustMicLevel() {
  // TODO: read the peak1 object and adjust sgtl5000_1.micGain()
  // if anyone gets this working, please submit a github pull request :-)
}
