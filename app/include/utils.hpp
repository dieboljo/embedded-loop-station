#ifndef UTILS_HPP
#define UTILS_HPP

#include "types.hpp"
#include <track-controller.hpp>

void adjustMicLevel();
void adjustPan(float *currentPan, TrackController &controller, Mode mode);
float adjustVolume(AudioControlSGTL5000 &interface);
void configureButtons();
void findSGTL5000(AudioControlSGTL5000 &interface);
void initializeInterface(AudioControlSGTL5000 &interface);
void initializeSdCard();
void initializeSerialCommunication();
void monitorAudioEngine();
void readButtons(Buttons &buttons);
void showLevels(AudioAnalyzePeak *peakL, AudioAnalyzePeak *peakR,
                elapsedMillis *ms, const char *label);
void showInputLevels(AudioAnalyzePeak *peakL, AudioAnalyzePeak *peakR);
void showOutputLevels(AudioAnalyzePeak *peakL, AudioAnalyzePeak *peakR);

#endif
