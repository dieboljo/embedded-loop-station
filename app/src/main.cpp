#include <Bounce.h>
#include <config.h>
#include <play-sd-raw.hpp>
#include <usb_audio.h>

void adjustMicLevel();
void continuePlaying();
void continueRecording();
void startPlaying();
void startRecording();
void stopPlaying();
void stopRecording();

AudioControlSGTL5000 interface;
AudioInputI2S source;

/* USB output */
// change this to AudioOutputI2S for boards without DAC output
AudioOutputAnalog dac;
// AudioOutputI2S dac;
AudioOutputUSB sink;

/* Audio shield output */
// AudioOutputI2S sink;

AudioAnalyzePeak monitor;
App::AudioPlaySdRaw playback;
AudioRecordQueue recordQueue;

AudioConnection sourceToMonitor(source, monitor);
AudioConnection sourceToRecordQueue(source, recordQueue);
AudioConnection playbackToSinkLeft(playback, 0, sink, 0);
AudioConnection playbackToSinkRight(playback, 0, sink, 1);

/* USB output */
AudioConnection patchCord3(source, 0, dac, 0);

// which input on the audio shield will be used?
const int input = audioInput;

// Remember which mode we're doing
int mode = 0; // 0=stopped, 1=recording, 2=playing

// The file where data is recorded
File track1[2];
int frame1 = 0;
uint64_t position1 = 0;

// Bounce objects to easily and reliably read the buttons
Bounce buttonRecord = Bounce(buttonRecordPin, 8);
Bounce buttonStop = Bounce(buttonStopPin, 8); // 8 = 8 ms debounce time
Bounce buttonPlay = Bounce(buttonPlayPin, 8);

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
}

void loop() {
  // First, read the buttons
  buttonRecord.update();
  buttonStop.update();
  buttonPlay.update();

  // read the knob position (analog input A1)
  int knob = analogRead(A1);
  float vol = (float)knob / 1280.0;
  interface.volume(vol);
  if (msecs > 1000) {
    Serial.print("volume = ");
    Serial.println(vol);
    msecs = 0;
    if (monitor.available()) {
      Serial.print(monitor.read());
      Serial.println();
    }
  }

  // Respond to button presses
  if (buttonRecord.fallingEdge()) {
    Serial.println("Record Button Press");
    if (mode == 2)
      stopPlaying();
    if (mode == 0)
      startRecording();
  }
  if (buttonStop.fallingEdge()) {
    Serial.println("Stop Button Press");
    if (mode == 1)
      stopRecording();
    if (mode == 2)
      stopPlaying();
  }
  if (buttonPlay.fallingEdge()) {
    Serial.println("Play Button Press");
    if (mode == 1)
      stopRecording();
    if (mode == 0)
      startPlaying();
  }

  // If we're playing or recording, carry on...
  if (mode == 1) {
    continueRecording();
  }
  if (mode == 2) {
    continuePlaying();
  }

  // when using a microphone, continuously adjust gain
  if (input == AUDIO_INPUT_MIC)
    adjustMicLevel();
}

void startRecording() {
  Serial.println("startRecording");
  track1[frame1] = SD.open("RECORD.RAW", FILE_WRITE);
  if (track1[frame1]) {
    track1[frame1].seek(0); // move cursor to beginning
    recordQueue.begin();
    mode = 1;
  }
}

void continueRecording() {
  if (recordQueue.available() >= 2) {
    byte buffer[512];
    // Fetch 2 blocks from the audio library and copy
    // into a 512 byte buffer.  The Arduino SD library
    // is most efficient when full 512 byte sector size
    // writes are used.
    memcpy(buffer, recordQueue.readBuffer(), 256);
    recordQueue.freeBuffer();
    memcpy(buffer + 256, recordQueue.readBuffer(), 256);
    recordQueue.freeBuffer();
    // write all 512 bytes to the SD card
    track1[frame1].write(buffer, 512);
    // Uncomment these lines to see how long SD writes
    // are taking.  A pair of audio blocks arrives every
    // 5802 microseconds, so hopefully most of the writes
    // take well under 5802 us.  Some will take more, as
    // the SD library also must write to the FAT tables
    // and the SD card controller manages media erase and
    // wear leveling.  The recordQueue object can buffer
    // approximately 301700 us of audio, to allow time
    // for occasional high SD card latency, as long as
    // the average write time is under 5802 us.
    // elapsedMicros usec = 0;
    // Serial.print("SD write, us=");
    // Serial.println(usec);
  }
}

void stopRecording() {
  Serial.println("stopRecording");
  recordQueue.end();
  if (mode == 1) {
    while (recordQueue.available() > 0) {
      track1[frame1].write((byte *)recordQueue.readBuffer(), 256);
      recordQueue.freeBuffer();
    }
    track1[frame1].close();
  }
  mode = 0;
}

void startPlaying() {
  Serial.println("startPlaying");
  playback.play("RECORD.RAW", position1);
  mode = 2;
}

void continuePlaying() {
  if (!playback.isPlaying()) {
    playback.play("RECORD.RAW", 0);
  }
}

void stopPlaying() {
  Serial.println("stopPlaying");
  position1 = playback.getOffset();
  if (mode == 2)
    playback.stop();
  mode = 0;
}

void adjustMicLevel() {
  // TODO: read the peak1 object and adjust sgtl5000_1.micGain()
  // if anyone gets this working, please submit a github pull request :-)
}
