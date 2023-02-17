#ifndef TRACK_TEST_HPP
#define TRACK_TEST_HPP

#include <track.hpp>

class TrackTest : public Track {
public:
  TrackTest(const char *f1, const char *f2, AudioInputI2S *s)
      : Track(f1, f2, s){};
  const char *getFileName1() { return fileName1; }
  const char *getFileName2() { return fileName2; }
};

#endif
