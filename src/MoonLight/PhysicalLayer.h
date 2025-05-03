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

#define MAX_CHANNELS 8192*3 //physical leds
#define NUM_LEDS MAX_CHANNELS / 3 //physical leds

#include <Arduino.h>
#include <vector>
#include "FastLED.h"
#include "../MoonBase/Utilities.h"

// #include "VirtualLayer.h"

class VirtualLayer; //Forward as PhysicalLayer refers back to VirtualLayer
class Node; //Forward as PhysicalLayer refers back to Node
class Modifier; //Forward as PhysicalLayer refers back to Modifier

enum ChannelType {
  ct_Leds,
  ct_LedsRGBW,
  ct_Position,
  ct_Channels,
  ct_MovingHead,
  ct_CrazyCurtain,
  ct_count
};

struct CRGBW {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
  uint8_t white;
};

struct MovingHead {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
  uint8_t pan;
  uint8_t tilt;
  uint8_t roll;
  byte channels[9]; //dummy
};

struct CrazyCurtain {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
  byte ccp[3];
};

struct LightsHeader {
  uint8_t type = ct_Leds; //default
  uint8_t ledFactor = 1;
  uint8_t ledSize = 4; //mm
  uint8_t dummy1;
  uint16_t nrOfLights = 256;
  Coord3D size = {16,16,1}; //not 0,0,0 to prevent div0 eg in Octopus2D
  uint8_t dummy2[4];
}; // fill with dummies to make size 24

struct Lights {
  LightsHeader header;
  union {
    CRGB leds[NUM_LEDS];
    CRGBW ledsRGBW[MAX_CHANNELS / sizeof(CRGBW)];
    byte channels[MAX_CHANNELS];
    MovingHead movingHeads[MAX_CHANNELS / sizeof(MovingHead)];
    CrazyCurtain crazyCurtain[MAX_CHANNELS / sizeof(CrazyCurtain)]; // 6 bytes
    Coord3D positions[MAX_CHANNELS / sizeof(Coord3D)]; //for layout / pass == 1, send positions to monitor / preview
  };
  // std::vector<size_t> universes; //tells at which byte the universe starts
};

//contains the Lights structure/definition and implements layout functions (add*, modify*)
class PhysicalLayer {

    public:

    Lights lights; //the physical lights

    std::vector<bool> lightsToBlend; //this is a 1-bit vector !!! overlapping effects will blend
    uint8_t globalBlend = 128; // to do add as UI control...

    std::vector<VirtualLayer *> layerV; // the virtual layers using this physical layer 

    PhysicalLayer();

    bool setup();
    bool loop();

    
    uint8_t pass = 0; //'class global' so addLight/Pin functions know which pass it is in
    void addLayoutPre();
    void addPin(uint8_t pinNr);
    void addLight(Coord3D position);
    void addLayoutPost();

    // an effect is using a virtual layer: tell the effect in which layer to run...

    //run one loop of an effect
    Node *addNode(const char * animation, uint8_t index);
    void removeNode(Node * node);

    // to be called in setup, if more then one effect
    void initLightsToBlend();

};

#endif //FT_MOONLIGHT