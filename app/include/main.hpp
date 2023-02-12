#ifndef main_hpp
#define main_hpp

#include <Audio.h>

class Main {
  void adjustMicLevel();
  void continuePlaying();
  void continueRecording();
  void startPlaying();
  void startRecording();
  void stopPlaying();
  void stopRecording();

public:
  Main() {};
  void loop();
  void setup();
};

#endif
