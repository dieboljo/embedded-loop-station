#ifndef CONFIG_H
#define CONFIG_H

#include <AudioControl.h>
#include <SD.h>

#include "types.hpp"

// SD pins for Teensy 4.1
const int sdCardCsPin = BUILTIN_SDCARD;
const int sdCardMosiPin = 11;
const int sdCardSckPin = 13;

// Button pins
const int buttonRecordPin = 3;
const int buttonStopPin = 4;
const int buttonPlayPin = 5;

// Which input on the audio shield will be used?
const int input = AUDIO_INPUT_MIC;

// Knob pins
const int volumePin = A1;
const int panPin = A2;
const int fadePin = A3;

// Knob types
const PotTaper volumeTaper = PotTaper::Linear;
const PotTaper panTaper = PotTaper::Logarithmic;
const PotTaper fadeTaper = PotTaper::Logarithmic;

// 8 = 8 ms debounce time
const int debounceTime = 8;

const bool monitorInput = false;
const bool monitorOutput = false;

#endif
