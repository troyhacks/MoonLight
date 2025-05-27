/**
    @title     MoonLight
    @file      LedsDriver.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/general/utilities/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
**/

#if FT_MOONLIGHT
#pragma once

#include <Arduino.h>
#include <vector>

struct SortedPin {
  uint16_t startLed;
  uint16_t nrOfLights;
  uint8_t pin;
};

struct Lights; //forward

class LedsDriver {
public:

  //clockless driver (check FastLED support...)
  uint8_t colorOrder = 3; //GRB is default for WS2812 (not for FastLED yet: see pio.ini)
  //for virtual driver (but keep enabled to avoid compile errors when used in non virtual context
  uint8_t clockPin = 3; //3 for S3, 26 for ESP32 (wrover)
  uint8_t latchPin = 46; //46 for S3, 27 for ESP32 (wrover)
  uint8_t clockFreq = 10; //clockFreq==10?clock_1000KHZ:clockFreq==11?clock_1111KHZ:clockFreq==12?clock_1123KHZ:clock_800KHZ
                // 1.0MHz is default and runs well (0.8MHz is normal non overclocking). higher then 1.0 is causing flickering at least at ewowi big screen
  uint8_t dmaBuffer = 30; //not used yet

  uint8_t setMaxPowerBrightnessFactor = 90; //tbd: implement driver.setMaxPowerInMilliWatts

  void init(Lights &lights, const std::vector<SortedPin> &sortedPins);

  void setBrightness(uint8_t brightness);

  void setColorCorrection(uint8_t red, uint8_t green, uint8_t blue);

  void show();

};

#endif //FT_MOONLIGHT