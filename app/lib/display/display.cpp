#include <display.hpp>

ILI9341_t3 tft = ILI9341_t3(TFT_CS, TFT_DC);

// Display setup - call during setup
void Display::setup(){
  //Setup LCD screen
  tft.begin();
  tft.setRotation(3);
  //ts.begin();
  //ts.setRotation(1);
  tft.fillScreen(ILI9341_LIGHTGREY);
}

// Set / Display play button
void Display::SetPlayButton (boolean audio){
  tft.setCursor(PLAY_X + 8, PLAY_Y + 8);
  tft.setFont(BUTTON_FONT);
  tft.setTextColor(ILI9341_WHITE);
  
  if (!audio) {  // button is set inactive, redraw button inactive
    tft.fillRoundRect(PLAY_X, PLAY_Y, PLAY_W, PLAY_H, 4, ILI9341_BLACK);
    tft.print ("Play Audio");
  }
  else {          // button is active, redraw button active
    tft.fillRoundRect(PLAY_X, PLAY_Y, PLAY_W, PLAY_H, 4, ILI9341_GREEN);
    tft.print ("Playing");
  }
}

// Set / Display record button
void Display::SetRecordButton (boolean audio){
  tft.setCursor(RECORD_X + 8, RECORD_Y + 8);
  tft.setFont(BUTTON_FONT);
  tft.setTextColor(ILI9341_WHITE);
  
  if (!audio) {  // button is set inactive, redraw button inactive
    tft.fillRoundRect(RECORD_X, RECORD_Y, RECORD_W, RECORD_H, 4, ILI9341_BLACK);
    tft.print ("Record");
  }
  else {          // button is active, redraw button active
    tft.fillRoundRect(RECORD_X, RECORD_Y, RECORD_W, RECORD_H, 4, ILI9341_GREEN);
    tft.print ("Recording");
  }
}

// Set / Display stop button
void Display::SetStopButton (boolean audio){
  tft.setCursor(STOP_X + 8, STOP_Y + 8);
  tft.setFont(BUTTON_FONT);
  tft.setTextColor(ILI9341_WHITE);
  
  if (!audio) {  // button is set inactive, redraw button inactive
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
void Display::displayVolume(float volume){

  float value = volume/0.076;
  tft.setCursor(VOLUME_X + 12, VOLUME_Y + 8);
  tft.setFont(Arial_10);
  tft.setTextColor(ILI9341_WHITE);
  tft.fillRoundRect(VOLUME_X, VOLUME_Y, VOLUME_W, VOLUME_H, 8, ILI9341_BLACK);
  tft.println("Vol");
  tft.setCursor(VOLUME_X + 7, VOLUME_Y + 28);
  tft.print(value);
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

// Display Library
void Display::displayLibrary(String a){
    //tft.setCursor(DISP_X + 50, DISP_Y + 80);
    tft.setFont(BUTTON_FONT);
    tft.setTextColor(ILI9341_WHITE);
    tft.fillRoundRect(DISP_X, DISP_Y, DISP_W, DISP_H, 8, ILI9341_BLACK);
    Serial.println(a);
    tft.println(a);
}

/*
void Display::displayLibrary(int index, String fileArray[]){
    tft.setCursor(DISP_X + 50, DISP_Y + 16);
    tft.setFont(BUTTON_FONT);
    tft.setTextColor(ILI9341_WHITE);
    tft.fillRoundRect(DISP_X, DISP_Y, DISP_W, DISP_H, 8, ILI9341_BLACK);
    Serial.println(fileArray[index]);
    tft.println(fileArray[index]);
    return;
}
*/

