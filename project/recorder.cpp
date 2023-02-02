#include "recorder.h"

void Recorder::setSelected(int i) {
  selectedTrackIdx = i;
  selected = tracks[i];
}

Track Recorder::getTrack(int i) { return tracks[i]; }
