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
const Display::Layout Display::library = {140, 110, 170, 30};
const Display::Layout Display::trackInfo = {10, 10, 300, 50};
const Display::Layout Display::track1 = {70, 70, 180, 32};
const Display::Layout Display::track2 = {70, 110, 180, 32};
const Display::Layout Display::track3 = {70, 150, 180, 32};
const Display::Layout Display::track4 = {70, 190, 180, 32};
const Display::Layout Display::next = {260, 100, 50, 50};
const Display::Layout Display::nextTrack = {110, 148, 105, 40};
const Display::Layout Display::prev = {10, 100, 50, 50};
const Display::Layout Display::panBar = {10, 110, 120, 30};
const Display::Layout Display::panDot = {16, 125, 5, 0};
const Display::Layout Display::save = {110, 195, 90, 40};
const Display::Layout Display::alert = {60, 110, 200, 50};

void Display::clearScreen() {
  for (int y = 0; y < 240; y++) {
    uint8_t r = map(y, 0, 240, 255, 0);
    uint8_t b = map(y, 0, 240, 0, 255);
    uint16_t color = tft.color565(r, 0, b);
    tft.fillRect(0, y, 320, 1, color);
  }
}

bool Display::clickedLibraryEntry() {
  if (!isTouched)
    return false;
  if (screen != Screen::Library)
    return false;
  for (int i = 0; i < numLibEntries; i++) {
    const Layout *entry = libraryEntries[i];
    if ((p.x > entry->x) && (p.x < (entry->x + entry->w))) {
      if ((p.y > entry->y) && (p.y <= (entry->y + entry->h))) {
        selectedLibEntry = lib.fileArray[libPage * numLibEntries + i].c_str();
        Serial.printf("Loop %s selected\n", lib.fileArray[i].c_str());
        isTouched = false;
        return true;
      }
    }
  }
  return false;
}

bool Display::clickedLibraryNav() {
  if (screen != Screen::Main)
    return false;
  if (!isTouched)
    return false;
  if ((p.x > library.x) && (p.x < (library.x + library.w))) {
    if ((p.y > library.y) && (p.y <= (library.y + library.h))) {
      Serial.println("Library button clicked");
      isTouched = false;
      return true;
    }
  }
  return false;
}

bool Display::clickedMainNav() {
  if (!isTouched)
    return false;
  if (screen != Screen::Library)
    return false;
  if ((p.x > trackInfo.x) && (p.x < (trackInfo.x + trackInfo.w))) {
    if ((p.y > trackInfo.y) && (p.y <= (trackInfo.y + trackInfo.h))) {
      Serial.println("Return to main screen clicked");
      isTouched = false;
      return true;
    }
  }
  return false;
}

bool Display::clickedNext() {
  if (!isTouched)
    return false;
  if (screen != Screen::Library)
    return false;
  if ((p.x > next.x) && (p.x < (next.x + next.w))) {
    if ((p.y > next.y) && (p.y <= (next.y + next.h))) {
      Serial.println("Next button clicked");
      isTouched = false;
      return true;
    }
  }
  return false;
}

bool Display::clickedNextTrack() {
  if (!isTouched)
    return false;
  if (screen != Screen::Main)
    return false;
  if ((p.x > nextTrack.x) && (p.x < (nextTrack.x + nextTrack.w))) {
    if ((p.y > nextTrack.y) && (p.y <= (nextTrack.y + nextTrack.h))) {
      Serial.println("Next Track button clicked");
      isTouched = false;
      return true;
    }
  }
  return false;
}

bool Display::clickedPrevious() {
  if (!isTouched)
    return false;
  if (screen != Screen::Library)
    return false;
  if ((p.x > prev.x) && (p.x < (prev.x + prev.w))) {
    if ((p.y > prev.y) && (p.y <= (prev.y + prev.h))) {
      Serial.println("Previous button clicked");
      isTouched = false;
      return true;
    }
  }
  return false;
}

bool Display::clickedMode() {
  if (!isTouched)
    return false;
  if (screen != Screen::Main)
    return false;
  if ((p.x > mode.x) && (p.x < (mode.x + mode.w))) {
    if ((p.y > mode.y) && (p.y <= (mode.y + mode.h))) {
      Serial.println("Mode button clicked");
      isTouched = false;
      return true;
    }
  }
  return false;
}

bool Display::clickedReverse() {
  if (!isTouched)
    return false;
  if (screen != Screen::Main)
    return false;
  if ((p.x > reverse.x) && (p.x < (reverse.x + reverse.w))) {
    if ((p.y > reverse.y) && (p.y <= (reverse.y + reverse.h))) {
      Serial.println("Reverse button clicked");
      isTouched = false;
      return true;
    }
  }
  return false;
}

bool Display::clickedSave() {
  if (!isTouched)
    return false;
  if (screen != Screen::Main)
    return false;
  if ((p.x > save.x) && (p.x < (save.x + save.w))) {
    if ((p.y > save.y) && (p.y <= (save.y + save.h))) {
      Serial.println("Save button clicked");
      isTouched = false;
      return true;
    }
  }
  return false;
}

void Display::drawLibraryNavButton() {
  if (!redraw)
    return;

  tft.setCursor(library.x + 50, library.y + 10);
  tft.setFont(BUTTON_FONT);
  tft.setTextColor(ILI9341_WHITE);
  tft.fillRoundRect(library.x, library.y, library.w, library.h, 4,
                    ILI9341_BLACK);
  tft.print("Library");
}

// Displays the mode - Overdub or Replace
void Display::drawModeButton(Mode m) {
  if (!redraw && m == state.mode)
    return;

  tft.setFont(BUTTON_FONT);
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(mode.x + 7, mode.y + 10);
  tft.fillRoundRect(mode.x, mode.y, mode.w, mode.h, 8, ILI9341_BLACK);

  tft.print(m == Mode::Replace ? "Replace" : "Overdub");

  state.mode = m;
}

// displays pan
void Display::drawPan(float p) {
  if (!redraw && p == state.pan)
    return;

  int panVal = (int)(p * 100);

  tft.setCursor(panBar.x + 40, panBar.y + 10);
  tft.setFont(Arial_14);
  tft.setTextColor(ILI9341_BLACK);
  tft.fillRoundRect(panBar.x, panBar.y, panBar.w, panBar.h, 8,
                    ILI9341_DARKGREY);
  tft.print("PAN");
  tft.fillCircle(panDot.x + panVal, panDot.y, panDot.w, ILI9341_NAVY);

  state.pan = p;
}

void Display::drawPosition(uint32_t position, uint32_t length) {
  if (!redraw && position == state.position && length == state.length)
    return;
  if (position == length)
    return;

  int x = map(position, 0, length, 20, 290);

  tft.drawFastHLine(20, 50, 285, ILI9341_BLACK);
  tft.drawFastHLine(20, 51, 285, ILI9341_BLACK);
  tft.drawFastHLine(20, 52, 285, ILI9341_BLACK);
  tft.drawFastHLine(20, 53, 285, ILI9341_BLACK);
  tft.drawFastHLine(20, 54, 285, ILI9341_BLACK);
  tft.drawFastHLine(20, 55, 285, ILI9341_BLACK);

  tft.drawFastHLine(x, 50, 5, ILI9341_WHITE);
  tft.drawFastHLine(x, 51, 5, ILI9341_WHITE);
  tft.drawFastHLine(x, 52, 5, ILI9341_WHITE);
  tft.drawFastHLine(x, 53, 5, ILI9341_WHITE);
  tft.drawFastHLine(x, 54, 5, ILI9341_WHITE);
  tft.drawFastHLine(x, 55, 5, ILI9341_WHITE);

  state.position = position;
  state.length = length;
}

void Display::drawSaveButton(bool s) {
  if (!redraw && s == state.saving)
    return;
  if (s == false && state.saving == true) {
    // New loop saved, refresh library
    lib.array();
  }

  tft.setFont(BUTTON_FONT);
  tft.setTextColor(ILI9341_BLACK);
  tft.setCursor(save.x + 22, save.y + 12);
  tft.fillRoundRect(save.x, save.y, save.w, save.h, 8, ILI9341_DARKGREY);
  tft.print(s ? "Saving" : "Save");

  state.saving = s;
}

void Display::drawStatus(Status status) {
  if (!redraw && status == state.status)
    return;

  recordButton(status == Status::Record);
  stopButton(status == Status::Pause);
  playButton(status == Status::Play);

  state.status = status;
}

// Display Track name
void Display::drawTrackName(int track) {
  if (!redraw && track == state.track)
    return;

  tft.setCursor(trackInfo.x + 50, trackInfo.y + 16);
  tft.setFont(BUTTON_FONT);
  tft.setTextColor(ILI9341_WHITE);
  tft.fillRoundRect(trackInfo.x, trackInfo.y, trackInfo.w, trackInfo.h, 8,
                    ILI9341_BLACK);
  tft.print("Track ");
  tft.print(track + 1);

  state.track = track;
}

// Display volume
void Display::drawVolume(float v) {
  if (!redraw && v == state.volume)
    return;

  int volumeVal = (int)(v * 100);

  tft.setCursor(volume.x + 12, volume.y + 8);
  tft.setFont(Arial_10);
  tft.setTextColor(ILI9341_WHITE);
  tft.fillRoundRect(volume.x, volume.y, volume.w, volume.h, 8, ILI9341_BLACK);
  tft.println("Vol");
  tft.setCursor(volume.x + 15, volume.y + 28);
  tft.print(volumeVal);

  state.volume = v;
}

void Display::readTouch() {
  if (ts.touched()) {
    delay(100);
    getPoint();
    isTouched = true;
  }
}

void Display::showLibraryScreen() {
  if (screen == Screen::Library)
    return;
  screen = Screen::Library;
  redraw = true;
}

void Display::showMainScreen() {
  if (screen == Screen::Main)
    return;
  libPage = 0;
  screen = Screen::Main;
  redraw = true;
}

void Display::update(AppState newState) {
  if (clickedMainNav()) {
    showMainScreen();
  }

  if (clickedLibraryNav()) {
    showLibraryScreen();
  }

  if (redraw)
    clearScreen();

  if (screen == Screen::Main) {
    drawPosition(newState.position, newState.length);
    drawModeButton(newState.mode);
    drawNextTrackButton();
    drawPan(newState.pan);
    drawStatus(newState.status);
    drawSaveButton(newState.saving);
    drawVolume(newState.volume);
    drawTrackName(newState.track);
    drawLibraryNavButton();
    reverseButton();
  } else {
    drawMainNavButton();
    drawPreviousButton();
    drawNextButton();
    drawLibraryEntries();
  }

  redraw = false;
}

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
  redraw = true;
}

// setup reverse button
void Display::reverseButton() {
  if (!redraw)
    return;

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

// Set / Display play button
void Display::playButton(bool playStatus) {
  tft.setCursor(play.x + 8, play.y + 8);
  tft.setFont(BUTTON_FONT);
  tft.setTextColor(ILI9341_WHITE);

  if (!playStatus) { // button is set inactive, redraw button inactive
    tft.fillRoundRect(play.x, play.y, play.w, play.h, 4, ILI9341_BLACK);
  } else { // button is active, redraw button active
    tft.fillRoundRect(play.x, play.y, play.w, play.h, 4, ILI9341_GREEN);
  }
  tft.print("Play");
}

// Set / Display record button
void Display::recordButton(bool recordStatus) {
  tft.setCursor(record.x + 8, record.y + 8);
  tft.setFont(BUTTON_FONT);
  tft.setTextColor(ILI9341_WHITE);

  if (!recordStatus) { // button is set inactive, redraw button inactive
    tft.fillRoundRect(record.x, record.y, record.w, record.h, 4, ILI9341_BLACK);
  } else { // button is active, redraw button active
    tft.fillRoundRect(record.x, record.y, record.w, record.h, 4, ILI9341_RED);
  }
  tft.print("Record");
}

// Set / Display stop button
void Display::stopButton(bool stopStatus) {
  tft.setCursor(stop.x + 8, stop.y + 8);
  tft.setFont(BUTTON_FONT);
  tft.setTextColor(ILI9341_WHITE);

  if (stopStatus) { // button is set inactive, redraw button inactive
    tft.fillRoundRect(stop.x, stop.y, stop.w, stop.h, 4, ILI9341_GREEN);
  } else { // button is active, redraw button active
    tft.fillRoundRect(stop.x, stop.y, stop.w, stop.h, 4, ILI9341_BLACK);
  }
  tft.print("Pause");
}

void Display::drawNextButton() {
  if (!redraw)
    return;

  tft.setFont(BUTTON_FONT);
  tft.setTextColor(ILI9341_WHITE);

  tft.setCursor(next.x + 5, next.y + 5);
  tft.fillRoundRect(next.x, next.y, next.w, next.h, 8, ILI9341_BLACK);
  tft.print("Next");
}

void Display::drawNextTrackButton() {
  if (!redraw)
    return;

  tft.setFont(BUTTON_FONT);
  tft.setTextColor(ILI9341_WHITE);

  tft.setCursor(nextTrack.x + 7, nextTrack.y + 10);
  tft.fillRoundRect(nextTrack.x, nextTrack.y, nextTrack.w, nextTrack.h, 8,
                    ILI9341_BLACK);
  tft.print("Next Track");
}

void Display::drawPreviousButton() {
  if (!redraw)
    return;

  tft.setFont(BUTTON_FONT);
  tft.setTextColor(ILI9341_WHITE);

  tft.setCursor(prev.x + 5, prev.y + 5);
  tft.fillRoundRect(prev.x, prev.y, prev.w, prev.h, 8, ILI9341_BLACK);
  tft.print("Prev");
}

void Display::drawMainNavButton() {
  if (!redraw)
    return;

  tft.setFont(BUTTON_FONT);
  tft.setTextColor(ILI9341_WHITE);

  tft.setCursor(trackInfo.x + 50, trackInfo.y + 20);
  tft.fillRoundRect(trackInfo.x, trackInfo.y, trackInfo.w, trackInfo.h, 8,
                    ILI9341_BLACK);
  tft.print("Select loop file - tap to exit");
}

void Display::drawLibraryEntries() {
  int p = libPage;
  if (clickedNext() && (p + 1) * numLibEntries < lib.size) {
    p++;
  } else if (clickedPrevious() && p > 0) {
    p--;
  }

  if (!redraw && p == libPage)
    return;

  tft.setFont(BUTTON_FONT);
  tft.setTextColor(ILI9341_WHITE);

  for (int i = 0; i < numLibEntries; i++) {
    const Layout *entry = libraryEntries[i];

    tft.fillRoundRect(entry->x, entry->y, entry->w, entry->h, 8, ILI9341_BLACK);
    tft.setCursor(entry->x + 25, entry->y + 8);
    tft.print(lib.fileArray[p * numLibEntries + i]);
  }

  libPage = p;
}

/* // displays the reverse button.
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
} */
