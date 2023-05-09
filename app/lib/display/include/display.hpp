#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include <Arduino.h>
#include <ILI9341_t3.h>          // Display library
#include <XPT2046_Touchscreen.h> // Touchscreen library
#include <font_Arial.h>          // Font for display
#include <library.hpp>

#include "types.hpp"

// LCD control pins defined by board
#define TFT_CS 40
#define TFT_DC 9

// Touch screen control pins defined by board
// TIRQ interrupt if used is on pin 2
#define TS_CS 41

// Font for buttons
#define BUTTON_FONT Arial_14

enum class KnobReset { ON, OFF };

class Display {
private:
  // Location and size of screen objects
  struct Layout {
    int x;
    int y;
    int w;
    int h;
  };

  // Display items
  static const Layout touchScreen;
  static const Layout boot;
  static const Layout play;
  static const Layout stop;
  static const Layout record;
  static const Layout volume;
  static const Layout mode;
  static const Layout library;
  static const Layout trackInfo;
  static const Layout track1;
  static const Layout track2;
  static const Layout track3;
  static const Layout track4;
  static const Layout next;
  static const Layout prev;
  static const Layout panBar;
  static const Layout panDot;
  static const Layout reverse;
  static const Layout save;
  static const Layout alert;

  bool nameChange = false;
  bool reverseBool = false;
  bool isTouched = false;
  bool modeChange = false;
  bool libraryVisible = false;
  bool displayOnce = false;

  Status displayedStatus = Status::Stop;
  int displayedTrack = 0;

  String fileName; // holds name of file to be used
  String libraryName;

  int panChange = 50;
  float volumeChange;
  int index = 0;

  KnobReset knobReset;
  Mode modeObj;
  ILI9341_t3 tft = ILI9341_t3(TFT_CS, TFT_DC);
  XPT2046_Touchscreen ts = XPT2046_Touchscreen(TS_CS);
  TS_Point p;

public:
  void setModeChange(bool change) { this->modeChange = change; }
  int getModeChange() const { return this->modeChange; }

  void setNameChange(bool change) { this->nameChange = change; }
  bool getNameChange() const { return this->nameChange; }

  void setRevBool(bool change) { this->reverseBool = change; }
  bool getRevBool() const { return this->reverseBool; }

  void setFileName(String name) { this->fileName = name; }
  String getFileName() { return fileName; };

  void setup();
  void bootup();
  void mainScreen();
  void playButton(bool audio);
  void recordButton(bool audio);
  void stopButton(bool audio);
  void saveButton();
  void saveAlert();
  void reverseButton();
  void libraryButton();
  void handleReverseButton();
  void displayTrack(String name);
  void displayPosition(uint32_t position, uint32_t length);
  void displayVol();
  void handleTouch(const Library &obj);
  void displayLibrary(const Library &obj);
  void libraryTracks(int index, const Library &obj);
  void selectMode();
  void modeButton();
  void pan();
  void displayPan();
  void updateStatus(Status status);
  void getPoint();

  void readTouch();
  bool libraryClicked();
  bool saveClicked();
  bool reverseClicked();
};

#endif
