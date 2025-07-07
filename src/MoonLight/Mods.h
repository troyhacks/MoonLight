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

#pragma once

#if FT_MOONLIGHT

#include <WLED-sync.h> // https://github.com/netmindz/WLED-sync
#include <WiFi.h>

//alphabetically from here

#define MAX_FREQUENCY   11025          // sample frequency / 2 (as per Nyquist criterion)

//data shared between nodes
static struct Audio {
  uint8_t bands[16]= {0}; // Our calculated freq. channel result table to be used by effects
  float volume; // either sampleAvg or sampleAgc depending on soundAgc; smoothed sample
  float majorPeak; // FFT: strongest (peak) frequency
} audio;


#include "Nodes.h" //needed here because of Mods.cpp includes Mods.h, otherwise Node unknown

class ArtNetMod: public Node {
  public:

  static const char * name() {return "ArtNet ☸️";}
  static uint8_t dim() {return _3D;}
  static const char * tags() {return "";}

  uint16_t controllerIP3 = 11;
  uint16_t throttleSpeed = 50; //default 50ms

  void setup() {
    addControl(controllerIP3, "controllerIP", "number");
    addControl(throttleSpeed, "throttleSpeed", "number");
  }

  const size_t ART_NET_HEADER_SIZE = 12;
  const uint8_t ART_NET_HEADER[12] = {0x41,0x72,0x74,0x2d,0x4e,0x65,0x74,0x00,0x00,0x50,0x00,0x0e};

  IPAddress controllerIP; //tbd: controllerIP also configurable from fixtures and artnet instead of pin output
  std::vector<uint16_t> hardware_outputs = {1024,1024,1024,1024,1024,1024,1024,1024};
  std::vector<uint16_t> hardware_outputs_universe_start = { 0,7,14,21,28,35,42,49 }; //7*170 = 1190 leds => last universe not completely used
  size_t sequenceNumber = 0;

  unsigned long lastMillis = 0;

  void loop() override;
};

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
      ESP_LOGD(TAG, "AudioSync: Initialized");
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


class FastLEDDriverMod: public Node {
  public:

  static const char * name() {return "FastLEDDriver ☸️";}
  static uint8_t dim() {return _3D;}
  static const char * tags() {return "";}

  uint16_t maxPowerSaved = 0;
  uint16_t maxPower = 10;

  void setup() override;
  void loop() override;

  void addLayout() override;
};

class HUB75DriverMod: public Node {
  public:

  static const char * name() {return "HUB75Driver ☸️";}
  static uint8_t dim() {return _3D;}
  static const char * tags() {return "";}

  void setup() override;
  void loop() override;

  void addLayout() override;
};

class PhysicalDriverMod: public Node {
  public:

  static const char * name() {return "PhysicalDriver ☸️";}
  static uint8_t dim() {return _3D;}
  static const char * tags() {return "";}

  uint8_t setMaxPowerBrightnessFactor = 90; //tbd: implement driver.setMaxPowerInMilliWatts
  bool initDone = false;
  uint8_t colorOrder;

  void setup() override;
  void loop() override;

  void addLayout() override;
};

class VirtualDriverMod: public Node {
  public:

  static const char * name() {return "VirtualDriver ☸️";}
  static uint8_t dim() {return _3D;}
  static const char * tags() {return "";}

  void setup() override;
  void loop() override;

  void addLayout() override;
};

#endif