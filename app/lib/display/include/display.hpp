#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include <ILI9341_t3.h> // Display library
#include <XPT2046_Touchscreen.h> // Touchscreen library
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

// Touch screen control pins defined by board
// TIRQ interrupt if used is on pin 2
#define TS_CS  41                 // NOT needed?????


// Screen is 240x320

// Define Track info 
#define TRACK_X 10
#define TRACK_Y 10
#define TRACK_W 300
#define TRACK_H 50

// Define Stop button location and size
#define STOP_X 110
#define STOP_Y 70
#define STOP_W 70
#define STOP_H 32

// Define Record button location and size
#define RECORD_X 10
#define RECORD_Y 70
#define RECORD_W 90
#define RECORD_H 32

// Define Play button location and size
#define PLAY_X 190
#define PLAY_Y 70
#define PLAY_W 105
#define PLAY_H 32

// Define Volume location and size
#define VOLUME_X 260
#define VOLUME_Y 180
#define VOLUME_W 50
#define VOLUME_H 50

// Define Mode location and size
#define MODE_X 10
#define MODE_Y 190
#define MODE_W 90
#define MODE_H 40

// Define Library location and size
#define DISP_X 0
#define DISP_Y 0
#define DISP_W 240
#define DISP_H 360

// Define startup intro location and size
#define BOOT_X 40
#define BOOT_Y 120
#define BOOT_W 240
#define BOOT_H 360

// Define track 1 location and size
#define T1_X 70
#define T1_Y 60
#define T1_W 180
#define T1_H 32

// Define track 2 location and size
#define T2_X 70
#define T2_Y 100
#define T2_W 180
#define T2_H 32

// Define track 3 location and size
#define T3_X 70
#define T3_Y 140
#define T3_W 180
#define T3_H 32

// Define track 4 location and size
#define T4_X 70
#define T4_Y 180
#define T4_W 180
#define T4_H 32

// Define next button location and size
#define NEXT_X 260
#define NEXT_Y 100
#define NEXT_W 50
#define NEXT_H 50

// Define Prev button location and size
#define PREV_X 10
#define PREV_Y 100
#define PREV_W 50
#define PREV_H 50

// Define track selection button location and size
#define LIB_X 10 
#define LIB_Y 10
#define LIB_W 300
#define LIB_H 30

// Define Pan button location and size
#define PAN_X 10 
#define PAN_Y 110
#define PAN_W 120
#define PAN_H 30

// Define Pan dot location and size
#define DOT_X 16
#define DOT_Y 125
#define DOT_R 5

// Define Reverse button location and size
#define REVERSE_X 160 
#define REVERSE_Y 160
#define REVERSE_W 60
#define REVERSE_H 60


//Font for buttons
#define BUTTON_FONT Arial_14

class Display{
    private:
    float volumeChange;
    String * fileArray; // array of files from SD card
    boolean isTouched = false;
    String fileName; // holds name of file to be used
    int mode = 1; // 1 = overdub. 2 = replace
    int modeChange = 0;
    boolean nameChange = false;
    int panChange = 50;
    boolean reverseBool = false;
    

    public:

        void setMode(int num) {this->mode = num;}
        int getMode() const {return this->mode;}

        void setModeChange(int num) {this->modeChange = num;}
        int getModeChange() const {return this->modeChange;}

        void setNameChange(boolean change) {this->nameChange = change;}
        boolean getNameChange() const {return this->nameChange;}

        void setRevBool(boolean change) {this->reverseBool = change;}
        boolean getRevBool() const {return this->reverseBool;}

        void Setup();

        void Boot();

        void mainScreen();

        void Library();

        void SetPlayButton (boolean audio);

        void SetRecordButton (boolean audio);

        void SetStopButton (boolean audio);

        void displayTrack(String name);

        void displayVol();

        void displayMode(int modeValue, int &actualMode);

        void showLib();

        void displayLibrary();

        void setArray(String array[]);

        void selectMode();

        void setModeButton();

        void setFileName(String name) {this->fileName = name;}

        String getFileName() {return fileName;};

        void setPanBar();

        void displayPan();

        void setReverseButton();
        void reverseButton();
};


#endif