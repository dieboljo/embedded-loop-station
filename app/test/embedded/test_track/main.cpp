#include "Audio.h"
#include <config.h>
#include <include/track-test.hpp>
#include <unity.h>

AudioInputI2S source;
AudioControlSGTL5000 interface;
TrackTest trackBase("FILE1.WAV", "FILE2.WAV", &source);
TrackTest track = trackBase;
const char *testFile = "FILE.WAV";

int i = 0;
bool loopTestsStarted = false;
bool stopLoopTests = false;

/*
## Tests
*/

void test_begin() {
  bool hasBegun = track.begin();
  TEST_ASSERT_TRUE(hasBegun);
}

// TODO: Fix this test (need to wait several loops)
void test_pause() {
  track.begin();
  track.startRecording(Mode::Replace);
  TEST_ASSERT_TRUE(track.pause());
}

void test_startPlaying() {
  track.begin();
  TEST_ASSERT_TRUE(track.startPlaying());
}

void test_startRecording() {
  track.begin();
  TEST_ASSERT_TRUE(track.startRecording(Mode::Replace));
}

void test_stop() {
  track.begin();
  track.startPlaying();
  TEST_ASSERT_TRUE(track.stop());
}

/*
## Test Runner
*/

void setUp(void) { track = trackBase; }

void tearDown(void) {
  i = 0;
  loopTestsStarted = false;
  stopLoopTests = false;
}

void setup() {
  Serial.begin(9600);

  AudioMemory(60);

  // Enable the audio shield, select input, and enable output
  interface.enable();
  interface.inputSelect(input);
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
  RUN_TEST(test_begin);
  delay(500);
}

void loop() {
  if (!loopTestsStarted) {
    // Only run these tests once, not in every loop
    loopTestsStarted = true;
    RUN_TEST(test_startPlaying);
    delay(500);
    RUN_TEST(test_startRecording);
    delay(500);
    RUN_TEST(test_pause);
    delay(500);
    RUN_TEST(test_stop);
    delay(500);
    // Toggle the stop flag so the program stops looping
    stopLoopTests = true;
  }
  if (stopLoopTests) {
    UNITY_END();
  }
  delay(1000);
}
