#include <config.h>
#include <track.hpp>
#include <unity.h>

AudioInputI2S source;
AudioControlSGTL5000 interface;
Track track("FILE.RAW", &source);

int i = 0;
bool loopTestsStarted = false;
bool stop = false;

/*
## Test-specific Setup Utilities
*/

void recordAndPlay() {
  if (i < 20) {
    track.advance(Status::Record);
    i++;
  } else if (i == 20) {
    track.stop();
    i++;
  } else if (i > 20 && i < 40) {
    track.advance(Status::Play);
    i++;
  } else if (i < 50) {
    track.play();
    i++;
  }
}

/*
## Tests
*/
void test_bufferWrite() {
  File test = SD.open("FILE.RAW", FILE_WRITE);
  TEST_ASSERT_TRUE(SD.exists("FILE.RAW"));
}

void test_openPlayback() {
  bool opened = track.play();
  TEST_ASSERT_TRUE(opened);
}

void test_recordQueue() {
  recordAndPlay();
  if (i == 50) {
    TEST_ASSERT_GREATER_THAN_INT32(0, track.playback.lengthMillis());
  }
}

void test_startRecord() {
  bool recording = track.record();
  TEST_ASSERT_TRUE(recording);
}

void test_stop() {
  recordAndPlay();
  if (i == 50) {
    track.stop();
    TEST_ASSERT_EQUAL_INT32(track.playback.getOffset(), 0);
    TEST_ASSERT_FALSE(track.playback.isPlaying());
  }
}

void test_pause() {
  recordAndPlay();
  if (i == 50) {
    track.pause();
    TEST_ASSERT_GREATER_THAN_INT32(0, track.playback.getOffset());
    TEST_ASSERT_FALSE(track.playback.isPlaying());
  }
}

/*
## Test Runner
*/

void setUp(void) {
  track.begin();
  track.playback.begin();
}

void tearDown(void) { i = 0; }

void setup() {
  Serial.begin(9600);

  AudioMemory(60);

  // Enable the audio shield, select input, and enable output
  interface.enable();
  interface.inputSelect(audioInput);
  interface.micGain(4);
  interface.volume(0.5);

  // Initialize the SD card
  SPI.setMOSI(sdCardMosiPin);
  SPI.setSCK(sdCardSckPin);
  if (!(SD.begin(sdCardCsPin))) {
    // stop here if no SD card, but print a message
    Serial.println("Unable to access the SD card");
    UNITY_END();
  }

  // Wait ~2 seconds before the Unity test runner
  // establishes connection with a board Serial interface
  delay(2000);

  UNITY_BEGIN();
  RUN_TEST(test_bufferWrite);
  RUN_TEST(test_openPlayback);
  RUN_TEST(test_startRecord);
}

void loop() {
  if (!loopTestsStarted) {
    loopTestsStarted = true;
    RUN_TEST(test_recordQueue);
    delay(500);
    RUN_TEST(test_stop);
    delay(500);
    RUN_TEST(test_pause);
    delay(500);
    stop = true;
  }
  if (stop) {
    UNITY_END();
  }
}
