/**
    @title     MoonBase
    @file      Leds.h
    @repo      https://github.com/ewowi/MoonBase, submit changes to this file as PRs
    @Authors   https://github.com/ewowi/MoonBase/commits/main
    @Doc       https://ewowi.github.io/MoonBase/general/utilities/
    @Copyright © 2025 Github MoonBase Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
**/

#pragma once

#include <Arduino.h>
#include "FastLED.h"

struct Coord3D {
    int x;
    int y;
    int z;
};

class Node {

};

class EffectNode : public Node {

    public:
    
    Coord3D size = {8,8,1}; //not 0,0,0 to prevent div0 eg in Octopus2D

    void fadeToBlackBy(const uint8_t fadeBy) {}
    void setPixelColor(const Coord3D &pixel, const CRGB& color) {}
};

class LedsModel {

    public:

    void addPixel(uint16_t x, uint16_t y, uint16_t z) {
        ESP_LOGD(TAG, "addPixel %d,%d,%d", x, y, z);
    }

};