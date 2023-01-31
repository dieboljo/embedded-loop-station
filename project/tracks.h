#ifndef tracks_h_
#define tracks_h_

#include "track.h"

class Tracks {
  Track tracks[4] = {Track("0A.WAV", "0B.WAV"), Track("1A.WAV", "1B.WAV"),
                     Track("2A.WAV", "2B.WAV"), Track("3A.WAV", "3B.WAV")};
  int selectedTrackIdx = 0;

public:
  Track selected = tracks[0];
  void setSelected(int i);
  Track getTrack(int i);
  Tracks();
};

#endif
