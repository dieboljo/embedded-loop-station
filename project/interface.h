#ifndef interface_h
#define interface_h

#include "constants.h"
#include "recorder.h"
#include <Audio.h>
#include <Bounce.h>

class Interface {
  AudioControlSGTL5000 controller;

  AudioAnalyzePeak monitor;
  AudioInputI2S source;
  AudioOutputI2S sink;

  Recorder recorder;

  AudioConnection sourceToMonitor;
  AudioConnection sinkInputToSinkChannelLeft;
  AudioConnection sinkInputToSinkChannelRight;

  void adjustMicLevel() {}

  // Bounce objects to easily and reliably read the buttons
  /* Bounce buttonRecord = Bounce(0, 8);
  Bounce buttonStop = Bounce(1, 8); // 8 = 8 ms debounce time
  Bounce buttonPlay = Bounce(2, 8); */

public:
  Interface()
      : recorder(&source), sourceToMonitor(source, 0, monitor, 0),
        sinkInputToSinkChannelLeft(recorder.mix, 0, sink, 0),
        sinkInputToSinkChannelRight(recorder.mix, 0, sink, 1){};
  void advance();
  bool begin();
  void setVolume(float vol) { controller.volume(vol); }
  float getPeak();
  void handleButtonPress(Button button);
};

#endif
