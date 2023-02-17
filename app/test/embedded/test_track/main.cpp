#include <config.h>
#include <track.hpp>
#include <unity.h>

AudioInputI2S source;
AudioControlSGTL5000 interface;
Track track("FILE.RAW", &source);

boolean initialized = false;

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

void test_startRecord() {
  bool recording = track.record();
  TEST_ASSERT_TRUE(recording);
}

/*
## Test Runner
*/

void setUp(void) {
  track.begin();
  track.playback.begin();
}

void tearDown(void) { initialized = false; }

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
  UNITY_END();
}

void loop() {}
