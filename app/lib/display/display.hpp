#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include <ILI9341_t3.h> // Display library
//#include <XPT2046_Touchscreen.h> // Touchscreen library
#include <Arduino.h>
#include <font_Arial.h> // Font for display

// touchscreen offset for four corners
#define TS_MINX 400
#define TS_MINY 400
#define TS_MAXX 3879
#define TS_MAXY 3843

// LCD control pins defined by board
#define TFT_CS 40
#define TFT_DC  9

// Use main SPI bus MOSI=11, MISO=12, SCK=13 with different control pins
//ILI9341_t3 tft = ILI9341_t3(TFT_CS, TFT_DC);

// Touch screen control pins defined by board
// TIRQ interrupt if used is on pin 2
//#define TS_CS  41                 // NOT needed?????
//XPT2046_Touchscreen ts(TS_CS);

// Screen is 240x320

// Define Track info 
#define TRACK_X 10 // X-axis pixel offset from left of screen
#define TRACK_Y 10
#define TRACK_W 300
#define TRACK_H 50

// Define Stop button location and size
#define STOP_X 10
#define STOP_Y 70
#define STOP_W 70
#define STOP_H 32

// Define Record button location and size
#define RECORD_X 90
#define RECORD_Y 70
#define RECORD_W 90
#define RECORD_H 32

// Define Play button location and size
#define PLAY_X 190
#define PLAY_Y 70 // 8 pixel Y-axis offset from "Track Info"
#define PLAY_W 105
#define PLAY_H 32

// Define Volume location and size
#define VOLUME_X 260
#define VOLUME_Y 180
#define VOLUME_W 50
#define VOLUME_H 50

// Define Mode location and size
#define MODE_X 10
#define MODE_Y 115
#define MODE_W 170
#define MODE_H 32

// Define Library location and size
#define DISP_X 0
#define DISP_Y 0
#define DISP_W 240
#define DISP_H 360

//Font for buttons
#define BUTTON_FONT Arial_14

class Display{
    private:
        //String * fileArray;
        //static const int SIZE = 10;
       // int size;
       // int index;

    public:
        void setup();

        void SetPlayButton (boolean audio);

        void SetRecordButton (boolean audio);

        void SetStopButton (boolean audio);

        void displayTrack(String name);

        void displayVolume(float volume);

        void displayMode(int modeValue, int &actualMode);

        //void displayLibrary(int index, String fileArray[]);
        void displayLibrary(String a);
};


#endif