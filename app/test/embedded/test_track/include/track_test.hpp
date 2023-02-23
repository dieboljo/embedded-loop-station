#ifndef TRACK_TEST_HPP
#define TRACK_TEST_HPP

#include <track.hpp>

class TrackTest : public Track {
public:
  TrackTest(const char *f1, const char *f2, AudioInputI2S *s)
      : Track(f1, f2, s){};
  const char *getReadFileName() { return readFileName; }
  const char *getWriteFileName() { return writeFileName; }
};

#endif
