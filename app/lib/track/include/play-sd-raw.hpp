#ifndef PROJECT_PLAY_SD_RAW_HPP
#define PROJECT_PLAY_SD_RAW_HPP

#include <Audio.h>

namespace App {
class AudioPlaySdRaw : public AudioStream {
public:
  AudioPlaySdRaw(void) : AudioStream(0, NULL) { begin(); }
  bool play(const char *filename, uint32_t offset = 0);
  void begin(void);
  void stop(void);
  bool isPlaying(void) { return playing; }
  uint32_t positionMillis(void);
  uint32_t getOffset(void);
  uint32_t lengthMillis(void);
  virtual void update(void);

private:
  File rawfile;
  uint32_t file_size;
  volatile uint32_t file_offset;
  volatile bool playing;
};
} // namespace App

#endif
