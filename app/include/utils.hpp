#ifndef UTILS_HPP
#define UTILS_HPP

#include "types.hpp"
#include <track-controller.hpp>

void adjustMicLevel();
void configureButtons();
void findSGTL5000(AudioControlSGTL5000 &interface);
void initializeInterface(AudioControlSGTL5000 &interface);
void initializeSdCard();
void initializeSerialCommunication();
void monitorAudioEngine();
void readButtons(Buttons &buttons);
void readFade(AppState &appState);
void readPan(AppState &appState);
void readVolume(AppState &appState);
void showLevels(
    AudioAnalyzePeak *peakL, AudioAnalyzePeak *peakR, elapsedMillis *ms,
    const char *label
);
void showInputLevels(AudioAnalyzePeak *peakL, AudioAnalyzePeak *peakR);
void showOutputLevels(AudioAnalyzePeak *peakL, AudioAnalyzePeak *peakR);

#endif
