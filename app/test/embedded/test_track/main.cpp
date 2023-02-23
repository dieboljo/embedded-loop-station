#include "Audio.h"
#include <config.h>
#include <include/track-test.hpp>
#include <unity.h>

AudioInputI2S source;
AudioControlSGTL5000 interface;
TrackTest trackBase("FILE1.WAV", "FILE2.WAV", &source);
const char *testFile = "FILE.WAV";

int i = 0;
bool loopTestsStarted = false;
bool stopLoopTests = false;

/*
## Test-specific Setup Utilities
*/

/* int recordAndPlay(Track *t, int i) {
  AudioNoInterrupts();
  if (i == 0) {
    t->startRecording();
  } else if (i < 20) {
    t->advance(Status::Record);
  } else if (i == 20) {
    t->stopRecording();
    t->startPlayback();
  } else if (i > 20 && i < 40) {
    t->advance(Status::Play);
  } else {
  }
  AudioInterrupts();
  return i + 1;
} */

/*
## Tests
*/
void test_begin() {
  TrackTest track = trackBase;
  bool hasBegun = track.begin();
  TEST_ASSERT_TRUE(hasBegun);
}

/* void test_bufferWrite() {
  File test = SD.open(testFile, FILE_WRITE);
  TEST_ASSERT_TRUE(SD.exists(testFile));
} */

/* void test_pause() {
  TrackTest track = trackBase;
  i = recordAndPlay(&track, i);
  if (i == 50) {
    track.pausePlayback();

    // Just testing copy constructor worked as expected
    TEST_ASSERT_EQUAL_INT32(0, trackBase.audio.getOffset());

    TEST_ASSERT_GREATER_THAN_INT32(0, track.audio.getOffset());
    TEST_ASSERT_FALSE(track.audio.isPlaying());
  }
} */

/* void test_play() {
  TrackTest track = trackBase;
  track.startPlayback();
  bool opened = track.advance(Status::Play);
  TEST_ASSERT_TRUE(opened);
} */

/* void test_record() {
  TrackTest track = trackBase;
  track.startRecording();
  bool recording = track.advance(Status::Record);
  TEST_ASSERT_TRUE(recording);
} */

/* void test_recordQueue() {
  TrackTest track = trackBase;
  i = recordAndPlay(&track, i);
  if (i == 50) {
    TEST_ASSERT_GREATER_THAN_INT32(0, track.audio.lengthMillis());
  }
} */

/* void test_removeFile() {
  const char *filename = testFile;
  File test = SD.open(filename, FILE_WRITE);
  TEST_ASSERT_TRUE(SD.exists(filename));
  SD.remove(filename);
  TEST_ASSERT_FALSE(SD.exists(filename));
} */

/* void test_stop() {
  TrackTest track = trackBase;
  i = recordAndPlay(&track, i);
  if (i == 50) {
    track.stopPlayback();
    TEST_ASSERT_EQUAL_INT32(track.audio.getOffset(), 0);
    TEST_ASSERT_FALSE(track.audio.isPlaying());
  }
} */

/* void test_swapBuffers() {
  TrackTest track = trackBase;
  track.stopRecording();
  TEST_ASSERT_EQUAL_STRING("FILE2.RAW", track.getReadFileName());
  TEST_ASSERT_EQUAL_STRING("FILE1.RAW", track.getWriteFileName());
} */

/*
## Test Runner
*/

void setUp(void) {}

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
  /* RUN_TEST(test_bufferWrite);
  delay(500); */
  /* RUN_TEST(test_removeFile);
  delay(500); */
  /* RUN_TEST(test_play);
  delay(500); */
  /* RUN_TEST(test_record);
  delay(500); */
  /* RUN_TEST(test_swapBuffers);
  delay(500); */
}

void loop() {
  if (!loopTestsStarted) {
    // Only run these tests once, not in every loop
    loopTestsStarted = true;
    /* RUN_TEST(test_recordQueue);
    delay(500); */
    /* RUN_TEST(test_stop);
    delay(500); */
    /* RUN_TEST(test_pause);
    delay(500); */
    // Toggle the stop flag so the program stops looping
    stopLoopTests = true;
  }
  if (stopLoopTests) {
    UNITY_END();
  }
  delay(1000);
}
