#include <Arduino.h>
#include <config.h>
#include <utils.hpp>

// Adjust the mic gain in response to the peak level
// TODO Implement or delete this
void adjustMicLevel() {}

// Read the volume knob position (analog input A1)
float adjustVolume(AudioControlSGTL5000 &interface) {
  int knob = analogRead(volumePin);
  float vol = (float)knob / 1280.0;
  interface.volume(vol);
  return vol;
}

// Configure the pushbutton pins
void configureButtons() {
  pinMode(buttonRecordPin, INPUT_PULLUP);
  pinMode(buttonStopPin, INPUT_PULLUP);
  pinMode(buttonPlayPin, INPUT_PULLUP);
}

// Find and start the audio shield
void findSGTL5000(AudioControlSGTL5000 &interface) {
  // search for SGTL5000 at both I²C addresses
  for (int i = 0; i < 2; i++) {
    uint8_t levels[]{LOW, HIGH};

    interface.setAddress(levels[i]);
    interface.enable();
    if (interface.volume(0.2)) {
      Serial.printf("SGTL5000 found at %s address\n", i ? "HIGH" : "LOW");
      break;
    }
  }
}

// Enable the audio shield, select input, and enable output
void initializeInterface(AudioControlSGTL5000 &interface) {
  findSGTL5000(interface);
  interface.inputSelect(input);
  if (input == AUDIO_INPUT_MIC) {
    interface.micGain(25);
  } else {
    interface.lineInLevel(2);
  }
  interface.volume(0.5);
}

// Initialize the SD card
void initializeSdCard() {
  SPI.setMOSI(sdCardMosiPin);
  SPI.setSCK(sdCardSckPin);
  while (!(SD.begin(sdCardCsPin))) {
    // stop here if no SD card, but print a message
    Serial.println("Unable to access the SD card");
    delay(500);
  }
}

// Start USB virtual serial communication
void initializeSerialCommunication() {
  Serial.begin(9600);

  delay(1000);
  if (CrashReport) {
    Serial.println(CrashReport);
    CrashReport.clear();
  }
}

void monitorAudioEngine(elapsedMillis *ms) {
  if (*ms > 10000) {
    Serial.print("Proc = ");
    Serial.print(AudioProcessorUsage());
    Serial.print(" (");
    Serial.print(AudioProcessorUsageMax());
    Serial.print("),  Mem = ");
    Serial.print(AudioMemoryUsage());
    Serial.print(" (");
    Serial.print(AudioMemoryUsageMax());
    Serial.println(")");
    *ms = 0;
  }
}

// Read and display stereo input or output channels
void showLevels(AudioAnalyzePeak *peakL, AudioAnalyzePeak *peakR,
                elapsedMillis *ms) {
  if (*ms > 1000) {
    int lp = 0, rp = 0, scale = 20;
    char cl = '?', cr = '?';
    char buf[scale * 2 + 10];

    *ms = 0;

    if (peakL->available()) {
      lp = peakL->read() * scale;
      cl = '#';
    }
    if (peakR->available()) {
      rp = peakR->read() * scale;
      cr = '#';
    }

    for (int i = 0; i < scale; i++) {
      buf[scale - i - 1] = lp >= i ? cl : ' ';
      buf[scale + i + 1] = rp >= i ? cr : ' ';
      buf[scale] = '|';
      buf[scale * 2 + 2] = 0;
    }
    Serial.println(buf);
  }
}
