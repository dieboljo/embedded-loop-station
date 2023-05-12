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

  enum class Screen { Main, Library };

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
  static const Layout nextTrack;
  static const Layout prev;
  static const Layout panBar;
  static const Layout panDot;
  static const Layout reverse;
  static const Layout save;
  static const Layout alert;

  static const int numLibEntries = 4;
  const Layout *libraryEntries[numLibEntries] = {&track1, &track2, &track3,
                                                 &track4};

  bool reverseBool = false;
  bool isTouched = false;
  bool redraw = false;

  Library &lib;
  int libPage = 0;
  const char *selectedLibEntry = "";

  AppState state = {
      0., 0, false, Mode::Overdub, 0., 0, false, Status::Stop, 0, 0.,
  };
  Screen screen = Screen::Main;

  ILI9341_t3 tft = ILI9341_t3(TFT_CS, TFT_DC);
  XPT2046_Touchscreen ts = XPT2046_Touchscreen(TS_CS);
  TS_Point p;

  void clearScreen();
  void getPoint();
  bool clickedLibraryNav();
  bool clickedMainNav();
  bool clickedNext();
  bool clickedPrevious();
  void drawLibraryNavButton();
  void drawLibraryEntries(bool l);
  void drawMainNavButton();
  void drawModeButton(Mode m);
  void drawNextButton();
  void drawNextTrackButton();
  void drawPan(float p);
  void drawPosition(uint32_t position, uint32_t length);
  void drawPreviousButton();
  void drawSaveButton(bool s);
  void drawStatus(Status status);
  void drawTrackName(int track);
  void drawVolume(float v);
  void playButton(bool audio);
  void recordButton(bool audio);
  void stopButton(bool audio);

public:
  Display(Library &lib) : lib(lib){};

  void reverseButton();
  // void handleReverseButton();

  void setup();
  void bootup();
  bool clickedLibraryEntry();
  bool clickedMode();
  bool clickedNextTrack();
  bool clickedSave();
  bool clickedReverse();
  const char *getSelectedEntry() { return selectedLibEntry; };
  void readTouch();
  void showLibraryScreen();
  void showMainScreen();
  void update(AppState newState);
};

#endif
