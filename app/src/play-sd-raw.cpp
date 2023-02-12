#include "play-sd-raw.hpp"
#include <SD.h>
#include <spi_interrupt.h>

namespace App {
bool AudioPlaySdRaw::play(const char *filename, uint32_t offset = 0) {
  stop();
#if defined(HAS_KINETIS_SDHC)
  if (!(SIM_SCGC3 & SIM_SCGC3_SDHC))
    AudioStartUsingSPI();
#else
  AudioStartUsingSPI();
#endif
  __disable_irq();
  rawfile = SD.open(filename);
  rawfile.seek(offset);
  __enable_irq();
  if (!rawfile) {
    Serial.println("Unable to open file");
#if defined(HAS_KINETIS_SDHC)
    if (!(SIM_SCGC3 & SIM_SCGC3_SDHC))
      AudioStopUsingSPI();
#else
    AudioStopUsingSPI();
#endif
    return false;
  }
  file_size = rawfile.size();
  file_offset = offset;
  // Serial.println(offset);
  // Serial.println("able to open file");
  playing = true;
  return true;
}

void AudioPlaySdRaw::begin(void) {
  playing = false;
  file_offset = 0;
  file_size = 0;
}

void AudioPlaySdRaw::stop(void) {
  __disable_irq();
  if (playing) {
    playing = false;
    __enable_irq();
    rawfile.close();
#if defined(HAS_KINETIS_SDHC)
    if (!(SIM_SCGC3 & SIM_SCGC3_SDHC))
      AudioStopUsingSPI();
#else
    AudioStopUsingSPI();
#endif
  } else {
    __enable_irq();
  }
}

void AudioPlaySdRaw::update(void) {
  unsigned int i, n;
  audio_block_t *block;

  // only update if we're playing
  if (!playing)
    return;

  // allocate the audio blocks to transmit
  block = allocate();
  if (block == NULL)
    return;

  if (rawfile.available()) {
    // we can read more data from the file...
    n = rawfile.read(block->data, AUDIO_BLOCK_SAMPLES * 2);
    file_offset += n;
    for (i = n / 2; i < AUDIO_BLOCK_SAMPLES; i++) {
      block->data[i] = 0;
    }
    transmit(block);
  } else {
    rawfile.close();
#if defined(HAS_KINETIS_SDHC)
    if (!(SIM_SCGC3 & SIM_SCGC3_SDHC))
      AudioStopUsingSPI();
#else
    AudioStopUsingSPI();
#endif
    playing = false;
  }
  release(block);
}

#define B2M                                                                    \
  (uint32_t)((double)4294967296000.0 / AUDIO_SAMPLE_RATE_EXACT /               \
             2.0) // 97352592

uint32_t AudioPlaySdRaw::positionMillis(void) {
  return ((uint64_t)file_offset * B2M) >> 32;
}

uint32_t AudioPlaySdRaw::getOffset(void) { return file_offset; }

uint32_t AudioPlaySdRaw::lengthMillis(void) {
  return ((uint64_t)file_size * B2M) >> 32;
}
} // namespace App
