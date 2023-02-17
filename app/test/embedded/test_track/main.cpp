#include <config.h>
#include <track.hpp>
#include <unity.h>

AudioInputI2S source;
AudioControlSGTL5000 interface;
Track track("FILE.RAW", &source);

boolean initialized = false;
int i = 0;
int j = 0;

elapsedMillis msecs;

/*
## Tests
*/
void test_bufferWrite() {
  if (msecs < 1000) {
    track.advance(Status::Record);
    i++;
  }
  if (!initialized) {
    track.play();
    initialized = true;
  }
  if (msecs < 2000) {
    track.playback.update();
    j++;
  } else {
    track.playback.stop();
    TEST_ASSERT_GREATER_THAN(0, track.playback.lengthMillis());
  }
}

/*
## Test Runner
*/

void setUp(void) { track.begin(); }

void tearDown(void) { i = 0, j = 0; }

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
}

void loop() {
  RUN_TEST(test_bufferWrite);
  if (msecs > 3000)
    UNITY_END();
}
