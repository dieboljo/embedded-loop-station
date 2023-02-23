#ifndef CONFIG_H
#define CONFIG_H

#include <AudioControl.h>
#include <SD.h>

// SD pins for Teensy 4.1
const int sdCardCsPin = BUILTIN_SDCARD;
const int sdCardMosiPin = 11;
const int sdCardSckPin = 13;

// Button pins
const int buttonRecordPin = 3;
const int buttonStopPin = 4;
const int buttonPlayPin = 5;

// which input on the audio shield will be used?
const int audioInput = AUDIO_INPUT_MIC;

#endif
