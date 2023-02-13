#include <config.h>
#include <track.hpp>
#include <unity.h>

int initialized = 0;

AudioInputI2S source;
AudioControlSGTL5000 interface;
Track track("FILE_1.WAV", "FILE_2.WAV", &source, 1);

elapsedMillis msecs;

void setUp(void) {}

void tearDown(void) {}

void setup() {
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

  track.begin();

  // Wait ~2 seconds before the Unity test runner
  // establishes connection with a board Serial interface
  delay(2000);

  UNITY_BEGIN();
}

void loop() {
  if (!initialized) {
    track.record();
  }

  if (msecs > 10000) {
    track.stop();
    track.play();
    TEST_ASSERT_GREATER_THAN(0, track.playback.lengthMillis());
    UNITY_END();
  }

  track.advance(1);
}
