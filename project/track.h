#ifndef track_h_
#define track_h_

class Track {
  const char *frames[2];
  File frame[2];
  uint64_t position;
  int readIdx;
  Project::AudioPlaySdRaw playRaw;
  AudioRecordQueue queue;
  AudioAnalyzePeak peak;

public:
  File readFrame;
  File writeFrame;
  void swap();
  Track(const char *frameName1, const char *frameName2);
};

#endif
