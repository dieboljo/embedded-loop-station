#include <Bounce.h>
#include <config.h>
#include <track.hpp>
#include <types.hpp>
#include <usb_audio.h>
#include <utils.hpp>

AudioControlSGTL5000 interface;

AudioInputI2S source;

#ifdef USB_MIDI_AUDIO_SERIAL
/* USB output */
AudioOutputAnalog dac;
AudioOutputUSB sink;
#else
/* Audio shield output */
AudioOutputI2S sink;
#endif

AudioAnalyzePeak peakLeft;
AudioAnalyzePeak peakRight;

// The track where data is recorded
Track track("FILE1.WAV", "FILE2.WAV", &source);

AudioConnection playbackToSinkLeft(track.playback, 0, sink, 0);
AudioConnection playbackToSinkRight(track.playback, 1, sink, 1);
AudioConnection playbackToPeakLeft(track.playback, 0, peakLeft, 0);
AudioConnection playbackToPeakRight(track.playback, 1, peakRight, 0);
#ifdef USB_MIDI_AUDIO_SERIAL
AudioConnection sourceToDac(source, 0, dac, 0);
#endif

Status status = Status::Stop;

Buttons buttons = {
    Bounce(buttonStopPin, 8),
    Bounce(buttonRecordPin, 8),
    Bounce(buttonPlayPin, 8),
};

elapsedMillis msecs;

void setup() {
  initializeSerialCommunication();

  // Configure the pushbutton pins
  configureButtons();

  // Audio connections require memory
  AudioMemory(10);

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

  adjustVolume(interface);

  // Respond to button presses

  if (buttons.record.fallingEdge()) {
    Serial.println("Record Button Press");
    switch (status) {
    case Status::Record:
      track.punchOut();
      status = Status::Play;
      break;
    case Status::Play:
      track.punchIn();
      status = Status::Record;
      break;
    case Status::Pause:
      track.record();
      status = Status::Record;
      break;
    case Status::Stop:
      track.startRecording();
      status = Status::Record;
      break;
    }
  }

  if (buttons.stop.fallingEdge()) {
    Serial.println("Stop Button Press");
    track.stop();
    status = Status::Stop;
  }

  if (buttons.play.fallingEdge()) {
    Serial.println("Play Button Press");
    switch (status) {
    case Status::Play:
    case Status::Record:
      track.pause();
      status = Status::Pause;
      break;
    case Status::Pause:
      track.play();
      status = Status::Play;
      break;
    case Status::Stop:
      track.startPlaying();
      status = Status::Play;
    }
  }

  track.advance(status);

  showLevels(&peakLeft, &peakRight);

  // when using a microphone, continuously adjust gain
  if (input == AUDIO_INPUT_MIC) {
    adjustMicLevel();
  }
}
