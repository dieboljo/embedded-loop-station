#ifndef TRACK_CONTROLLER_HPP
#define TRACK_CONTROLLER_HPP

#include "track.hpp"

class TrackController {
  struct Gain {
    float replace;
    float overdub;
    float play;
  };

  AudioMixer4 recMixLeft;
  AudioMixer4 recMixRight;
  AudioRecordWAVstereo recording;

#ifdef USE_USB_INPUT
  AudioInputUSB &source;
#else
  AudioInputI2S &source;
#endif
  AudioConnection recMixLeftToRecording;
  AudioConnection recMixRightToRecording;

  static const AudioBuffer::bufType bufferLocation;
  static const size_t recordBufferSize;
  static const Gain gain;
  static const int numTracks = 4;
  static const int fileNameSize = 20;
  static size_t controllerId;

  File recordingFile;

  bool isRecording = false;
  uint32_t loopLength = 0;
  elapsedMillis ms;
  int selectedTrack = 0;

  char recFileNames[2 * fileNameSize];
  char *readFileName;
  char *writeFileName;

  char trackFileNames[2 * numTracks * fileNameSize];
  Track *tracks[numTracks];
  AudioConnection *trackConnections[numTracks * 4];
  float panPos[numTracks];
  float fadePos[numTracks];

  void adjustOutput(Mode mode);
  void adjustOutput();
  void createTrack(int trackNumber);
  static float panLeft(float gain, float pos);
  static float panRight(float gain, float pos);
  void printStatus(Status status);
  bool start();
  bool swapBuffers();

public:
#ifdef USE_USB_INPUT
  TrackController(AudioInputUSB &s);
#else
  TrackController(AudioInputI2S &s);
#endif

  AudioMixer4 outMixLeft;
  AudioMixer4 outMixRight;

  bool begin();
  Status checkTracks(Status status);
  void clearTrack();
  void establishLoop();
  void fade(float fadePos, Mode mode);
  uint32_t getLength() { return loopLength ? loopLength : getPosition(); }
  uint32_t getPosition() { return recording.positionMillis(); }
  int getNumTracks() { return numTracks; };
  int getSelectedTrack() { return selectedTrack; };
  int nextTrack();
  void pan(float panPos, Mode mode);
  bool play();
  bool pause();
  void punchIn(Mode mode);
  void punchOut();
  void punchOut(bool cancel);
  bool record(Mode mode);
  void save();
  bool startPlaying();
  bool startRecording(Mode mode);
  bool stop(bool cancel = false);
};

#endif
