#include <Bounce.h>
#include <play-sd-raw.hpp>
#include <ILI9341_t3.h> // Display library
#include <XPT2046_Touchscreen.h> // Touchscreen library
#include <font_Arial.h> // Font for display

#define SDCARD_CS_PIN BUILTIN_SDCARD
#define SDCARD_MOSI_PIN 11 // not actually used
#define SDCARD_SCK_PIN 13  // not actually used

// touchscreen offset for four corners
#define TS_MINX 400
#define TS_MINY 400
#define TS_MAXX 3879
#define TS_MAXY 3843

// LCD control pins defined by board
#define TFT_CS 40
#define TFT_DC  9

// Use main SPI bus MOSI=11, MISO=12, SCK=13 with different control pins
ILI9341_t3 tft = ILI9341_t3(TFT_CS, TFT_DC);


// Touch screen control pins defined by board
// TIRQ interrupt if used is on pin 2
#define TS_CS  41
XPT2046_Touchscreen ts(TS_CS);

// Screen is 240x320

// Define Track info 
#define TRACK_X 10 // X-axis pixel offset from left of screen
#define TRACK_Y 10
#define TRACK_W 300
#define TRACK_H 50

// Define Stop button location and size
#define STOP_X 10
#define STOP_Y 70
#define STOP_W 70
#define STOP_H 32

// Define Record button location and size
#define RECORD_X 90
#define RECORD_Y 70
#define RECORD_W 90
#define RECORD_H 32

// Define Play button location and size
#define PLAY_X 190
#define PLAY_Y 70 // 8 pixel Y-axis offset from "Track Info"
#define PLAY_W 105
#define PLAY_H 32

// Define Volume location and size
#define VOLUME_X 260
#define VOLUME_Y 180
#define VOLUME_W 50
#define VOLUME_H 50

// Define Mode location and size
#define MODE_X 10
#define MODE_Y 115
#define MODE_W 170
#define MODE_H 32

//Font for buttons
#define BUTTON_FONT Arial_14

// Playback function declarations
void adjustMicLevel();
void continuePlaying();
void continueRecording();
void startPlaying();
void startRecording();
void stopPlaying();
void stopRecording();

// Display function declarations
void displayTrack(String name);
void displayVolume(float volume);
void displayMode(int modeValue);
void SetPlayButton (boolean);   // Handles Play button when touched
void SetStopButton (boolean);  // Handles Stop button when touched
void SetRecordButton (boolean);   // Handles Record button when touched

AudioControlSGTL5000 sgtl5000_1;
AudioInputI2S i2s2;
AudioOutputI2S i2s1;
App::AudioPlaySdRaw playRaw1;
App::AudioPlaySdRaw playSD; // Added SD for SD demo
AudioRecordQueue queue1;

AudioConnection patchCord1(i2s2, 0, queue1, 0);
AudioConnection patchCord3(playRaw1, 0, i2s1, 0);
AudioConnection patchCord4(playRaw1, 0, i2s1, 1);

AudioConnection patchCord5(playSD, 0, i2s1, 0); //????
AudioConnection patchCord6(playSD, 0, i2s1, 1);

// which input on the audio shield will be used?
const int myInput = AUDIO_INPUT_MIC;

// Remember which mode we're doing
int mode = 0; // 0=stopped, 1=recording, 2=playing
int actualMode = 0;

float volumeChange;
int modeChange;

// The file where data is recorded
File track1[2];
int frame1 = 0;
uint64_t position1 = 0;

// Bounce objects to easily and reliably read the buttons - Bounce(pin, delay-time)
Bounce buttonRecord = Bounce(4, 8);
Bounce buttonStop = Bounce(3, 8); // 8 = 8 ms debounce time
Bounce buttonPlay = Bounce(5, 8);

void setup() {
  // Configure the pushbutton pins
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);

  // Audio connections require memory, and the record queue
  // uses this memory to buffer incoming audio.
  AudioMemory(60);

  // Enable the audio shield, select input, and enable output
  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(myInput);
  sgtl5000_1.micGain(4);
  sgtl5000_1.volume(0.5);

  // Setup LCD screen
  tft.begin();
  tft.setRotation(3);
  ts.begin();
  ts.setRotation(1);
  tft.fillScreen(ILI9341_LIGHTGREY);

  //Draw buttons
  SetPlayButton (false);
  SetStopButton (false);
  SetRecordButton (false);
  String defaultTrack = "Press play to begin!";
  displayTrack(defaultTrack);

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
  int volumeKnob = analogRead(A1);
  float vol = (float)volumeKnob / 1280.0;
  // Volume pot is very sensitive - need to set a change range
  if (volumeChange <= (vol * .95) || volumeChange >= (vol * 1.05)){
    displayVolume(vol);
    // update volume if changed
    volumeChange = vol;
  }
  sgtl5000_1.volume(vol);

  //Serial.println("volume is ");
  //Serial.print(volumeKnob);

  int modeKnob = analogRead(A2);
  modeKnob = modeKnob/10;
  //int libRemainder = libraryKnob % 10;
  if (modeChange <= (modeKnob * .90) || modeChange >= (modeKnob * 1.10))  {
    displayMode(modeChange);
    modeChange = modeKnob;
  }
  //Serial.println("modeChange is ");
  //Serial.print(modeChange);
  
  


  // Respond to button presses
  if (buttonRecord.fallingEdge()) {
    Serial.println("Record Button Press");
    if (mode == 2)
      stopPlaying(); // mode = 0
      SetStopButton(false);
      SetRecordButton(true);
      SetPlayButton(false);
    if (mode == 0)
      startRecording(); // mode = 1
      SetRecordButton(true);
      SetStopButton(false);
      SetPlayButton(false);
  }
  if (buttonStop.fallingEdge()) {
    Serial.println("Stop Button Press");
    if (mode == 1)
      stopRecording(); // mode = 0
      SetStopButton(true);
      SetRecordButton(false);
      SetPlayButton(false);
    if (mode == 2)
      stopPlaying(); // mode = 0
      SetStopButton(true);
      SetRecordButton(false);
      SetPlayButton(false);
  }
  if (buttonPlay.fallingEdge()) {
    Serial.println("Play Button Press");
    if (mode == 1)
      stopRecording(); // mode = 0
      SetStopButton(false);
      SetRecordButton(false);
      SetPlayButton(true);
    if (mode == 0)
      startPlaying(); // mode = 2
      SetStopButton(false);
      SetRecordButton(false);
      SetPlayButton(true);
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

// not sure why this isn't playing from the SD card. if you repalce RECORD.RAW with SDTEST1.WAV it plays

// added different different playback mode
void startPlaying() {
  if (mode == 0 && actualMode == 1){
    Serial.println("startPlaying");
    displayTrack("Playing RECORD.RAW");
    playRaw1.play("RECORD.RAW", position1);
    mode = 2;
  }
  if (mode == 0 && actualMode == 2){
    displayTrack("Playing SDTEST1");
    playSD.play("SDTEST1.WAV", position1);
    mode = 2;
  }
}

/*
void startPlaying() {
  Serial.println("startPlaying");
  displayTrack("Playing RECORD.RAW");
  playRaw1.play("RECORD.RAW", position1);
  mode = 2;
}
*/

void continuePlaying() {
  if (mode == 0 && actualMode == 1){
    if (!playRaw1.isPlaying()) {
      playRaw1.play("RECORD.RAW", 0);
    }
  }
  if (mode == 0 && actualMode == 2){
    if (!playSD.isPlaying()) {
        playSD.play("SDTEST1.WAV", 0);
      }
  }
}

/*
void continuePlaying() {
  if (!playRaw1.isPlaying()) {
    playRaw1.play("RECORD.RAW", 0);
  }
}
*/

void stopPlaying() {
  if (mode == 2 && actualMode == 1){
    Serial.println("stopPlaying");
    displayTrack("RECORD.RAW Stopped");
    position1 = playRaw1.getOffset();
    if (mode == 2)
      playRaw1.stop();
  }
  if (mode == 2 && actualMode == 2){
    displayTrack("SDTEST1 STOPPED");
    if (mode == 2)
      playSD.stop();
  }
  mode = 0;
}

/*
void stopPlaying() {
  Serial.println("stopPlaying");
  displayTrack("RECORD.RAW Stopped");
  position1 = playRaw1.getOffset();
  if (mode == 2)
    playRaw1.stop();
  mode = 0;
}
*/

///////////////// Display functions - Will move to separate class ////////////////////////////

void adjustMicLevel() {
  // TODO: read the peak1 object and adjust sgtl5000_1.micGain()
  // if anyone gets this working, please submit a github pull request :-)
}

void SetPlayButton (boolean audio){
  tft.setCursor(PLAY_X + 8, PLAY_Y + 8);
  tft.setFont(BUTTON_FONT);
  tft.setTextColor(ILI9341_WHITE);
  
  if (!audio) {  // button is set inactive, redraw button inactive
    tft.fillRoundRect(PLAY_X, PLAY_Y, PLAY_W, PLAY_H, 4, ILI9341_BLACK);
    tft.print ("Play Audio");
  }
  else {          // button is active, redraw button active
    tft.fillRoundRect(PLAY_X, PLAY_Y, PLAY_W, PLAY_H, 4, ILI9341_GREEN);
    tft.print ("Playing");
  }
}

void SetRecordButton (boolean audio){
  tft.setCursor(RECORD_X + 8, RECORD_Y + 8);
  tft.setFont(BUTTON_FONT);
  tft.setTextColor(ILI9341_WHITE);
  
  if (!audio) {  // button is set inactive, redraw button inactive
    tft.fillRoundRect(RECORD_X, RECORD_Y, RECORD_W, RECORD_H, 4, ILI9341_BLACK);
    tft.print ("Record");
  }
  else {          // button is active, redraw button active
    tft.fillRoundRect(RECORD_X, RECORD_Y, RECORD_W, RECORD_H, 4, ILI9341_GREEN);
    tft.print ("Recording");
  }
}

void SetStopButton (boolean audio){
  tft.setCursor(STOP_X + 8, STOP_Y + 8);
  tft.setFont(BUTTON_FONT);
  tft.setTextColor(ILI9341_WHITE);
  
  if (!audio) {  // button is set inactive, redraw button inactive
    tft.fillRoundRect(STOP_X, STOP_Y, STOP_W, STOP_H, 4, ILI9341_RED);
    tft.print ("Stop");
  }
  else {          // button is active, redraw button active
    tft.fillRoundRect(STOP_X, STOP_Y, STOP_W, STOP_H, 4, ILI9341_BLACK);
    tft.print ("Stop");
  }
}

void displayTrack(String name){
  tft.setCursor(TRACK_X + 50, TRACK_Y + 16);
  tft.setFont(BUTTON_FONT);
  tft.setTextColor(ILI9341_WHITE);
  tft.fillRoundRect(TRACK_X, TRACK_Y, TRACK_W, TRACK_H, 8, ILI9341_BLACK);
  tft.print(name);
}

void displayVolume(float volume){

  float value = volume/0.076;
  tft.setCursor(VOLUME_X + 12, VOLUME_Y + 8);
  tft.setFont(Arial_10);
  tft.setTextColor(ILI9341_WHITE);
  tft.fillRoundRect(VOLUME_X, VOLUME_Y, VOLUME_W, VOLUME_H, 8, ILI9341_BLACK);
  tft.println("Vol");
  tft.setCursor(VOLUME_X + 7, VOLUME_Y + 28);
  tft.print(value);
}

void displayMode(int modeValue){
  //tft.setCursor(MODE_X + 8, MODE_Y + 8);
  tft.setFont(BUTTON_FONT);
  tft.setTextColor(ILI9341_WHITE);
  
  //Serial.println(modeValue);
  if (modeValue < 60 && actualMode != 1){
    actualMode = 1;
    Serial.println(actualMode);
    tft.setCursor(MODE_X + 8, MODE_Y + 8);
    tft.fillRoundRect(MODE_X, MODE_Y, MODE_W, MODE_H, 8, ILI9341_BLACK);
    tft.println("Record Mode");
  }
  if (modeValue > 65 && actualMode != 2){
    actualMode = 2;
    tft.setCursor(MODE_X + 8, MODE_Y + 8);
    tft.fillRoundRect(MODE_X, MODE_Y, MODE_W, MODE_H, 8, ILI9341_BLACK);
    tft.println("Library Mode");
  }
}