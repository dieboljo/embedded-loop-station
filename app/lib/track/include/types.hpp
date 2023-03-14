#ifndef TYPES_HPP
#define TYPES_HPP

#include <Bounce.h>

// Bounce objects to easily and reliably read the buttons
struct Buttons {
  /* Bounce left;
  Bounce nav;
  Bounce select;
  Bounce right; */
  Bounce stop;
  Bounce record;
  Bounce play;
  Bounce mode;
};

enum class Mode { Replace, Overdub, Reverse };
enum class Status { Stop, Record, Play, Pause };

#endif
