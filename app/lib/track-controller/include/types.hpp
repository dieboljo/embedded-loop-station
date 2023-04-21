#ifndef TYPES_HPP
#define TYPES_HPP

#include <Bounce.h>

// Bounce objects to easily and reliably read the buttons
struct Buttons {
  Bounce stop;
  Bounce record;
  Bounce play;
  Bounce mode;
  Bounce save;
};

enum class Mode { Replace, Overdub };
enum class Status { Stop, Record, Play, Pause };

#endif
