/**
    @title     MoonLight
    @file      Drivers.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/moonlight/overview/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact us for more information.
**/

#pragma once

#if FT_MOONLIGHT

//alphabetically from here

#include "../Nodes.h" //needed here because of Driver.cpp includes Driver.h, otherwise Node unknown

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
  uint8_t lightPreset = 2; //GRB

  public:

  void setup() override;

  void loop() override;

  void reOrderAndDimRGBW(uint8_t *packetRGBChannel, uint8_t *lightsRGBChannel);

  //called in addControl (oldValue = "") and in NodeManager onUpdate nodes[i].control[j]
  void onUpdate(String &oldValue, JsonObject control) override;
};

#define ARTNET_CHANNELS_PER_PACKET 512
static const uint8_t ART_NET_HEADER[] = {0x41,0x72,0x74,0x2d,0x4e,0x65,0x74,0x00,0x00,0x50,0x00,0x0e};
//0..7: Array of 8 characters, the final character is a null termination. Value = 'A' 'r' 't' '-' 'N' 'e' 't' 0x00
//8-9: OpOutput Transmitted low byte first (so 0x50, 0x00)
//10: High byte of the Art-Net protocol revision number
//11: Low byte of the Art-Net protocol revision number. Current value 14
#include <AsyncUDP.h>

class ArtNetDriver: public DriverNode {
  public:

  static const char * name() {return "Art-Net Driver ☸️";}
  static uint8_t dim() {return _3D;}
  static const char * tags() {return "";}

  uint16_t controllerIP3 = 11;
  uint16_t port = 6454;
  uint16_t FPSLimiter = 50; //default 50ms
  uint16_t nrOfOutputs = 1; //max 12 on Art-Net LED Controller
  uint16_t channelsPerOutput = 1024; //3096 (1024x3) on Art-Net LED Controller {1024,1024,1024,1024,1024,1024,1024,1024};
  uint16_t universesPerOutput = 2; //7 on on Art-Net LED Controller { 0,7,14,21,28,35,42,49 }

  void setup() override;

  //loop variables:
  IPAddress controllerIP; //tbd: controllerIP also configurable from fixtures and Art-Net instead of pin output
  unsigned long lastMillis = millis();
  unsigned long wait;
  uint8_t packet_buffer[sizeof(ART_NET_HEADER) + 6 + ARTNET_CHANNELS_PER_PACKET];
  uint_fast16_t packetSize;
  size_t sequenceNumber = 0; // this needs to be shared across all outputs
  AsyncUDP artnetudp;// AsyncUDP so we can just blast packets.
  
  uint_fast16_t universe = 0;
  uint_fast16_t channels_remaining;

  bool writePackage();
  void loop() override;
};

class FastLEDDriver: public Node {
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
    bool usesI2S = false;
  #endif
  #if FASTLED_RMT5
    bool usesRMT5 = true;
  #else
    bool usesRMT5 = false;
  #endif

  void setup() override;
  void loop() override;

  bool hasLayout() const override { return true; }
  void onLayout() override;

  void onUpdate(String &oldValue, JsonObject control) override;

};

class HUB75Driver: public Node {
  public:

  static const char * name() {return "HUB75 Driver ☸️🚧";}
  static uint8_t dim() {return _3D;}
  static const char * tags() {return "";}

  void setup() override;
  void loop() override;

  bool hasLayout() const override { return true; }
  void onLayout() override;
};

#define NUMSTRIPS 16 //not needed for non virtal... (see transpose...)

class PhysicalDriver: public DriverNode {
  public:

  static const char * name() {return "Physical Driver ☸️";}
  static uint8_t dim() {return _3D;}
  static const char * tags() {return "";}

  uint8_t pins[NUMSTRIPS]; //max 16 pins
  int lengths[NUMSTRIPS];
  int nb_pins=0;

  #if HP_ALL_DRIVERS
    char version[30] = HP_ALL_VERSION;
    #ifndef BOARD_HAS_PSRAM
      uint8_t dmaBuffer = 6;
    #else
      uint8_t dmaBuffer = 75;
    #endif
  #endif

  void setup() override;
  void loop() override;

  bool hasLayout() const override { return true; }
  void onLayout() override;

  ~PhysicalDriver() override;
};

class VirtualDriver: public DriverNode {
  public:

  static const char * name() {return "Virtual Driver ☸️🚧";}
  static uint8_t dim() {return _3D;}
  static const char * tags() {return "";}

  void setup() override;
  void loop() override;

  bool hasLayout() const override { return true; }
  void onLayout() override;
};
class ParlioDriver: public DriverNode {
  public:

  static const char * name() {return "Parallel IO Driver ☸️🚧";}
  static uint8_t dim() {return _3D;}
  static const char * tags() {return "";}

  void setup() override;

  bool hasLayout() const override { return true; }
  void onLayout() override;

  void onUpdate(String &oldValue, JsonObject control) override;

  void loop() override;

  ~ParlioDriver() override;
  
};


#endif