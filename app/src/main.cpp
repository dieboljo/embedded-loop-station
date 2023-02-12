#include "play-sd-raw.hpp"
#include <Bounce.h>

#define SDCARD_CS_PIN BUILTIN_SDCARD
#define SDCARD_MOSI_PIN 11 // not actually used
#define SDCARD_SCK_PIN 13  // not actually used

void adjustMicLevel();
void continuePlaying();
void continueRecording();
void startPlaying();
void startRecording();
void stopPlaying();
void stopRecording();

AudioControlSGTL5000 sgtl5000_1;
AudioInputI2S i2s2;
AudioOutputI2S i2s1;
App::AudioPlaySdRaw playRaw1;
AudioRecordQueue queue1;

AudioConnection patchCord1(i2s2, 0, queue1, 0);
AudioConnection patchCord3(playRaw1, 0, i2s1, 0);
AudioConnection patchCord4(playRaw1, 0, i2s1, 1);

// which input on the audio shield will be used?
const int myInput = AUDIO_INPUT_MIC;

// Remember which mode we're doing
int mode = 0; // 0=stopped, 1=recording, 2=playing

// The file where data is recorded
File track1[2];
int frame1 = 0;
uint64_t position1 = 0;

// Bounce objects to easily and reliably read the buttons
Bounce buttonRecord = Bounce(0, 8);
Bounce buttonStop = Bounce(1, 8); // 8 = 8 ms debounce time
Bounce buttonPlay = Bounce(2, 8);

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
  sgtl5000_1.inputSelect(myInput);
  sgtl5000_1.micGain(4);
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
  /* Serial.print("volume = ");
  Serial.println(vol); */

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
  if (myInput == AUDIO_INPUT_MIC)
    adjustMicLevel();
}

void startRecording() {
  Serial.println("startRecording");
  track1[frame1] = SD.open("RECORD.RAW", FILE_WRITE);
  if (track1[frame1]) {
    track1[frame1].seek(0); // move cursor to beginning
    queue1.begin();
    mode = 1;
  }
}

void continueRecording() {
  if (queue1.available() >= 2) {
    byte buffer[512];
    // Fetch 2 blocks from the audio library and copy
    // into a 512 byte buffer.  The Arduino SD library
    // is most efficient when full 512 byte sector size
    // writes are used.
    memcpy(buffer, queue1.readBuffer(), 256);
    queue1.freeBuffer();
    memcpy(buffer + 256, queue1.readBuffer(), 256);
    queue1.freeBuffer();
    // write all 512 bytes to the SD card
    track1[frame1].write(buffer, 512);
    // Uncomment these lines to see how long SD writes
    // are taking.  A pair of audio blocks arrives every
    // 5802 microseconds, so hopefully most of the writes
    // take well under 5802 us.  Some will take more, as
    // the SD library also must write to the FAT tables
    // and the SD card controller manages media erase and
    // wear leveling.  The queue1 object can buffer
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
  queue1.end();
  if (mode == 1) {
    while (queue1.available() > 0) {
      track1[frame1].write((byte *)queue1.readBuffer(), 256);
      queue1.freeBuffer();
    }
    track1[frame1].close();
  }
  mode = 0;
}

void startPlaying() {
  Serial.println("startPlaying");
  playRaw1.play("RECORD.RAW", position1);
  mode = 2;
}

void continuePlaying() {
  if (!playRaw1.isPlaying()) {
    playRaw1.play("RECORD.RAW", 0);
  }
}

void stopPlaying() {
  Serial.println("stopPlaying");
  position1 = playRaw1.getOffset();
  if (mode == 2)
    playRaw1.stop();
  mode = 0;
}

void adjustMicLevel() {
  // TODO: read the peak1 object and adjust sgtl5000_1.micGain()
  // if anyone gets this working, please submit a github pull request :-)
}
