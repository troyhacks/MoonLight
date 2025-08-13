/**
    @title     MoonLight
    @file      Mods.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/moonlight/overview/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact us for more information.
**/

#pragma once

#if FT_MOONLIGHT

#include <WLED-sync.h> // https://github.com/netmindz/WLED-sync
#include <WiFi.h>

//alphabetically from here

//data shared between nodes
static struct SharedData {
  uint8_t bands[16]= {0}; // Our calculated freq. channel result table to be used by effects
  float volume; // either sampleAvg or sampleAgc depending on soundAgc; smoothed sample
  float majorPeak; // FFT: strongest (peak) frequency
  uint16_t fps;
  uint8_t connectionStatus;
  size_t connectedClients;
  size_t clientListSize;
} sharedData;

#include "Nodes.h" //needed here because of Mods.cpp includes Mods.h, otherwise Node unknown

class DriverNode: public Node {
  uint16_t maxPower = 10;
  uint8_t brightnessSaved = UINT8_MAX;
  
  protected:
  
  bool lightPresetSaved = false; //initLeds can only start if this has been saved

  #if HP_ALL_DRIVERS
    CRGB savedColorCorrection;
    bool initDone = false;
  #endif

  protected:
  uint8_t lightPreset = 3;

  public:

  void setup() override;

  void loop() override;

  void reOrderAndDimRGBW(uint8_t *packetRGBChannel, uint8_t *lightsRGBChannel);

  void updateControl(JsonObject control) override;
};

class ArtNetDriverMod: public DriverNode {
  public:

  static const char * name() {return "Art-Net Driver ☸️";}
  static uint8_t dim() {return _3D;}
  static const char * tags() {return "";}

  uint16_t controllerIP3 = 11;
  uint16_t port = 6454;
  uint16_t FPSLimiter = 50; //default 50ms
  uint16_t nrOfOutputs = 1; //8 on Art-Net LED Controller
  uint16_t channelsPerOutput = 768; //3096 (1024x3) on Art-Net LED Controller {1024,1024,1024,1024,1024,1024,1024,1024};
  uint16_t universesPerOutput = 1; //7 on on Art-Net LED Controller { 0,7,14,21,28,35,42,49 }

  void setup() override;

  IPAddress controllerIP; //tbd: controllerIP also configurable from fixtures and Art-Net instead of pin output
  size_t sequenceNumber = 0;

  unsigned long lastMillis = millis();

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
      ESP_LOGV(TAG, "AudioSync: Initialized");
    }
    if (sync.read()) {
      memcpy(sharedData.bands, sync.fftResult, NUM_GEQ_CHANNELS);
      sharedData.volume = sync.volumeSmth;
      sharedData.majorPeak = sync.FFT_MajorPeak;
      // if (audio.bands[0] > 0) {
      //   ESP_LOGV(TAG, "AudioSync: %d %f", audio.bands[0], audio.volume);
      // }
    }
  }
};

static AudioSyncMod *audioNode;


class FastLEDDriverMod: public Node {
  public:

  static const char * name() {return "FastLED Driver ☸️";}
  static uint8_t dim() {return _3D;}
  static const char * tags() {return "";}

  uint16_t maxPower = 10;
  char chipSet[20] = TOSTRING(ML_CHIPSET);
  char version[20] = TOSTRING(FASTLED_VERSION);// "." TOSTRING(FASTLED_VERSION_MINOR) "." TOSTRING(FASTLED_VERSION_PATCH);
  char colorOrder[20] = TOSTRING(ML_COLOR_ORDER);
  #if FASTLED_USES_ESP32S3_I2S
    bool usesI2S = true;
  #else
    bool usesI2S = true;
  #endif

  void setup() override;
  void loop() override;

  void addLayout() override;

  void updateControl(JsonObject control) override;

};

class HUB75DriverMod: public Node {
  public:

  static const char * name() {return "HUB75 Driver ☸️🚧";}
  static uint8_t dim() {return _3D;}
  static const char * tags() {return "";}

  void setup() override;
  void loop() override;

  void addLayout() override;
};

class PhysicalDriverMod: public DriverNode {
  public:

  static const char * name() {return "Physical Driver ☸️";}
  static uint8_t dim() {return _3D;}
  static const char * tags() {return "";}

  char version[30] = HP_ALL_VERSION;

  void setup() override;
  void loop() override;

  void addLayout() override;
};

class VirtualDriverMod: public DriverNode {
  public:

  static const char * name() {return "Virtual Driver ☸️🚧";}
  static uint8_t dim() {return _3D;}
  static const char * tags() {return "";}

  void setup() override;
  void loop() override;

  void addLayout() override;
};

#endif