#ifndef project_play_sd_raw_h
#define project_play_sd_raw_h

#include <Arduino.h>
#include <AudioStream.h>
#include <SD.h>

namespace Project {
class AudioPlaySdRaw : public AudioStream {
public:
  AudioPlaySdRaw(void) : AudioStream(0, NULL) { begin(); }
  bool play(const char *filename, uint32_t offset);
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
}; // namespace Project

#endif
