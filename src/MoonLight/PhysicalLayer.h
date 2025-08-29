/**
    @title     MoonLight
    @file      PhysicalLayer.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/moonlight/overview/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact us for more information.
**/

#pragma once

#if FT_MOONLIGHT

#include <Arduino.h>
#include <vector>
#include "FastLED.h"
#include "../MoonBase/Utilities.h"

// #include "VirtualLayer.h"

class VirtualLayer; //Forward as PhysicalLayer refers back to VirtualLayer
class Node; //Forward as PhysicalLayer refers back to Node
class Modifier; //Forward as PhysicalLayer refers back to Modifier

struct LightsHeader {
  Coord3D size = Coord3D(16,16,1); //0 max position of light, counted by addLayoutPre/Post and addLight. 12 bytes not 0,0,0 to prevent div0 eg in Octopus2D
  uint16_t nrOfLights = 256; //4 nr of physical lights, counted by addLight
  struct {                 //condensed rgb
    uint8_t isPositions: 2 = 0;
    uint8_t offsetRed:2 = 1; //GRB is default
    uint8_t offsetGreen:2 = 0;
    uint8_t offsetBlue:2 = 2;
  }; //8 bits
  // uint8_t isPositions = 0; //6 is the lights.positions array filled with positions
  uint8_t brightness; //7 brightness set by light control (sent to LEDs driver normally)
  uint8_t red = 255; //8 brightness set by light control (sent to LEDs driver normally)
  uint8_t green = 255; //9 brightness set by light control (sent to LEDs driver normally)
  uint8_t blue = 255; //10 brightness set by light control (sent to LEDs driver normally)
  uint8_t channelsPerLight = 3; //11 RGB default
  uint8_t offsetRGB = 0; //12 RGB default
  uint8_t offsetWhite = UINT8_MAX;
  uint8_t offsetBrightness = UINT8_MAX; //in case the light has a separate brightness channel
  uint8_t offsetPan = UINT8_MAX;
  uint8_t offsetTilt = UINT8_MAX;
  uint8_t offsetZoom = UINT8_MAX;
  uint8_t offsetRotate = UINT8_MAX;
  uint8_t offsetGobo = UINT8_MAX;
  uint8_t offsetRGB1 = UINT8_MAX;
  uint8_t offsetRGB2 = UINT8_MAX;
  uint8_t offsetRGB3 = UINT8_MAX;
  uint8_t offsetBrightness2 = UINT8_MAX;
  //19 bytes until here
  // uint8_t ledFactor = 1; //factor to multiply the positions with 
  // uint8_t ledSize = 4; //mm size of each light, used in monitor ...
  //24 bytes total !!! so full ATM

  //support for more channels, like white, pan, tilt etc.

  void resetOffsets() {
    channelsPerLight = 3; //RGB default
    offsetRGB = 0;
    offsetRed = 1;
    offsetGreen = 0;
    offsetBlue = 2;
    offsetWhite = UINT8_MAX;
    offsetBrightness = UINT8_MAX;
    offsetPan = UINT8_MAX;
    offsetTilt = UINT8_MAX;
    offsetZoom = UINT8_MAX;
    offsetRotate = UINT8_MAX;
    offsetGobo = UINT8_MAX;
    offsetRGB1 = UINT8_MAX;
    offsetRGB2 = UINT8_MAX;
    offsetRGB3 = UINT8_MAX;
    offsetBrightness2 = UINT8_MAX;
  }

}; // fill with dummies to make size 24, be aware of padding so do not change order of vars

struct Lights {
  LightsHeader header;
  uint8_t *channels = nullptr; // //pka leds, created in constructor
  size_t nrOfChannels = 0;

  // std::vector<size_t> universes; //tells at which byte the universe starts
};

struct SortedPin {
  uint16_t startLed;
  uint16_t nrOfLights;
  uint8_t pin;
};

//contains the Lights structure/definition and implements layout functions (add*, modify*)
class PhysicalLayer {

    public:

    Lights lights; //the physical lights

    // std::vector<bool> lightsToBlend; //this is a 1-bit vector !!! overlapping effects will blend
    // uint8_t globalBlend = 128; // to do add as UI control...

    std::vector<VirtualLayer *, PSRAMAllocator<VirtualLayer *>> layerV; // the virtual layers using this physical layer 

    CRGBPalette16 palette = PartyColors_p;

    uint8_t requestMapPhysical = false; //collect requests to map as it is requested by setup and updateControl and only need to be done once
    uint8_t requestMapVirtual = false; //collect requests to map as it is requested by setup and updateControl and only need to be done once

    std::vector<Node *, PSRAMAllocator<Node *>> nodes;

    uint16_t indexP = 0;

    PhysicalLayer();

    void setup();
    void loop();
    void loopDrivers();

    //mapLayout calls addLayoutPre, addLayout for each node and addLayoutPost and expects pass to be set (1 or 2)
    void mapLayout();

    uint8_t pass = 0; //'class global' so addLight/Pin functions know which pass it is in
    void addLayoutPre();
    void addLight(Coord3D position);
    void addPin(uint8_t pinNr);
    void addLayoutPost();
    
    std::vector<SortedPin> sortedPins;

    // an effect is using a virtual layer: tell the effect in which layer to run...

    void removeNode(Node * node);

    // to be called in setup, if more then one effect
    // void initLightsToBlend();

};

#endif //FT_MOONLIGHT