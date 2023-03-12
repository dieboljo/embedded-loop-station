#ifndef UTILS_HPP
#define UTILS_HPP

#include <track.hpp>

void adjustMicLevel();
void adjustPan(float *currentPan, Track &track, Mode mode);
float adjustVolume(AudioControlSGTL5000 &interface);
void configureButtons();
void findSGTL5000(AudioControlSGTL5000 &interface);
void initializeInterface(AudioControlSGTL5000 &interface);
void initializeSdCard();
void initializeSerialCommunication();
void monitorAudioEngine();
void showLevels(AudioAnalyzePeak *peakL, AudioAnalyzePeak *peakR,
                elapsedMillis *ms, const char *label);
void showInputLevels(AudioAnalyzePeak *peakL, AudioAnalyzePeak *peakR);
void showOutputLevels(AudioAnalyzePeak *peakL, AudioAnalyzePeak *peakR);

#endif
