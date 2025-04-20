/**
    @title     MoonLight
    @file      PhysicalLayer.h
    @repo      https://github.com/ewowi/MoonBase, submit changes to this file as PRs
    @Authors   https://github.com/ewowi/MoonBase/commits/main
    @Doc       https://ewowi.github.io/MoonBase/general/utilities/
    @Copyright © 2025 Github MoonBase Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
**/

#pragma once

#undef TAG
#define TAG "💫"

#define NUM_LEDS 8192 //physical leds

#include <Arduino.h>
#include <vector>
#include "FastLED.h"
#include "../MoonBase/Utilities.h"

// #include "VirtualLayer.h"

struct Coord3D {
    int x;
    int y;
    int z;
  };
  
  
class VirtualLayer; //Forward as PhysicalLayer refers back to VirtualLayer
class Effect; //Forward as PhysicalLayer refers back to Effect
class Projection; //Forward as PhysicalLayer refers back to Projection

//contains leds array and implements fixture definition functions (add*)
class PhysicalLayer {

    public:

    CRGB leds[NUM_LEDS];
    uint16_t nrOfLeds = 256;

    std::vector<bool> pixelsToBlend; //this is a 1-bit vector !!! overlapping effects will blend
    uint8_t globalBlend = 128; // to do add as UI control...

    uint32_t indexP; //used in fixdef functions.

    std::vector<VirtualLayer *> layerV; // the layers using this fixdef 

    PhysicalLayer();

    static void addPin(uint8_t pinNr);

    void addPixelsPre();

    void addPixel(Coord3D pixel);

    void addPixelsPost();

    // an effect is using a virtual layer: tell the effect in which layer to run...


    //run one loop of an effect
    bool addEffect(const char * animation);
    bool loop();

    // to be called in setup, if more then one effect
    void initPixelsToBlend();

};