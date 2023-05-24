#include "ILI9341_t3.h"
#include <Arduino.h>
#include <SD.h>
#include <display.hpp>

// touchscreen offset for four corners
const Display::Layout Display::touchScreen = {400, 400, 3879, 3843};

// Display item location values - Screen is 320x240
const Display::Layout Display::alert = {60, 110, 200, 50};
const Display::Layout Display::boot = {40, 120, 240, 360};
const Display::Layout Display::library = {140, 110, 170, 30};
const Display::Layout Display::mode = {10, 195, 90, 40};
const Display::Layout Display::next = {260, 100, 50, 50};
const Display::Layout Display::nextTrack = {110, 148, 105, 40};
const Display::Layout Display::prev = {10, 100, 50, 50};
const Display::Layout Display::panBar = {10, 110, 120, 30};
const Display::Layout Display::panDot = {16, 125, 5, 0};
const Display::Layout Display::play = {190, 70, 105, 32};
const Display::Layout Display::record = {10, 70, 90, 32};
const Display::Layout Display::reverse = {10, 148, 90, 40};
const Display::Layout Display::save = {110, 195, 90, 40};
const Display::Layout Display::status = {270, 10, 50, 50};
const Display::Layout Display::stop = {110, 70, 70, 32};
const Display::Layout Display::trackInfo = {10, 10, 260, 50};
const Display::Layout Display::track1 = {70, 70, 180, 32};
const Display::Layout Display::track2 = {70, 110, 180, 32};
const Display::Layout Display::track3 = {70, 150, 180, 32};
const Display::Layout Display::track4 = {70, 190, 180, 32};
const Display::Layout Display::volume = {260, 185, 50, 50};

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
  tft.fillRoundRect(
      library.x, library.y, library.w, library.h, 4, ILI9341_BLACK
  );
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
  tft.fillRoundRect(
      panBar.x, panBar.y, panBar.w, panBar.h, 8, ILI9341_DARKGREY
  );
  tft.print("PAN");
  tft.fillCircle(panDot.x + panVal, panDot.y, panDot.w, ILI9341_NAVY);

  state.pan = p;
}

void Display::drawPause() {
  const int padding = 15;
  const int barWidth = (status.w - padding * 2) / 3;
  tft.fillRect(status.x, status.y, status.w, status.h, bgColor);
  tft.fillRect(
      status.x + padding, status.y + padding, barWidth, status.h - padding * 2,
      ILI9341_ORANGE
  );
  tft.fillRect(
      status.x + status.w - padding - barWidth, status.y + padding, barWidth,
      status.h - padding * 2, ILI9341_ORANGE
  );
  return;
}

void Display::drawPlay() {
  const int padding = 15;
  tft.fillRect(status.x, status.y, status.w, status.h, bgColor);
  tft.fillTriangle(
      status.x + padding, status.y + padding, status.x + status.w - padding,
      status.y + status.h / 2, status.x + padding,
      status.y + status.h - padding, ILI9341_GREEN
  );
}

void Display::drawPosition(uint32_t position, uint32_t length) {
  if (!redraw && position == state.position && length == state.length)
    return;
  if (position == length)
    return;

  const int padding = 10;
  const int trackNameWidth = tft.measureTextWidth("Track X");
  const uint16_t start = trackInfo.x + trackNameWidth + padding * 2;
  const uint16_t width = trackInfo.w - (trackNameWidth + padding * 2) - padding;

  int x = map(position, 0, length, start, trackInfo.w - padding);

  int midY = trackInfo.y + trackInfo.h / 2;

  tft.drawFastHLine(start, midY - 3, width, ILI9341_BLACK);
  tft.drawFastHLine(start, midY - 2, width, ILI9341_BLACK);
  tft.drawFastHLine(start, midY - 1, width, ILI9341_BLACK);
  tft.drawFastHLine(start, midY, width, ILI9341_BLACK);
  tft.drawFastHLine(start, midY + 1, width, ILI9341_BLACK);
  tft.drawFastHLine(start, midY + 2, width, ILI9341_BLACK);
  tft.drawFastHLine(start, midY + 3, width, ILI9341_BLACK);

  tft.drawFastHLine(x, midY - 3, 5, ILI9341_WHITE);
  tft.drawFastHLine(x, midY - 2, 5, ILI9341_WHITE);
  tft.drawFastHLine(x, midY - 1, 5, ILI9341_WHITE);
  tft.drawFastHLine(x, midY, 5, ILI9341_WHITE);
  tft.drawFastHLine(x, midY + 1, 5, ILI9341_WHITE);
  tft.drawFastHLine(x, midY + 2, 5, ILI9341_WHITE);
  tft.drawFastHLine(x, midY + 3, 5, ILI9341_WHITE);

  state.position = position;
  state.length = length;
}

void Display::drawRecord() {
  const int padding = 15;
  tft.fillRect(status.x, status.y, status.w, status.h, bgColor);
  tft.fillCircle(
      status.x + status.w / 2, status.y + status.h / 2,
      (status.w - padding * 2) / 2, ILI9341_RED
  );
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

  switch (status) {
  case Status::Stop:
    drawStop();
    break;
  case Status::Play:
    drawPlay();
    break;
  case Status::Pause:
    drawPause();
    break;
  case Status::Record:
    drawRecord();
    break;
  default:
    Serial.println("!!! Unhandled status !!!");
  }

  state.status = status;
}

void Display::drawStop() {
  const int padding = 15;
  tft.fillRect(status.x, status.y, status.w, status.h, bgColor);
  tft.fillRect(
      status.x + padding, status.y + padding, status.w - padding * 2,
      status.h - padding * 2, ILI9341_PURPLE
  );
}

// Display Track name
void Display::drawTrackName(int track) {
  if (!redraw && track == state.track)
    return;

  tft.fillRoundRect(
      trackInfo.x, trackInfo.y, trackInfo.w, trackInfo.h, 8, ILI9341_BLACK
  );
  tft.setFont(BUTTON_FONT);
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(
      trackInfo.x + 10, trackInfo.y + (trackInfo.h - tft.fontCapHeight()) / 2
  );
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
    // reverseButton();
  } else {
    drawMainNavButton();
    drawPreviousButton();
    drawNextButton();
    drawLibraryEntries(newState.loading);
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
    tft.fillRoundRect(
        reverse.x, reverse.y, reverse.w, reverse.h, 8, ILI9341_RED
    );
    tft.print("Rev ON");

  } else {
    tft.fillRoundRect(
        reverse.x, reverse.y, reverse.w, reverse.h, 8, ILI9341_DARKGREY
    );
    tft.print("Rev OFF");
  }
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
  tft.fillRoundRect(
      nextTrack.x, nextTrack.y, nextTrack.w, nextTrack.h, 8, ILI9341_BLACK
  );
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
  tft.fillRoundRect(
      trackInfo.x, trackInfo.y, trackInfo.w, trackInfo.h, 8, ILI9341_BLACK
  );
  tft.print("Select loop file - tap to exit");
}

void Display::drawLibraryEntries(bool l) {
  int p = libPage;
  if (clickedNext() && (p + 1) * numLibEntries < lib.size &&
      lib.fileArray[(p + 1) * numLibEntries]) {
    p++;
  } else if (clickedPrevious() && p > 0) {
    p--;
  }

  if (!redraw && p == libPage && l == state.loading)
    return;

  tft.setFont(BUTTON_FONT);
  tft.setTextColor(ILI9341_WHITE);

  for (int i = 0; i < numLibEntries; i++) {
    const Layout *entry = libraryEntries[i];
    const String fileName = lib.fileArray[p * numLibEntries + i];

    tft.fillRoundRect(entry->x, entry->y, entry->w, entry->h, 8, ILI9341_BLACK);
    tft.setCursor(entry->x + 25, entry->y + 8);
    if (l && fileName == String(selectedLibEntry)) {
      tft.print("Loading");
    } else {
      tft.print(fileName);
    }
  }

  libPage = p;
  state.loading = l;
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
