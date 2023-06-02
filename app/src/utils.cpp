#include <config.h>
#include <math.h>
#include <utils.hpp>

const float knobMax = 1023.0;

void readFade(AppState &appState) {
  int knob = analogRead(fadePin);
  float fadePos = knob / knobMax;
  if (panTaper == PotTaper::Logarithmic) {
    fadePos = map(exp10f(fadePos), 1, 10, 0, 1);
  }
  // Return current if knob change doesn't exceed threshold
  if (fadePos < appState.fade + 0.05 && fadePos > appState.fade - 0.05) {
    return;
  }
  Serial.print("Fade: ");
  Serial.println(fadePos);
  appState.fade = fadePos;
}

// Adjust the mic gain in response to the peak level
// TODO Implement or delete this
void adjustMicLevel() {}

void readPan(AppState &appState) {
  int knob = analogRead(panPin);
  float panPos = knob / knobMax;
  if (panTaper == PotTaper::Logarithmic) {
    panPos = map(exp10f(panPos), 1, 10, 0, 1);
  }
  // Return current if knob change doesn't exceed threshold
  if (panPos < appState.pan + 0.05 && panPos > appState.pan - 0.05) {
    return;
  }
  Serial.print("Pan: ");
  Serial.println(panPos - 0.5);
  appState.pan = panPos;
}

// Read the volume knob position (analog input A1)
void readVolume(AppState &appState) {
  int knob = analogRead(volumePin);
  float volPos = knob / knobMax;
  if (volumeTaper == PotTaper::Logarithmic) {
    volPos = map(exp10f(volPos), 1, 10, 0, 1);
  }
  // Return current if knob change doesn't exceed threshold
  if (volPos < appState.volume + 0.05 && volPos > appState.volume - 0.05) {
    return;
  }
  Serial.print("Volume: ");
  Serial.println(volPos);
  appState.volume = volPos;
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
#ifdef USE_USB_INPUT
  Serial.println("Using USB input");
#else
  interface.inputSelect(input);
  if (input == AUDIO_INPUT_MIC) {
    interface.micGain(25);
  } else {
    interface.lineInLevel(2);
  }
#endif

#ifdef USE_USB_OUTPUT
  Serial.println("Using USB output");
#else
  interface.volume(0.5);
#endif
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

#ifdef DEBUG
  // Wait for Serial Monitor to open
  while (!Serial)
    ;
  delay(100);
  if (CrashReport) {
    Serial.println(CrashReport);
    CrashReport.clear();
  }
#endif
}

void monitorAudioEngine() {
  static elapsedMillis ms;
  if (ms > 10000) {
    Serial.print("Proc: ");
    Serial.print(AudioProcessorUsage());
    Serial.print(" (");
    Serial.print(AudioProcessorUsageMax());
    Serial.print("),  Mem: ");
    Serial.print(AudioMemoryUsage());
    Serial.print(" (");
    Serial.print(AudioMemoryUsageMax());
    Serial.println(")");
    ms = 0;
  }
}

void readButtons(Buttons &buttons) {
  buttons.record.update();
  buttons.stop.update();
  buttons.play.update();
}

// Read and display stereo input or output channels
void showLevels(
    AudioAnalyzePeak *peakL, AudioAnalyzePeak *peakR, elapsedMillis *ms,
    const char *label
) {
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
    Serial.print(label);
    Serial.println(buf);
  }
}

void showInputLevels(AudioAnalyzePeak *peakL, AudioAnalyzePeak *peakR) {
  static elapsedMillis ms;
  showLevels(peakL, peakR, &ms, "Input level:  ");
}

void showOutputLevels(AudioAnalyzePeak *peakL, AudioAnalyzePeak *peakR) {
  static elapsedMillis ms;
  showLevels(peakL, peakR, &ms, "Output level: ");
}
