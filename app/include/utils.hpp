#ifndef UTILS_HPP
#define UTILS_HPP

#include <analyze_peak.h>
#include <control_sgtl5000.h>

void adjustMicLevel(){};
float adjustVolume(AudioControlSGTL5000 &interface);
void configureButtons();
void findSGTL5000(AudioControlSGTL5000 &interface);
void initializeInterface(AudioControlSGTL5000 &interface);
void initializeSdCard();
void initializeSerialCommunication();
void showLevels(AudioAnalyzePeak *peakL, AudioAnalyzePeak *peakR);

#endif
