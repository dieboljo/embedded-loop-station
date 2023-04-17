#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include <ILI9341_t3.h> // Display library
#include <XPT2046_Touchscreen.h> // Touchscreen library
#include <Arduino.h>
#include <font_Arial.h> // Font for display
#include <library.hpp>

// touchscreen offset for four corners
#define TS_MINX 400
#define TS_MINY 400
#define TS_MAXX 3879
#define TS_MAXY 3843

// LCD control pins defined by board
#define TFT_CS 40
#define TFT_DC  9

// Touch screen control pins defined by board
// TIRQ interrupt if used is on pin 2
#define TS_CS  41

//Font for buttons
#define BUTTON_FONT Arial_14

class Display{
    private:

    // Location and size of screen objects
    struct Layout{
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

    float volumeChange;
    //String * fileArray; // array of files from SD card
    boolean isTouched = false;
    String fileName; // holds name of file to be used
    int modeStatus = 1; // 1 = overdub. 2 = replace
    int modeChange = 0;

    boolean nameChange = false;
    int panChange = 50;
    boolean reverseBool = false;

    ILI9341_t3 tft = ILI9341_t3(TFT_CS, TFT_DC);
    XPT2046_Touchscreen ts = XPT2046_Touchscreen(TS_CS);
    TS_Point p;

    public:

        void setMode(int num) {this->modeStatus = num;}
        int getMode() const {return this->modeStatus;}

        void setModeChange(int num) {this->modeChange = num;}
        int getModeChange() const {return this->modeChange;}

        void setNameChange(boolean change) {this->nameChange = change;}
        boolean getNameChange() const {return this->nameChange;}

        void setRevBool(boolean change) {this->reverseBool = change;}
        boolean getRevBool() const {return this->reverseBool;}

        void setup();
        void bootup();
        void mainScreen();
        void showLibrary();
        void setPlayButton (boolean audio);
        void setRecordButton (boolean audio);
        void setStopButton (boolean audio);
        void displayTrack(String name);
        void displayVol();
        void displayMode(int modeValue, int &actualMode);
        void showLib(const Library& obj);
        void displayLibrary(const Library& obj);
        void selectMode();
        void setModeButton();
        void setFileName(String name) {this->fileName = name;}
        void setPanBar();
        void displayPan();
        void setReverseButton();
        void reverseButton();

        void setArray(String array[]);
        String getFileName() {return fileName;};

};

#endif