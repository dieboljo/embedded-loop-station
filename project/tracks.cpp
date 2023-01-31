#include "tracks.h"

void Tracks::setSelected(int i) {
  selectedTrackIdx = i;
  selected = tracks[i];
}

Track Tracks::getTrack(int i) { return tracks[i]; }
