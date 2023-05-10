#ifndef TYPES_HPP
#define TYPES_HPP

#include <Arduino.h>
#include <Bounce.h>

// Bounce objects to easily and reliably read the buttons
struct Buttons {
  Bounce stop;
  Bounce record;
  Bounce play;
  Bounce mode;
  Bounce save;
  Bounce nextTrack;
  Bounce clearTrack;
};

enum class Mode { Replace, Overdub };
enum class Status { Stop, Record, Play, Pause };

struct AppState {
  float fade;
  uint32_t length;
  Mode mode;
  float pan;
  uint32_t position;
  bool saving;
  Status status;
  int track;
  float volume;
};

#endif
