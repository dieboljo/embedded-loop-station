#ifndef interface_h
#define interface_h

#include <Audio.h>
#include <Bounce.h>

class Interface {
  AudioControlSGTL5000 controller;

  AudioInputI2S source;
  AudioOutputI2S sink;

  // Bounce objects to easily and reliably read the buttons
  Bounce buttonRecord = Bounce(0, 8);
  Bounce buttonStop = Bounce(1, 8); // 8 = 8 ms debounce time
  Bounce buttonPlay = Bounce(2, 8);

public:
  bool begin();
};

#endif
