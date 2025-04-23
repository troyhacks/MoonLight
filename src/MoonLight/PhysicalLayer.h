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
    //Minus / delta (abs)
    Coord3D operator-(const Coord3D rhs) const {
        return Coord3D{x - rhs.x, y - rhs.y, z - rhs.z};;
      }
      Coord3D operator+(const Coord3D rhs) const {
        return Coord3D{x + rhs.x, y + rhs.y, z + rhs.z};
      }
      Coord3D operator*(const Coord3D rhs) const {
        return Coord3D{x * rhs.x, y * rhs.y, z * rhs.z};
      }
      Coord3D operator/(const Coord3D rhs) const {
        return Coord3D{x / rhs.x, y / rhs.y, z / rhs.z};
      }
      Coord3D operator%(const Coord3D rhs) const {
        return Coord3D{x % rhs.x, y % rhs.y, z % rhs.z};
      }
    };
  
  
class VirtualLayer; //Forward as PhysicalLayer refers back to VirtualLayer
class Node; //Forward as PhysicalLayer refers back to Node
class Projection; //Forward as PhysicalLayer refers back to Projection

//contains leds array and implements fixture definition functions (add*)
class PhysicalLayer {

    public:

    CRGB leds[NUM_LEDS];
    uint16_t nrOfLeds = 256;
    Coord3D size = {16,16,1}; //not 0,0,0 to prevent div0 eg in Octopus2D

    std::vector<bool> pixelsToBlend; //this is a 1-bit vector !!! overlapping effects will blend
    uint8_t globalBlend = 128; // to do add as UI control...

    uint32_t indexP; //used in fixdef functions.

    std::vector<VirtualLayer *> layerV; // the layers using this fixdef 

    PhysicalLayer();

    bool setup();
    bool loop();

    static void addPin(uint8_t pinNr);

    void addPixelsPre();

    void addPixel(Coord3D pixel);

    void addPixelsPost();

    // an effect is using a virtual layer: tell the effect in which layer to run...


    //run one loop of an effect
    bool addNode(const char * animation);

    // to be called in setup, if more then one effect
    void initPixelsToBlend();

};