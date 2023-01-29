#include "play-sd-raw.h"
#include "track.h"
#include <Audio.h>
#include <Bounce.h>
#include <SD.h>
#include <SPI.h>
#include <SerialFlash.h>
#include <Wire.h>

#define SDCARD_CS_PIN BUILTIN_SDCARD
#define INPUT AUDIO_INPUT_MIC

enum Status = { stop, rec, play }

AudioControlSGTL5000 sgtl5000_1;
AudioInputI2S i2s2;
AudioOutputI2S i2s1;
Project::AudioPlaySdRaw playRaw1;
AudioRecordQueue queue1;
AudioAnalyzePeak peak1;

AudioConnection patchCord1(i2s2, 0, queue1, 0);
AudioConnection patchCord2(i2s2, 0, peak1, 0);
AudioConnection patchCord3(playRaw1, 0, i2s1, 0);
AudioConnection patchCord4(playRaw1, 0, i2s1, 1);

// Remember which mode we're doing
int mode = STOP; // 0=stopped, 1=recording, 2=playing

// The files where data is recorded
Track tracks[] = {Track("0A.WAV", "0B.WAV"), Track("1A.WAV", "1B.WAV"),
                  Track("2A.WAV", "2B.WAV"), Track("3A.WAV", "3B.WAV")};
int selected = 0;

File track1[2];
int frame1 = 0;
uint64_t position1 = 0;

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
  sgtl5000_1.inputSelect(myInput);
  sgtl5000_1.micGain(20);
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

  if (msecs > 15) {
    Serial.print("volume = ");
    Serial.println(vol);
    if (peak1.available()) {
      msecs = 0;
      float leftNumber = peak1.read();
      Serial.print(leftNumber);
      Serial.println();
    }
  }

  // Respond to button presses
  if (buttonRecord.fallingEdge()) {
    Serial.println("Record Button Press");
    if (mode == PLAY)
      stopPlaying();
    if (mode == STOP)
      startRecording();
  }
  if (buttonStop.fallingEdge()) {
    Serial.println("Stop Button Press");
    if (mode == REC)
      stopRecording();
    if (mode == PLAY)
      stopPlaying();
  }
  if (buttonPlay.fallingEdge()) {
    Serial.println("Play Button Press");
    if (mode == REC)
      stopRecording();
    if (mode == STOP)
      startPlaying();
  }

  // If we're playing or recording, carry on...
  if (mode == REC) {
    continueRecording();
  }
  if (mode == PLAY) {
    continuePlaying();
  }

  // when using a microphone, continuously adjust gain
  if (myInput == AUDIO_INPUT_MIC)
    adjustMicLevel();
}

void startRecording() {
  Serial.println("startRecording");
  if (SD.exists("RECORD.RAW")) {
    // The SD library writes new data to the end of the
    // file, so to start a new recording, the old file
    // must be deleted before new data is written.
    SD.remove("RECORD.RAW");
  }
  track1[frame1] = SD.open("RECORD.RAW", FILE_WRITE);
  if (track1[frame1]) {
    /* track1[frame1].seek(0); // move cursor to beginning */
    queue1.begin();
    mode = REC;
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
  if (mode == REC) {
    while (queue1.available() > 0) {
      track1[frame1].write((byte *)queue1.readBuffer(), 256);
      queue1.freeBuffer();
    }
    track1[frame1].close();
  }
  mode = STOP;
}

void startPlaying() {
  Serial.println("startPlaying");
  playRaw1.play("RECORD.RAW", position1);
  mode = PLAY;
}

void continuePlaying() {
  if (!playRaw1.isPlaying()) {
    // loop to start
    playRaw1.play("RECORD.RAW", 0);
  }
}

void pausePlaying() {
  Serial.println("pausePlaying");
  position1 = playRaw1.getOffset();
  if (mode == PLAY)
    playRaw1.stop();
  mode = STOP;
}

void stopPlaying() {
  Serial.println("stopPlaying");
  position1 = 0;
  if (mode == PLAY)
    playRaw1.stop();
  mode = STOP;
}

void adjustMicLevel() {
  // TODO: read the peak1 object and adjust sgtl5000_1.micGain()
  // if anyone gets this working, please submit a github pull request :-)
}
