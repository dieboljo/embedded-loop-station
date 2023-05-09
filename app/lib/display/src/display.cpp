#include <Arduino.h>
#include <SD.h>
#include <display.hpp>

// touchscreen offset for four corners
const Display::Layout Display::touchScreen = {400, 400, 3879, 3843};

// Display item location values - Screen is 320x240
const Display::Layout Display::boot = {40, 120, 240, 360};
const Display::Layout Display::play = {190, 70, 105, 32};
const Display::Layout Display::stop = {110, 70, 70, 32};
const Display::Layout Display::record = {10, 70, 90, 32};
const Display::Layout Display::volume = {260, 185, 50, 50};
const Display::Layout Display::mode = {10, 195, 90, 40};
const Display::Layout Display::reverse = {10, 148, 90, 40};
const Display::Layout Display::library = {140, 110, 170, 40};
const Display::Layout Display::trackInfo = {10, 10, 300, 50};
const Display::Layout Display::track1 = {70, 70, 180, 32};
const Display::Layout Display::track2 = {70, 110, 180, 32};
const Display::Layout Display::track3 = {70, 150, 180, 32};
const Display::Layout Display::track4 = {70, 190, 180, 32};
const Display::Layout Display::next = {260, 100, 50, 50};
const Display::Layout Display::prev = {10, 100, 50, 50};
const Display::Layout Display::panBar = {10, 110, 120, 30};
const Display::Layout Display::panDot = {16, 125, 5, 0};
const Display::Layout Display::save = {110, 195, 90, 40};
const Display::Layout Display::alert = {60, 110, 200, 50};

// Get touch screen location
void Display::getPoint() {
  p = ts.getPoint();
  p.x = map(p.x, touchScreen.y, touchScreen.h, 0, tft.width());
  p.y = map(p.y, touchScreen.x, touchScreen.w, 0, tft.height());
}

// Display setup - call during setup
void Display::setup() {
  tft.begin();
  ts.begin();
  tft.setRotation(3);
  tft.fillScreen(ILI9341_LIGHTGREY);
  modeObj = Mode::Overdub;
  bootup();
}

// Intro bootup screen
void Display::bootup() {
  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(boot.x, boot.y);
  tft.setFont(Arial_10);
  tft.setTextColor(ILI9341_WHITE);
  tft.print("Embedded Digital Audio Loop Station");
  delay(3000);
  mainScreen();
}

// setup the main screen
void Display::mainScreen() {

  for (int y = 0; y < 240; y++) {
    uint8_t r = map(y, 0, 240, 255, 0);
    uint8_t b = map(y, 0, 240, 0, 255);
    uint16_t color = tft.color565(r, 0, b);
    tft.fillRect(0, y, 320, 1, color);
  }

  // Draw buttons
  playButton(false);
  stopButton(false);
  recordButton(false);
  displayPan();
  displayVol();
  modeButton();
  reverseButton();
  saveButton();
  libraryButton();
  String defaultTrack = "Audio Loop Station";
  displayTrack(defaultTrack);
}

// setup reverse button
void Display::reverseButton() {
  tft.setFont(BUTTON_FONT);
  tft.setTextColor(ILI9341_BLACK);
  tft.setCursor(reverse.x + 7, reverse.y + 10);

  if (reverseBool) {
    tft.fillRoundRect(reverse.x, reverse.y, reverse.w, reverse.h, 8,
                      ILI9341_RED);
    tft.print("Rev ON");

  } else {
    tft.fillRoundRect(reverse.x, reverse.y, reverse.w, reverse.h, 8,
                      ILI9341_DARKGREY);
    tft.print("Rev OFF");
  }
}

void Display::saveButton() {
  tft.setFont(BUTTON_FONT);
  tft.setTextColor(ILI9341_BLACK);
  tft.setCursor(save.x + 22, save.y + 12);
  tft.fillRoundRect(save.x, save.y, save.w, save.h, 8, ILI9341_DARKGREY);
  tft.print("Save");
}

void Display::saveAlert() {
  tft.setFont(BUTTON_FONT);
  tft.setTextColor(ILI9341_WHITE);
  tft.fillRoundRect(alert.x - 5, alert.y - 5, alert.w + 10, alert.h + 10, 8,
                    ILI9341_WHITE);
  tft.setCursor(alert.x + 22, alert.y + 12);
  tft.fillRoundRect(alert.x, alert.y, alert.w, alert.h, 8, ILI9341_BLACK);
  tft.print("Recoring Saved!");
  isTouched = false;
  knobReset = KnobReset::ON;
  delay(800);
}

// set the pan bar
void Display::pan() {
  tft.fillRoundRect(panBar.x, panBar.y, panBar.w, panBar.h, 8,
                    ILI9341_DARKGREY);
  tft.fillCircle(panDot.x, panDot.y, panDot.w, ILI9341_NAVY);
}

// set mode button
void Display::modeButton() {
  tft.setFont(BUTTON_FONT);
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(mode.x + 5, mode.y + 10);
  tft.fillRoundRect(mode.x, mode.y, mode.w, mode.h, 8, ILI9341_BLACK);
  if (modeObj == Mode::Overdub) {
    tft.print("Overdub");
  }
  if (modeObj == Mode::Replace) {
    tft.print("Replace");
  }
}

// Set / Display play button
void Display::playButton(bool playStatus) {
  tft.setCursor(play.x + 8, play.y + 8);
  tft.setFont(BUTTON_FONT);
  tft.setTextColor(ILI9341_WHITE);

  if (!playStatus) { // button is set inactive, redraw button inactive
    tft.fillRoundRect(play.x, play.y, play.w, play.h, 4, ILI9341_BLACK);
    tft.print("Play Audio");
  } else { // button is active, redraw button active
    tft.fillRoundRect(play.x, play.y, play.w, play.h, 4, ILI9341_GREEN);
    tft.print("Playing");
  }
}

// Set / Display record button
void Display::recordButton(bool recordStatus) {
  tft.setCursor(record.x + 8, record.y + 8);
  tft.setFont(BUTTON_FONT);
  tft.setTextColor(ILI9341_WHITE);

  if (!recordStatus) { // button is set inactive, redraw button inactive
    tft.fillRoundRect(record.x, record.y, record.w, record.h, 4, ILI9341_BLACK);
    tft.print("Record");
  } else { // button is active, redraw button active
    tft.fillRoundRect(record.x, record.y, record.w, record.h, 4, ILI9341_GREEN);
    tft.print("Recording");
  }
}

// Set / Display stop button
void Display::stopButton(bool stopStatus) {
  tft.setCursor(stop.x + 8, stop.y + 8);
  tft.setFont(BUTTON_FONT);
  tft.setTextColor(ILI9341_WHITE);

  if (!stopStatus) { // button is set inactive, redraw button inactive
    tft.fillRoundRect(stop.x, stop.y, stop.w, stop.h, 4, ILI9341_RED);
    tft.print("Stop");
  } else { // button is active, redraw button active
    tft.fillRoundRect(stop.x, stop.y, stop.w, stop.h, 4, ILI9341_BLACK);
    tft.print("Stop");
  }
}

void Display::libraryButton() {
  tft.setCursor(library.x + 50, library.y + 10);
  tft.setFont(BUTTON_FONT);
  tft.setTextColor(ILI9341_WHITE);
  tft.fillRoundRect(library.x, library.y, library.w, library.h, 4,
                    ILI9341_BLACK);
  tft.print("Library");
}

// Display Track name
void Display::displayTrack(String name) {
  if (!libraryVisible) {
    tft.setCursor(trackInfo.x + 50, trackInfo.y + 16);
    tft.setFont(BUTTON_FONT);
    tft.setTextColor(ILI9341_WHITE);
    tft.fillRoundRect(trackInfo.x, trackInfo.y, trackInfo.w, trackInfo.h, 8,
                      ILI9341_BLACK);
    tft.print(name);
  }
}

// Display volume
void Display::displayVol() {

  if (!libraryVisible) {
    // read the knob position (analog input A1)
    int vol = map(analogRead(A1), 0, 1000, 0, 100);

    if (knobReset == KnobReset::ON) {
      tft.setCursor(volume.x + 12, volume.y + 8);
      tft.setFont(Arial_10);
      tft.setTextColor(ILI9341_WHITE);
      tft.fillRoundRect(volume.x, volume.y, volume.w, volume.h, 8,
                        ILI9341_BLACK);
      tft.println("Vol");
      tft.setCursor(volume.x + 15, volume.y + 28);
      tft.print(vol);
      knobReset = KnobReset::OFF;
    }

    // Volume pot is very sensitive - need to set a change range
    if (volumeChange <= (vol * .95) || volumeChange >= (vol * 1.05)) {
      tft.setCursor(volume.x + 12, volume.y + 8);
      tft.setFont(Arial_10);
      tft.setTextColor(ILI9341_WHITE);
      tft.fillRoundRect(volume.x, volume.y, volume.w, volume.h, 8,
                        ILI9341_BLACK);
      tft.println("Vol");
      tft.setCursor(volume.x + 15, volume.y + 28);
      tft.print(vol);
      // update volume if changed
      volumeChange = vol;
    }
  }
}

// Display Library - select from library
void Display::displayLibrary(const Library &obj) {

  if (!displayOnce) {
    p.x = 0;
    p.y = 0;

    for (int y = 0; y < 240; y++) {
      uint8_t r = map(y, 0, 240, 255, 0);
      uint8_t b = map(y, 0, 240, 0, 255);
      uint16_t color = tft.color565(r, 0, b);
      tft.fillRect(0, y, 320, 1, color);
    }

    tft.setFont(BUTTON_FONT);
    tft.setTextColor(ILI9341_WHITE);

    tft.setCursor(next.x + 5, next.y + 5);
    tft.fillRoundRect(next.x, next.y, next.w, next.h, 8, ILI9341_BLACK);
    tft.print("Next");

    tft.setCursor(prev.x + 5, prev.y + 5);
    tft.fillRoundRect(prev.x, prev.y, prev.w, prev.h, 8, ILI9341_BLACK);
    tft.print("Prev");

    tft.setCursor(trackInfo.x + 50, trackInfo.y + 20);
    tft.fillRoundRect(trackInfo.x, trackInfo.y, trackInfo.w, trackInfo.h, 8,
                      ILI9341_BLACK);
    tft.print("Select Track - tap to exit");

    libraryTracks(0, obj);
    displayOnce = true;
    libraryVisible = true;
    delay(500);
  }

  if (ts.touched() && isTouched) {
    // touchscreen is no longer being touched, reset flag
    isTouched = false;
  }

  if (ts.touched() && isTouched == false) {
    // get touch location
    getPoint();

    isTouched = true;

    if ((p.x > trackInfo.x) && (p.x < (trackInfo.x + trackInfo.w))) {
      if ((p.y > trackInfo.y) && (p.y <= (trackInfo.y + trackInfo.h))) {
        isTouched = false;
        libraryVisible = false;
      }
    }

    if ((p.x > next.x) && (p.x < (next.x + next.w))) {
      if ((p.y > next.y) && (p.y <= (next.y + next.h))) {

        Serial.println("Next pressed");
        if (index + 4 > obj.size - 1) {
          // do nothing
        } else {
          index = index + 4;
        }
        libraryTracks(index, obj);
      }
    }

    if ((p.x > prev.x) && (p.x < (prev.x + prev.w))) {
      if ((p.y > prev.y) && (p.y <= (prev.y + prev.h))) {

        Serial.println("Prev pressed");
        index = index - 4;

        if (index < 0) {
          index = 0;
        }
        libraryTracks(index, obj);
      }
    }

    if ((p.x > track1.x) && (p.x < (track1.x + track1.w))) {
      if ((p.y > track1.y) && (p.y <= (track1.y + track1.h))) {
        Serial.println("track one selected");
        libraryName = obj.fileArray[index];
        libraryVisible = false;
      }
    }
    if ((p.x > track2.x) && (p.x < (track2.x + track2.w))) {
      if ((p.y > track2.y) && (p.y <= (track2.y + track2.h))) {
        Serial.println("track two selected");
        libraryName = obj.fileArray[index + 1];
        libraryVisible = false;
      }
    }
    if ((p.x > track3.x) && (p.x < (track3.x + track3.w))) {
      if ((p.y > track3.y) && (p.y <= (track3.y + track3.h))) {
        Serial.println("track three selected");
        libraryName = obj.fileArray[index + 2];
        libraryVisible = false;
      }
    }
    if ((p.x > track4.x) && (p.x < (track4.x + track4.w))) {
      if ((p.y > track4.y) && (p.y <= (track4.y + track4.h))) {
        Serial.println("track four selected");
        libraryName = obj.fileArray[index + 3];
        libraryVisible = false;
      }
    }
  }

  if (ts.touched() && !libraryVisible) {
    // touchscreen is no longer being touched, reset flag
    isTouched = false;
    displayOnce = false;
    index = 0;
    // Set exit flag
    knobReset = KnobReset::ON;
    // reset to main screen
    mainScreen();
    // sets new file name to be used
    setFileName(libraryName);
    // sets name change flag
    setNameChange(true);
  }
}

// Helper for displayLibrary
void Display::libraryTracks(int index, const Library &obj) {

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

void Display::readTouch() {
  if (ts.touched()) {
    delay(100);
    getPoint();
  }
}

bool Display::libraryClicked() {
  if (libraryVisible)
    return false;
  if ((p.x > library.x) && (p.x < (library.x + library.w))) {
    if ((p.y > library.y) && (p.y <= (library.y + library.h))) {
      Serial.println("Library button clicked");
      return true;
    }
  }
  return false;
}

bool Display::saveClicked() {
  if (libraryVisible)
    return false;
  if ((p.x > save.x) && (p.x < (save.x + save.w))) {
    if ((p.y > save.y) && (p.y <= (save.y + save.h))) {
      Serial.println("Save button clicked");
      return true;
    }
  }
  return false;
}

bool Display::reverseClicked() {
  if (libraryVisible)
    return false;
  if ((p.x > reverse.x) && (p.x < (reverse.x + reverse.w))) {
    if ((p.y > reverse.y) && (p.y <= (reverse.y + reverse.h))) {
      Serial.println("Reverse button clicked");
      return true;
    }
  }
  return false;
}

// Reacts to touch
void Display::handleTouch(const Library &obj) {

  isTouched = ts.touched();
  if (isTouched) {

    delay(100);
    getPoint();
  }

  if (libraryVisible) {
    displayLibrary(obj);
  }
  if (!libraryVisible && isTouched) {
    // select mode
    if ((p.x > mode.x) && (p.x < (mode.x + mode.w))) {
      if ((p.y > mode.y) && (p.y <= (mode.y + mode.h))) {
        selectMode();
      }
    }
    // display library
    if ((p.x > library.x) && (p.x < (library.x + library.w))) {
      if ((p.y > library.y) && (p.y <= (library.y + library.h))) {
        displayLibrary(obj);
        Serial.println("Library");
      }
    }
    // Reverse on / off
    if ((p.x > reverse.x) && (p.x < (reverse.x + reverse.w))) {
      if ((p.y > reverse.y) && (p.y <= (reverse.y + reverse.h))) {
        handleReverseButton();
      }
    }
    // Save Button
    if ((p.x > save.x) && (p.x < (save.x + save.w))) {
      if ((p.y > save.y) && (p.y <= (save.y + save.h))) {

        // Call save function here
        saveAlert();
        mainScreen();
      }
    }
  }

  if (!ts.touched() && isTouched) {
    isTouched = false; // touchscreen is no longer being touched, reset flag
  }
}

// Displays the mode - Overdub or Replace const Mode& modeObj
// Sets mode changed flags
void Display::selectMode() {

  tft.setFont(BUTTON_FONT);
  tft.setTextColor(ILI9341_WHITE);

  isTouched = false;

  if (ts.touched() && isTouched == false) {

    getPoint();
    tft.setCursor(mode.x + 7, mode.y + 10);

    if ((p.x > mode.x) && (p.x < (mode.x + mode.w))) {
      if ((p.y > mode.y) && (p.y <= (mode.y + mode.h))) {
        if (modeObj == Mode::Overdub && !isTouched) {
          tft.fillRoundRect(mode.x, mode.y, mode.w, mode.h, 8, ILI9341_BLACK);
          tft.print("Replace");
          modeObj = Mode::Replace;
          modeChange = true;
          isTouched = true;
        }
        if (modeObj == Mode::Replace && !isTouched) {
          tft.fillRoundRect(mode.x, mode.y, mode.w, mode.h, 8, ILI9341_BLACK);
          tft.print("Overdub");
          modeObj = Mode::Overdub;
          modeChange = true;
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
void Display::displayPan() {

  if (!libraryVisible) {
    int16_t pan = map(analogRead(A2), 0, 1000, 0, 100);
    if (knobReset == KnobReset::ON) {
      tft.setCursor(panBar.x + 40, panBar.y + 10);
      tft.setFont(Arial_14);
      tft.setTextColor(ILI9341_BLACK);
      tft.fillRoundRect(panBar.x, panBar.y, panBar.w, panBar.h, 8,
                        ILI9341_DARKGREY);
      tft.print("PAN");
      tft.fillCircle(panDot.x + pan, panDot.y, panDot.w, ILI9341_NAVY);
    }

    if (panChange < (pan - 5) || panChange > (pan + 1)) {

      tft.setCursor(panBar.x + 40, panBar.y + 10);
      tft.setFont(Arial_14);
      tft.setTextColor(ILI9341_BLACK);
      tft.fillRoundRect(panBar.x, panBar.y, panBar.w, panBar.h, 8,
                        ILI9341_DARKGREY);
      tft.print("PAN");
      tft.fillCircle(panDot.x + pan, panDot.y, panDot.w, ILI9341_NAVY);
      panChange = pan;
    }
  }
}

// displays the reverse button.
// Sets flag for reverse
void Display::handleReverseButton() {

  isTouched = false;

  if (ts.touched() && isTouched == false) {
    delay(100);

    getPoint();

    if ((p.x > reverse.x) && (p.x < (reverse.x + reverse.w))) {
      if ((p.y > reverse.y) && (p.y <= (reverse.y + reverse.h))) {
        tft.setFont(BUTTON_FONT);
        tft.setTextColor(ILI9341_BLACK);
        tft.setCursor(reverse.x + 7, reverse.y + 10);
        if (!reverseBool && !isTouched) {
          tft.fillRoundRect(reverse.x, reverse.y, reverse.w, reverse.h, 8,
                            ILI9341_RED);
          tft.print("Rev ON");
          setRevBool(true);
          isTouched = true;
        }
        if (reverseBool && !isTouched) {
          tft.fillRoundRect(reverse.x, reverse.y, reverse.w, reverse.h, 8,
                            ILI9341_DARKGREY);
          tft.print("Rev OFF");
          setRevBool(false);
          isTouched = true;
        }
      }
    }
  }
}

void Display::updateStatus(Status status) {
  if (libraryVisible)
    return;
  if (status == displayedStatus)
    return;
  displayedStatus = status;
  recordButton(status == Status::Record);
  stopButton(status == Status::Stop);
  playButton(status == Status::Play);
}

void Display::displayPosition(uint32_t position, uint32_t length) {
  if (!libraryVisible) {
    int x = map(position, 0, length, 10, 295);

    // tft.fillRoundRect(10,50,300,10,8,ILI9341_BLACK);
    // tft.fillCircle(10+x,53, 5, ILI9341_WHITE);

    tft.drawFastHLine(10 + x, 50, 5, ILI9341_WHITE);
    tft.drawFastHLine(10 + x, 51, 5, ILI9341_WHITE);
    tft.drawFastHLine(10 + x, 52, 5, ILI9341_WHITE);
    tft.drawFastHLine(10 + x, 53, 5, ILI9341_WHITE);
    tft.drawFastHLine(10 + x, 54, 5, ILI9341_WHITE);
    tft.drawFastHLine(10 + x, 55, 5, ILI9341_WHITE);

    tft.drawFastHLine(10 + x, 50, 1, ILI9341_BLACK);
    tft.drawFastHLine(10 + x, 51, 1, ILI9341_BLACK);
    tft.drawFastHLine(10 + x, 52, 1, ILI9341_BLACK);
    tft.drawFastHLine(10 + x, 53, 1, ILI9341_BLACK);
    tft.drawFastHLine(10 + x, 54, 1, ILI9341_BLACK);
    tft.drawFastHLine(10 + x, 55, 1, ILI9341_BLACK);
  }
}
