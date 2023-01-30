#include "tracks.h"

void Tracks::setSelected(int i) {
  selectedTrackIdx = i;
  selected = tracks[i];
}
