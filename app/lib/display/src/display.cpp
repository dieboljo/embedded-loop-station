#include <display.hpp>
#include <Arduino.h>
#include <SD.h>

ILI9341_t3 tft = ILI9341_t3(TFT_CS, TFT_DC);
XPT2046_Touchscreen ts(TS_CS);
TS_Point p;


// Display setup - call during setup
void Display::Setup(){
  //Setup LCD screen
  tft.begin();
  // touch screen
  ts.begin();
  tft.setRotation(3);
  tft.fillScreen(ILI9341_LIGHTGREY);
}

// Intro bootup screen
void Display::Boot(){
  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(BOOT_X, BOOT_Y);
  tft.setFont(Arial_10);
  tft.setTextColor(ILI9341_WHITE);
  //load screen
  tft.print ("Embedded Digital Audio Loop Station");
  //ts.begin();
  //ts.setRotation(1);

}

// setup the main screen
void Display::mainScreen(){
  tft.fillScreen(ILI9341_WHITE);
  //Draw buttons
  SetPlayButton (false);
  SetStopButton (false);
  SetRecordButton (false);
  setPanBar();
  setModeButton();
  setReverseButton();
  displayVol();
  String defaultTrack = "Tap for library";
  displayTrack(defaultTrack);
}

// setup reverse button
void Display::setReverseButton(){
  tft.setFont(BUTTON_FONT);
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(REVERSE_X + 10, REVERSE_Y + 10);
  tft.fillRoundRect(REVERSE_X, REVERSE_Y, REVERSE_W , REVERSE_H, 8, ILI9341_DARKGREY);
  tft.print("Rev");
  tft.setCursor(REVERSE_X + 8, REVERSE_Y + 30);
  tft.print("OFF");
}

// set the pan bar
void Display::setPanBar(){
  tft.fillRoundRect(PAN_X, PAN_Y, PAN_W , PAN_H, 8, ILI9341_DARKGREY);
  tft.fillCircle(DOT_X, DOT_Y, DOT_R, ILI9341_NAVY);
}

// set mode button
void Display::setModeButton(){
  tft.setFont(BUTTON_FONT);
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(MODE_X + 5, MODE_Y + 5);
  tft.fillRoundRect(MODE_X, MODE_Y, MODE_W, MODE_H, 8, ILI9341_BLACK);
  tft.print("Overdub");
}

// Set / Display play button
void Display::SetPlayButton (boolean play){
  tft.setCursor(PLAY_X + 8, PLAY_Y + 8);
  tft.setFont(BUTTON_FONT);
  tft.setTextColor(ILI9341_WHITE);
  
  if (!play) {  // button is set inactive, redraw button inactive
    tft.fillRoundRect(PLAY_X, PLAY_Y, PLAY_W, PLAY_H, 4, ILI9341_BLACK);
    tft.print ("Play Audio");
  }
  else {          // button is active, redraw button active
    tft.fillRoundRect(PLAY_X, PLAY_Y, PLAY_W, PLAY_H, 4, ILI9341_GREEN);
    tft.print ("Playing");
  }
}

// Set / Display record button
void Display::SetRecordButton (boolean record){
  tft.setCursor(RECORD_X + 8, RECORD_Y + 8);
  tft.setFont(BUTTON_FONT);
  tft.setTextColor(ILI9341_WHITE);
  
  if (!record) {  // button is set inactive, redraw button inactive
    tft.fillRoundRect(RECORD_X, RECORD_Y, RECORD_W, RECORD_H, 4, ILI9341_BLACK);
    tft.print ("Record");
  }
  else {          // button is active, redraw button active
    tft.fillRoundRect(RECORD_X, RECORD_Y, RECORD_W, RECORD_H, 4, ILI9341_GREEN);
    tft.print ("Recording");
  }
}

// Set / Display stop button
void Display::SetStopButton (boolean stop){
  tft.setCursor(STOP_X + 8, STOP_Y + 8);
  tft.setFont(BUTTON_FONT);
  tft.setTextColor(ILI9341_WHITE);
  
  if (!stop) {  // button is set inactive, redraw button inactive
    tft.fillRoundRect(STOP_X, STOP_Y, STOP_W, STOP_H, 4, ILI9341_RED);
    tft.print ("Stop");
  }
  else {          // button is active, redraw button active
    tft.fillRoundRect(STOP_X, STOP_Y, STOP_W, STOP_H, 4, ILI9341_BLACK);
    tft.print ("Stop");
  }
}


// Display Track name
void Display::displayTrack(String name){
  tft.setCursor(TRACK_X + 50, TRACK_Y + 16);
  tft.setFont(BUTTON_FONT);
  tft.setTextColor(ILI9341_WHITE);
  tft.fillRoundRect(TRACK_X, TRACK_Y, TRACK_W, TRACK_H, 8, ILI9341_BLACK);
  tft.print(name);
}

// Display volume
void Display::displayVol(){
  // read the knob position (analog input A1)
  int vol = map(analogRead(A1), 0, 1000, 0, 100);
  // Volume pot is very sensitive - need to set a change range
  if (volumeChange <= (vol * .95) || volumeChange >= (vol * 1.05)){
    tft.setCursor(VOLUME_X + 12, VOLUME_Y + 8);
    tft.setFont(Arial_10);
    tft.setTextColor(ILI9341_WHITE);
    tft.fillRoundRect(VOLUME_X, VOLUME_Y, VOLUME_W , VOLUME_H, 8, ILI9341_BLACK);
    tft.println("Vol");
    tft.setCursor(VOLUME_X + 15, VOLUME_Y + 28);
    tft.print(vol);
    // update volume if changed
    volumeChange = vol;
  }
}

// Display Mode -- will need to update or remove
void Display::displayMode(int modeValue, int &actualMode){
  //tft.setCursor(MODE_X + 8, MODE_Y + 8);
  tft.setFont(BUTTON_FONT);
  tft.setTextColor(ILI9341_WHITE);
  
  //Serial.println(modeValue);
  if (modeValue < 60 && actualMode != 1){
    actualMode = 1;
    Serial.println(actualMode);
    tft.setCursor(MODE_X + 8, MODE_Y + 8);
    tft.fillRoundRect(MODE_X, MODE_Y, MODE_W, MODE_H, 8, ILI9341_BLACK);
    tft.println("Record Mode");
  }
  if (modeValue > 65 && actualMode != 2){
    actualMode = 2;
    tft.setCursor(MODE_X + 8, MODE_Y + 8);
    tft.fillRoundRect(MODE_X, MODE_Y, MODE_W, MODE_H, 8, ILI9341_BLACK);
    tft.println("Library Mode");
  }
}

// copies array from library class
void Display::setArray(String array[]){
  fileArray = array;
}

// Display Library
// select from library
void Display::displayLibrary(){

  String name;
  int index = 0;


  tft.fillScreen(ILI9341_WHITE);
  tft.setFont(BUTTON_FONT);
  tft.setTextColor(ILI9341_WHITE);

  tft.setCursor(NEXT_X + 5, NEXT_Y + 5);
  tft.fillRoundRect(NEXT_X, NEXT_Y, NEXT_W, NEXT_H, 8, ILI9341_BLACK);
  tft.print("Next");

  tft.setCursor(PREV_X + 5, PREV_Y + 5);
  tft.fillRoundRect(PREV_X, PREV_Y, PREV_W, PREV_H, 8, ILI9341_BLACK);
  tft.print("Prev");

  tft.setCursor(LIB_X + 50, LIB_Y + 5);
  tft.fillRoundRect(LIB_X, LIB_Y, LIB_W, LIB_H, 8, ILI9341_BLACK);
  tft.print("Select Track - tap to exit");

  tft.fillRoundRect(T1_X, T1_Y, T1_W, T1_H, 8, ILI9341_BLACK);
  tft.setCursor(T1_X + 25, T1_Y + 8);
  tft.print(fileArray[0]);

  tft.fillRoundRect(T2_X, T2_Y, T2_W, T2_H, 8, ILI9341_BLACK);
  tft.setCursor(T2_X + 25, T2_Y + 8);
  tft.print(fileArray[1]);

  tft.fillRoundRect(T3_X, T3_Y, T3_W, T3_H, 8, ILI9341_BLACK);
  tft.setCursor(T3_X + 25, T3_Y + 8);
  tft.print(fileArray[2]);

  tft.fillRoundRect(T4_X, T4_Y, T4_W, T4_H, 8, ILI9341_BLACK);
  tft.setCursor(T4_X + 25, T4_Y + 8);
  tft.print(fileArray[3]);

  isTouched = false;

  while(true){
    if(ts.touched() && isTouched == false){
      // get touch location
      p = ts.getPoint();
      delay(100);
      p.x = map(p.x, TS_MINY, TS_MAXY, 0, tft.width());
      p.y = map(p.y, TS_MINX, TS_MAXX, 0, tft.height());

      isTouched = true;

      if ((p.x > TRACK_X) && (p.x < (TRACK_X + TRACK_W))) {
        if ((p.y > TRACK_Y) && (p.y <= (TRACK_Y + TRACK_H))) {
          isTouched = false;
          // reset to main screen
          mainScreen();
          delay(50);
          return;
        }
      }

      if ((p.x > NEXT_X) && (p.x < (NEXT_X + NEXT_W))) {
          if ((p.y > NEXT_Y) && (p.y <= (NEXT_Y + NEXT_H))) {
              Serial.println("Next pressed");

              index = index + 4;
        
              tft.fillRoundRect(T1_X, T1_Y, T1_W, T1_H, 8, ILI9341_BLACK);
              tft.setCursor(T1_X + 25, T1_Y + 8);
              tft.print(fileArray[index]);

              tft.fillRoundRect(T2_X, T2_Y, T2_W, T2_H, 8, ILI9341_BLACK);
              tft.setCursor(T2_X + 25, T2_Y + 8);
              tft.print(fileArray[index + 1]);

              tft.fillRoundRect(T3_X, T3_Y, T3_W, T3_H, 8, ILI9341_BLACK);
              tft.setCursor(T3_X + 25, T3_Y + 8);
              tft.print(fileArray[index + 2]);

              tft.fillRoundRect(T4_X, T4_Y, T4_W, T4_H, 8, ILI9341_BLACK);
              tft.setCursor(T4_X + 25, T4_Y + 8);
              tft.print(fileArray[index + 3]);              
          }
      }

      if ((p.x > PREV_X) && (p.x < (PREV_X + PREV_W))) {
          if ((p.y > PREV_Y) && (p.y <= (PREV_Y + PREV_H))) {
            Serial.println("Prev pressed");

            index = index - 4;
          
            if(index < 0){
              index = 0;
            }

            tft.fillRoundRect(T1_X, T1_Y, T1_W, T1_H, 8, ILI9341_BLACK);
            tft.setCursor(T1_X + 25, T1_Y + 8);
            tft.print(fileArray[index]);

            tft.fillRoundRect(T2_X, T2_Y, T2_W, T2_H, 8, ILI9341_BLACK);
            tft.setCursor(T2_X + 25, T2_Y + 8);
            tft.print(fileArray[index + 1]);

            tft.fillRoundRect(T3_X, T3_Y, T3_W, T3_H, 8, ILI9341_BLACK);
            tft.setCursor(T3_X + 25, T3_Y + 8);
            tft.print(fileArray[index + 2]);

            tft.fillRoundRect(T4_X, T4_Y, T4_W, T4_H, 8, ILI9341_BLACK);
            tft.setCursor(T4_X + 25, T4_Y + 8);
            tft.print(fileArray[index + 3]);
          }
      }
    }
  

    if (!ts.touched() && isTouched) {
       // touchscreen is no longer being touched, reset flag
      isTouched = false; 
    }

      p = ts.getPoint();
      delay(100);
      p.x = map(p.x, TS_MINY, TS_MAXY, 0, tft.width());
      p.y = map(p.y, TS_MINX, TS_MAXX, 0, tft.height());

      if ((p.x > T1_X) && (p.x < (T1_X + T1_W))) {
          if ((p.y > T1_Y) && (p.y <= (T1_Y + T1_H))) {
            Serial.println("track one selected");
            name = fileArray[index];
            break;
          }
      }
      if ((p.x > T2_X) && (p.x < (T2_X + T2_W))) {
        if ((p.y > T2_Y) && (p.y <= (T2_Y + T2_H))) {
          Serial.println("track two selected");
          name = fileArray[index + 1];
          break;
        }
      }
      if ((p.x > T3_X) && (p.x < (T3_X + T3_W))) {
        if ((p.y > T3_Y) && (p.y <= (T3_Y + T3_H))) {
          Serial.println("track three selected");
          name = fileArray[index + 2];
          break;
        }
      }
      if ((p.x > T4_X) && (p.x < (T4_X + T4_W))) {
        if ((p.y > T4_Y) && (p.y <= (T4_Y + T4_H))) {
          Serial.println("track four selected");
          name = fileArray[index + 3];
          break;
        }
      }
    }

  // reset to main screen
  mainScreen();

  
  // sets new file name to be used
  setFileName(name);
  // sets name change flag
  setNameChange(true);
}


// Reacts to touch
void Display::showLib(){
  if(ts.touched() && isTouched == false){
    delay(100);

    p = ts.getPoint();
    p.x = map(p.x, TS_MINY, TS_MAXY, 0, tft.width());
    p.y = map(p.y, TS_MINX, TS_MAXX, 0, tft.height());
    isTouched = true;

    // select mode
    if ((p.x > MODE_X) && (p.x < (MODE_X + MODE_W))) {
        if ((p.y > MODE_Y) && (p.y <= (MODE_Y + MODE_H))) {
          selectMode(); 
        }
    }

    // display library
    if ((p.x > TRACK_X) && (p.x < (TRACK_X + TRACK_W))) {
        if ((p.y > TRACK_Y) && (p.y <= (TRACK_Y + TRACK_H))) {
          displayLibrary(); 
        }
    }

    // Reverse on / off
    if ((p.x > REVERSE_X) && (p.x < (REVERSE_X + REVERSE_W))) {
        if ((p.y > REVERSE_Y) && (p.y <= (REVERSE_Y + REVERSE_H))) {
          reverseButton(); 
        }
    }

  }

   if (!ts.touched() && isTouched) {
    isTouched = false;  // touchscreen is no longer being touched, reset flag
  }

}


// Displays the mode - Overdub or Replace
// Sets mode changed flags
void Display::selectMode(){

  tft.setFont(BUTTON_FONT);
  tft.setTextColor(ILI9341_WHITE);

  isTouched = false;

  if(ts.touched() && isTouched == false){
    Serial.println("mode pressed");
    Serial.println(mode);

    p = ts.getPoint();
    delay(100);
    p.x = map(p.x, TS_MINY, TS_MAXY, 0, tft.width());
    p.y = map(p.y, TS_MINX, TS_MAXX, 0, tft.height());

    if ((p.x > MODE_X) && (p.x < (MODE_X + MODE_W))) {
      if ((p.y > MODE_Y) && (p.y <= (MODE_Y + MODE_H))) {
        if(getMode() == 2 && !isTouched){
          tft.setCursor(MODE_X + 5, MODE_Y + 10);
          tft.fillRoundRect(MODE_X, MODE_Y, MODE_W, MODE_H, 8, ILI9341_BLACK);
          tft.print("Replace");
          this->mode = 1;
          this->modeChange = 1;
          isTouched = true;
        }

        if(getMode() == 1 && !isTouched){
          tft.setCursor(MODE_X + 5, MODE_Y + 10);
          tft.fillRoundRect(MODE_X, MODE_Y, MODE_W, MODE_H, 8, ILI9341_BLACK);
          tft.print("Overdub");
          this->mode = 2;
          this->modeChange = 1;
          isTouched = true;
        }
      }
    }  
  }
}


// displays pan
// pan is very sensitive and "quick" when below a lower threshold of half/output
// below this threshold
// ---- Need to adjust for a smoother display experience
void Display::displayPan(){
  int16_t pan = map(analogRead(A2), 0, 1000, 0, 100);

  if (panChange < (pan - 5) || panChange > (pan + 1)){
    
    tft.setCursor(PAN_X + 40, PAN_Y +10);
    tft.setFont(Arial_14);
    tft.setTextColor(ILI9341_BLACK);
    tft.fillRoundRect(PAN_X, PAN_Y, PAN_W , PAN_H, 8, ILI9341_DARKGREY);
    tft.print("PAN");
    tft.fillCircle(DOT_X + pan, DOT_Y, DOT_R, ILI9341_NAVY);
    panChange = pan;
  }
}


// displays the reverse button.
// Sets flag for reverse
void Display::reverseButton(){

  isTouched = false;

  if(ts.touched() && isTouched == false){
    delay(100);

    p = ts.getPoint();
    p.x = map(p.x, TS_MINY, TS_MAXY, 0, tft.width());
    p.y = map(p.y, TS_MINX, TS_MAXX, 0, tft.height());
  
   if ((p.x > REVERSE_X) && (p.x < (REVERSE_X + REVERSE_W))) {
        if ((p.y > REVERSE_Y) && (p.y <= (REVERSE_Y + REVERSE_H))) {
          isTouched = true;
          if(!reverseBool){
            tft.setFont(BUTTON_FONT);
            tft.setTextColor(ILI9341_BLACK);
            tft.setCursor(REVERSE_X + 10, REVERSE_Y + 10);
            tft.fillRoundRect(REVERSE_X, REVERSE_Y, REVERSE_W , REVERSE_H, 8, ILI9341_RED);
            tft.print("Rev");
            tft.setCursor(REVERSE_X + 13, REVERSE_Y + 30);
            tft.print("ON");
            setRevBool(true);
            return;
          }
          if(reverseBool){
            tft.setFont(BUTTON_FONT);
            tft.setTextColor(ILI9341_BLACK);
            tft.setCursor(REVERSE_X + 10, REVERSE_Y + 10);
            tft.fillRoundRect(REVERSE_X, REVERSE_Y, REVERSE_W , REVERSE_H, 8, ILI9341_LIGHTGREY);
            tft.print("Rev");
            tft.setCursor(REVERSE_X + 8, REVERSE_Y + 30);
            tft.print("OFF");
            setRevBool(false);
          } 
      }
    }
  }
}