#include "Audio.h"
#include <config.h>
#include <track-controller.hpp>
#include <unity.h>

AudioInputI2S source;
AudioControlSGTL5000 interface;
TrackController controllerBase(&source);
TrackController controller = controllerBase;

int i = 0;
bool loopTestsStarted = false;
bool stopLoopTests = false;

/*
## Tests
*/

void test_begin() {
  bool hasBegun = controller.begin();
  TEST_ASSERT_TRUE(hasBegun);
}

void test_setSelectedTrack() {
  controller.setSelectedTrack(2);
  TEST_ASSERT_EQUAL_INT(controller.getSelectedTrack(), 2);
  int overflow = controller.getNumTracks();
  controller.setSelectedTrack(overflow);
  TEST_ASSERT_NOT_EQUAL_INT(controller.getSelectedTrack(), overflow);
}

void test_startPlaying() {
  controller.begin();
  TEST_ASSERT_TRUE(controller.startPlaying());
}

void test_startRecording() {
  controller.begin();
  TEST_ASSERT_TRUE(controller.startRecording(Mode::Replace));
}

void test_stop() {
  controller.begin();
  controller.startPlaying();
  TEST_ASSERT_TRUE(controller.stop());
}

/*
## Test Runner
*/

void setUp(void) { controller = controllerBase; }

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
  RUN_TEST(test_setSelectedTrack);
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
