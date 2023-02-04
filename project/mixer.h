#ifndef mixer_h
#define mixer_h

#include <Audio.h>

class Mixer {
  AudioAnalyzePeak monitor;
  AudioMixer4 sinkInput;
};

#endif
