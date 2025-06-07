/**
    @title     MoonLight
    @file      PhysicalLayer.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/general/utilities/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
**/

#pragma once

#if FT_MOONLIGHT

#undef TAG
#define TAG "💫"

#ifndef MAX_LEDS
  #define MAX_LEDS 8192 //physical leds 
#endif

#define MAX_CHANNELS MAX_LEDS*3 //physical channels

#include <Arduino.h>
#include <vector>
#include "FastLED.h"
#include "../MoonBase/Utilities.h"

// #include "VirtualLayer.h"

#include "LedsDriver.h"

class VirtualLayer; //Forward as PhysicalLayer refers back to VirtualLayer
class Node; //Forward as PhysicalLayer refers back to Node
class Modifier; //Forward as PhysicalLayer refers back to Modifier

struct LightsHeader {
  Coord3D size = {16,16,1}; //max position of light, counted by addLayoutPre/Post and addLight. 12 bytes not 0,0,0 to prevent div0 eg in Octopus2D
  uint16_t nrOfLights = 256; //nr of physical lights, counted by addLight
  uint8_t brightness; //brightness set by light control
  uint8_t channelsPerLight = 3; //RGB default
  uint8_t offsetRGB = 0; //RGB default
  uint8_t offsetPan = -1;
  uint8_t offsetTilt = -1;
  uint8_t offsetZoom = -1;
  uint8_t offsetBrightness = -1;
  uint8_t isPositions = 0; //is the lights.positions array filled with positions
  // uint8_t ledFactor = 1; //factor to multiply the positions with 
  // uint8_t ledSize = 4; //mm size of each light, used in monitor ...
  uint8_t dummy2[2];

  //support for more channels, like white, pan, tilt etc.

  void resetOffsets() {
    channelsPerLight = 3; //RGB default
    offsetRGB = 0;
    offsetPan = -1;
    offsetTilt = -1;
    offsetZoom = -1;
    offsetBrightness = -1;
  }

  void setRGB(uint8_t *light, const CRGB &color) {
    light[offsetRGB] = color.red;
    light[offsetRGB + 1] = color.green;
    light[offsetRGB + 2] = color.blue;
  }
  void setWhite(uint8_t *light, const uint8_t white) {
    if (offsetRGB != -1)
      light[offsetRGB + 3] = white;
  }
  void setPan(uint8_t *light, const uint8_t pan) {
    if (offsetPan != -1)
      light[offsetPan] = pan;
  }
  void setTilt(uint8_t *light, const uint8_t tilt) {
    if (offsetTilt != -1)
     light[offsetTilt] = tilt;
  }
  void setZoom(uint8_t *light, const uint8_t zoom) {
    if (offsetZoom != -1)
     light[offsetZoom] = zoom;
  }
  void setBrightness(uint8_t *light, const uint8_t brightness) {
    if (offsetBrightness != -1)
      light[offsetBrightness] = brightness;
  }
}; // fill with dummies to make size 24, be aware of padding so do not change order of vars

struct Lights {
  LightsHeader header;
  union {
    CRGB leds[MAX_LEDS];
    uint8_t channels[MAX_CHANNELS];
    Coord3D positions[MAX_CHANNELS / sizeof(Coord3D)]; //for layout / pass == 1, send positions to monitor / preview
  };
  // std::vector<size_t> universes; //tells at which byte the universe starts
};

//contains the Lights structure/definition and implements layout functions (add*, modify*)
class PhysicalLayer {

    public:

    Lights lights; //the physical lights

    // std::vector<bool> lightsToBlend; //this is a 1-bit vector !!! overlapping effects will blend
    // uint8_t globalBlend = 128; // to do add as UI control...

    std::vector<VirtualLayer *> layerV; // the virtual layers using this physical layer 

    PhysicalLayer();

    bool setup();
    bool loop();

    uint8_t pass = 0; //'class global' so addLight/Pin functions know which pass it is in
    void addLayoutPre();
    void addLight(Coord3D position);
    void addPin(uint8_t pinNr);
    void addLayoutPost();
    
    std::vector<SortedPin> sortedPins;
    LedsDriver ledsDriver; 

    // an effect is using a virtual layer: tell the effect in which layer to run...

    //run one loop of an effect
    Node *addNode(const uint8_t index, const char * animation, const JsonArray controls);
    void removeNode(Node * node);

    // to be called in setup, if more then one effect
    // void initLightsToBlend();

};

#endif //FT_MOONLIGHT