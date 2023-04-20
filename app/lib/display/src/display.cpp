#include <display.hpp>
#include <Arduino.h>
#include <SD.h>

// touchscreen offset for four corners
const Display::Layout Display::touchScreen = {400, 400, 3879, 3843};

// Display item location values - Screen is 240x320
const Display::Layout Display::boot = {40, 120, 240, 360};
const Display::Layout Display::play = {190, 70, 105, 32};
const Display::Layout Display::stop = {110, 70, 70, 32};
const Display::Layout Display::record = {10, 70, 90, 32};
const Display::Layout Display::volume = {260, 180, 50, 50};
const Display::Layout Display::mode = {10, 190, 90, 40};
const Display::Layout Display::library = {10, 10, 300, 30};
const Display::Layout Display::trackInfo = {10, 10, 300, 50};
const Display::Layout Display::track1 = {70, 60, 180, 32};
const Display::Layout Display::track2 = {70, 100, 180, 32};
const Display::Layout Display::track3 = {70, 140, 180, 32};
const Display::Layout Display::track4 = {70, 180, 180, 32};
const Display::Layout Display::next = {260, 100, 50, 50};
const Display::Layout Display::prev = {10, 100, 50, 50};
const Display::Layout Display::reverse = {160, 160, 60, 60};
const Display::Layout Display::panBar = {10, 110, 120, 30};
const Display::Layout Display::panDot = {16, 125, 5, 0};

// Display setup - call during setup
void Display::setup(){
  //Setup LCD screen
  tft.begin();
  // touch screen
  ts.begin();
  tft.setRotation(3);
  tft.fillScreen(ILI9341_LIGHTGREY);
  bootup();
}

// Intro bootup screen
void Display::bootup(){
  tft.fillScreen(ILI9341_BLACK);
  //tft.setCursor(BOOT_X, BOOT_Y);
  tft.setCursor(boot.x, boot.y);
  tft.setFont(Arial_10);
  tft.setTextColor(ILI9341_WHITE);
  //load screen
  tft.print ("Embedded Digital Audio Loop Station");
  delay(3000);
  mainScreen();
}

// setup the main screen
void Display::mainScreen(){
  tft.fillScreen(ILI9341_WHITE);
  //Draw buttons
  setPlayButton (false);
  setStopButton (false);
  setRecordButton (false);
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
  tft.setCursor(reverse.x + 10, reverse.y + 10);
  tft.fillRoundRect(reverse.x, reverse.y, reverse.w, reverse.h, 8, ILI9341_DARKGREY);
  tft.print("Rev");
  tft.setCursor(reverse.x + 8, reverse.y + 30);
  tft.print("OFF");
}

// set the pan bar
void Display::setPanBar(){
  tft.fillRoundRect(panBar.x, panBar.y, panBar.w , panBar.h, 8, ILI9341_DARKGREY);
  tft.fillCircle(panDot.x, panDot.y, panDot.w, ILI9341_NAVY);
}

// set mode button
void Display::setModeButton(){
  tft.setFont(BUTTON_FONT);
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(mode.x + 5, mode.y + 10);
  tft.fillRoundRect(mode.x, mode.y, mode.w, mode.h, 8, ILI9341_BLACK);
  tft.print("Overdub");
}

// Set / Display play button
void Display::setPlayButton (boolean playStatus){
  tft.setCursor(play.x + 8, play.y + 8);
  tft.setFont(BUTTON_FONT);
  tft.setTextColor(ILI9341_WHITE);
  
  if (!playStatus) {  // button is set inactive, redraw button inactive
    tft.fillRoundRect(play.x, play.y, play.w, play.h, 4, ILI9341_BLACK);
    tft.print ("Play Audio");
  }
  else {          // button is active, redraw button active
    tft.fillRoundRect(play.x, play.y, play.w, play.h, 4, ILI9341_GREEN);
    tft.print ("Playing");
  }
}

// Set / Display record button
void Display::setRecordButton (boolean recordStatus){
  tft.setCursor(record.x + 8, record.y + 8);
  tft.setFont(BUTTON_FONT);
  tft.setTextColor(ILI9341_WHITE);
  
  if (!recordStatus) {  // button is set inactive, redraw button inactive
    tft.fillRoundRect(record.x, record.y, record.w, record.h, 4, ILI9341_BLACK);
    tft.print ("Record");
  }
  else {          // button is active, redraw button active
    tft.fillRoundRect(record.x, record.y, record.w, record.h, 4, ILI9341_GREEN);
    tft.print ("Recording");
  }
}

// Set / Display stop button
void Display::setStopButton (boolean stopStatus){
  tft.setCursor(stop.x + 8, stop.y + 8);
  tft.setFont(BUTTON_FONT);
  tft.setTextColor(ILI9341_WHITE);
  
  if (!stopStatus) {  // button is set inactive, redraw button inactive
    tft.fillRoundRect(stop.x, stop.y, stop.w, stop.h, 4, ILI9341_RED);
    tft.print ("Stop");
  }
  else {          // button is active, redraw button active
    tft.fillRoundRect(stop.x, stop.y, stop.w, stop.h, 4, ILI9341_BLACK);
    tft.print ("Stop");
  }
}


// Display Track name
void Display::displayTrack(String name){
  tft.setCursor(trackInfo.x + 50, trackInfo.y + 16);
  tft.setFont(BUTTON_FONT);
  tft.setTextColor(ILI9341_WHITE);
  tft.fillRoundRect(trackInfo.x, trackInfo.y, trackInfo.w, trackInfo.h, 8, ILI9341_BLACK);
  tft.print(name);
}

// Display volume
void Display::displayVol(){
  // read the knob position (analog input A1)
  int vol = map(analogRead(A1), 0, 1000, 0, 100);
  // Volume pot is very sensitive - need to set a change range
  if (volumeChange <= (vol * .95) || volumeChange >= (vol * 1.05)){
    tft.setCursor(volume.x + 12, volume.y + 8);
    tft.setFont(Arial_10);
    tft.setTextColor(ILI9341_WHITE);
    tft.fillRoundRect(volume.x, volume.y, volume.w , volume.h, 8, ILI9341_BLACK);
    tft.println("Vol");
    tft.setCursor(volume.x + 15, volume.y + 28);
    tft.print(vol);
    // update volume if changed
    volumeChange = vol;
  }
}


// Display Library - select from library
void Display::displayLibrary(const Library& obj){

  String name;
  int index = 0;

  tft.fillScreen(ILI9341_WHITE);
  tft.setFont(BUTTON_FONT);
  tft.setTextColor(ILI9341_WHITE);

  tft.setCursor(next.x + 5, next.y + 5);
  tft.fillRoundRect(next.x , next.y, next.w, next.h, 8, ILI9341_BLACK);
  tft.print("Next");

  tft.setCursor(prev.x + 5, prev.y + 5);
  tft.fillRoundRect(prev.x, prev.y, prev.w, prev.h, 8, ILI9341_BLACK);
  tft.print("Prev");

  tft.setCursor(library.x + 50, library.y + 5);
  tft.fillRoundRect(library.x, library.y, library.w, library.h, 8, ILI9341_BLACK);
  tft.print("Select Track - tap to exit");

  libraryTracks(0, obj);

  isTouched = false;

  while(true){
    if(ts.touched() && isTouched == false){
      // get touch location
      p = ts.getPoint();
      delay(100);
      p.x = map(p.x, touchScreen.y, touchScreen.h, 0, tft.width());
      p.y = map(p.y, touchScreen.x, touchScreen.w, 0, tft.height());

      isTouched = true;

      if ((p.x > trackInfo.x) && (p.x < (trackInfo.x + trackInfo.w))) {
        if ((p.y > trackInfo.y) && (p.y <= (trackInfo.y + trackInfo.h))) {
          isTouched = false;
          // reset to main screen
          mainScreen();
          delay(50);
          return;
        }
      }

      if ((p.x > next.x) && (p.x < (next.x + next.w))) {
          if ((p.y > next.y) && (p.y <= (next.y + next.h))) {

              Serial.println("Next pressed");
              if(index +4 > obj.size - 1){
                // do nothing
              }else{
                index = index + 4;
              }
        
              libraryTracks(index, obj);             
          }
      }

      if ((p.x > prev.x) && (p.x < (prev.x + prev.w))) {
          if ((p.y > prev.y) && (p.y <= (prev.y + prev.h))) {

            Serial.println("Prev pressed");
            index = index - 4;

            if(index < 0){
              index = 0;
            }

            libraryTracks(index, obj);
          }
      }
    }
  

    if (!ts.touched() && isTouched) {
       // touchscreen is no longer being touched, reset flag
      isTouched = false; 
    }

      p = ts.getPoint();
      delay(100);
      p.x = map(p.x, touchScreen.y, touchScreen.h, 0, tft.width());
      p.y = map(p.y, touchScreen.x, touchScreen.w, 0, tft.height());
      
      if ((p.x > track1.x) && (p.x < (track1.x + track1.w))) {
          if ((p.y > track1.y) && (p.y <= (track1.y + track1.h))) {
            Serial.println("track one selected");
            name = obj.fileArray[index];
            break;
          }
      }
      if ((p.x > track2.x) && (p.x < (track2.x + track2.w))) {
        if ((p.y > track2.y) && (p.y <= (track2.y + track2.h))) {
          Serial.println("track two selected");
          name = obj.fileArray[index + 1];
          break;
        }
      }
      if ((p.x > track3.x) && (p.x < (track3.x + track3.w))) {
        if ((p.y > track3.y) && (p.y <= (track3.y + track3.h))) {
          Serial.println("track three selected");
          name = obj.fileArray[index + 2];
          break;
        }
      }
      if ((p.x > track4.x) && (p.x < (track4.x + track4.w))) {
        if ((p.y > track4.y) && (p.y <= (track4.y + track4.h))) {
          Serial.println("track four selected");
          name = obj.fileArray[index + 3];
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

// Helper for displayLibrary
void Display::libraryTracks(int index, const Library& obj){

  tft.fillRoundRect(track1.x, track1.y, track1.w, track1.h, 8, ILI9341_BLACK);
  tft.setCursor(track1.x + 25, track1.y + 8);
  tft.print(obj.fileArray[index]);

  tft.fillRoundRect(track2.x, track2.y, track2.w, track2.h, 8, ILI9341_BLACK);
  tft.setCursor(track2.x + 25, track2.y + 8);
  tft.print(obj.fileArray[index + 1]);

  tft.fillRoundRect(track3.x, track3.y, track3.w, track3.h, 8, ILI9341_BLACK);
  tft.setCursor(track3.x + 25, track3.y + 8);
  tft.print(obj.fileArray[index + 2]);

  tft.fillRoundRect(track4.x, track4.y, track4.w, track4.h, 8, ILI9341_BLACK);
  tft.setCursor(track4.x + 25, track4.y + 8);
  tft.print(obj.fileArray[index + 3]);     

}

// Reacts to touch
void Display::showLib(const Library& obj){
  if(ts.touched() && isTouched == false){

    delay(100);
    p = ts.getPoint();
    p.x = map(p.x, touchScreen.y, touchScreen.h, 0, tft.width());
    p.y = map(p.y, touchScreen.x, touchScreen.w, 0, tft.height());
    isTouched = true;

    // select mode
    if ((p.x > mode.x) && (p.x < (mode.x + mode.w))) {
        if ((p.y > mode.y) && (p.y <= (mode.y + mode.h))) {
          selectMode(); 
        }
    }
    // display library
    if ((p.x > trackInfo.x) && (p.x < (trackInfo.x + trackInfo.w))) {
        if ((p.y > trackInfo.y) && (p.y <= (trackInfo.y + trackInfo.h))) {
          displayLibrary(obj); 
        }
    }
    // Reverse on / off
    if ((p.x > reverse.x) && (p.x < (reverse.x + reverse.w))) {
        if ((p.y > reverse.y) && (p.y <= (reverse.y + reverse.h))) {
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
    Serial.println(modeStatus);

    p = ts.getPoint();
    delay(100);
    p.x = map(p.x, touchScreen.y, touchScreen.h, 0, tft.width());
    p.y = map(p.y, touchScreen.x, touchScreen.w, 0, tft.height());

    if ((p.x > mode.x) && (p.x < (mode.x + mode.w))) {
      if ((p.y > mode.y) && (p.y <= (mode.y + mode.h))) {
        if(getMode() == 2 && !isTouched){
          tft.setCursor(mode.x + 5, mode.y + 10);
          tft.fillRoundRect(mode.x, mode.y, mode.w, mode.h, 8, ILI9341_BLACK);
          tft.print("Replace");
          this->modeStatus = 1;
          this->modeChange = 1;
          isTouched = true;
        }
        if(getMode() == 1 && !isTouched){
          tft.setCursor(mode.x + 5, mode.y + 10);
          tft.fillRoundRect(mode.x, mode.y, mode.w, mode.h, 8, ILI9341_BLACK);
          tft.print("Overdub");
          this->modeStatus = 2;
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
    
    tft.setCursor(panBar.x + 40, panBar.y +10);
    tft.setFont(Arial_14);
    tft.setTextColor(ILI9341_BLACK);
    tft.fillRoundRect(panBar.x, panBar.y, panBar.w , panBar.h, 8, ILI9341_DARKGREY);
    tft.print("PAN");
    tft.fillCircle(panDot.x + pan, panDot.y, panDot.w, ILI9341_NAVY);
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
    p.x = map(p.x, touchScreen.y, touchScreen.h, 0, tft.width());
    p.y = map(p.y, touchScreen.x, touchScreen.w, 0, tft.height());
  
    if ((p.x > reverse.x) && (p.x < (reverse.x + reverse.w))) {
      if ((p.y > reverse.y) && (p.y <= (reverse.y + reverse.h))) {
        tft.setFont(BUTTON_FONT);
        tft.setTextColor(ILI9341_BLACK);
        tft.setCursor(reverse.x + 10, reverse.y + 10);
        if(!reverseBool && !isTouched){
          tft.fillRoundRect(reverse.x, reverse.y, reverse.w , reverse.h, 8, ILI9341_RED);
          tft.print("Rev");
          tft.setCursor(reverse.x + 13, reverse.y + 30);
          tft.print("ON");
          setRevBool(true);
          isTouched = true;
        }
        if(reverseBool && !isTouched){
          tft.fillRoundRect(reverse.x, reverse.y, reverse.w , reverse.h, 8, ILI9341_LIGHTGREY);
          tft.print("Rev");
          tft.setCursor(reverse.x + 8, reverse.y + 30);
          tft.print("OFF");
          setRevBool(false);
          isTouched = true;
        }
      }
    }
  }
}

void Display::updateStatus(bool record, bool stop, bool play){
  setRecordButton(record);
  setStopButton(stop);
  setPlayButton(play);
}