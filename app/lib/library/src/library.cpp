#include <Arduino.h>
#include <SD.h>
#include <display.hpp>
#include <library.hpp>

// create the array of filenames
void Library::array() {
  delete[] fileArray;
  fileArray = new String[size];
  index = 0;
  File loops = SD.open("/loops");
  while (true) {
    if (index >= size)
      break;
    File track = loops.openNextFile();
    if (!track) {
      break;
    }
    // Skip directories
    if (track.isDirectory()) {
      continue;
    }
    fileArray[index++] = track.name();
    track.close();
  };
  loops.close();
}

// Add a value to the array
void Library::addValue(String value) {
  if (index > size - 1) {
    resize();
    fileArray[index++] = value;
  } else {
    fileArray[index++] = value;
  }
}

// Resize the array
void Library::resize() {
  String *temp = new String[size * 2];
  for (int i = 0; i < size; i++) {
    temp[i] = fileArray[i];
  }
  size = size * 2;
  fileArray = temp;
  delete[] temp;
}

// Return value from array
String *Library::returnValue() { return fileArray; }
