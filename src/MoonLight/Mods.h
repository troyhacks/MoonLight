/**
    @title     MoonLight
    @file      Mods.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/general/utilities/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
**/

#if FT_MOONLIGHT

#include <WLED-sync.h> // https://github.com/netmindz/WLED-sync
#include <WiFi.h>

//alphabetically from here

#define MAX_FREQUENCY   11025          // sample frequency / 2 (as per Nyquist criterion)

//data shared between nodes
static struct Audio {
  byte bands[16]= {0}; // Our calculated freq. channel result table to be used by effects
  float volume; // either sampleAvg or sampleAgc depending on soundAgc; smoothed sample
  float majorPeak; // FFT: strongest (peak) frequency
} audio;

class AudioSyncMod: public Node {
  public:

  static const char * name() {return "AudioSync ☸️ ♫";}
  static uint8_t dim() {return _3D;}
  static const char * tags() {return "";}

  WLEDSync sync;
  bool init = false;

  void loop() override {
    if (!WiFi.localIP()) return;
    if (!init) {
      sync.begin();
      init = true;
      ESP_LOGW(TAG, "AudioSync: Initialized");
    }
    if (sync.read()) {
      memcpy(audio.bands, sync.fftResult, NUM_GEQ_CHANNELS);
      audio.volume = sync.volumeSmth;
      audio.majorPeak = sync.FFT_MajorPeak;
      // if (audio.bands[0] > 0) {
      //   ESP_LOGD(TAG, "AudioSync: %d %f", audio.bands[0], audio.volume);
      // }
    }
  }
};

static AudioSyncMod *audioNode;


#endif