/**
    @title     MoonLight
    @file      Mods.cpp
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/general/utilities/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
**/

#if FT_MOONLIGHT

#include "Mods.h"

// Expand to , COLOR_ORDER if defined
#if defined(ML_COLOR_ORDER)
  #define COLOR_ORDER_ARG , ML_COLOR_ORDER
#else
  #define COLOR_ORDER_ARG
#endif

// Expand to .setRgbw(RgbwDefault()) if ML_RGBW is 1
#if ML_RGBW
  #define RGBW_CALL .setRgbw(RgbwDefault())
#else
  #define RGBW_CALL
#endif


  void  FastLEDDriverMod::setup() {
    hasLayout = true; //so addLayout is called if needed

    addControl(maxPower, "maxPower", "number", 0, 100);
  }

  void  FastLEDDriverMod::addLayout() {
    if (layerV->layerP->sortedPins.size() == 0) return;

    if (layerV->layerP->pass == 1) { //physical
      ESP_LOGD(TAG, "sortedPins #:%d", layerV->layerP->sortedPins.size());
    for (const SortedPin &sortedPin : layerV->layerP->sortedPins) {
      ESP_LOGD(TAG, "sortpins s:%d #:%d p:%d", sortedPin.startLed, sortedPin.nrOfLights, sortedPin.pin);

      uint16_t startLed = sortedPin.startLed;
      uint16_t nrOfLights = sortedPin.nrOfLights;
      uint16_t pin = sortedPin.pin;

      switch (sortedPin.pin) {
      #if CONFIG_IDF_TARGET_ESP32
        case 0: FastLED.addLeds<ML_CHIPSET, 0 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 1: FastLED.addLeds<ML_CHIPSET, 1 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 2: FastLED.addLeds<ML_CHIPSET, 2 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 3: FastLED.addLeds<ML_CHIPSET, 3 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 4: FastLED.addLeds<ML_CHIPSET, 4 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 5: FastLED.addLeds<ML_CHIPSET, 5 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        // case 6: FastLED.addLeds<ML_CHIPSET, 6 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        // case 7: FastLED.addLeds<ML_CHIPSET, 7 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        // case 8: FastLED.addLeds<ML_CHIPSET, 8 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        // case 9: FastLED.addLeds<ML_CHIPSET, 9 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        // case 10: FastLED.addLeds<ML_CHIPSET, 10 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 11: FastLED.addLeds<ML_CHIPSET, 11 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 12: FastLED.addLeds<ML_CHIPSET, 12 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 13: FastLED.addLeds<ML_CHIPSET, 13 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 14: FastLED.addLeds<ML_CHIPSET, 14 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 15: FastLED.addLeds<ML_CHIPSET, 15 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
    #if !defined(BOARD_HAS_PSRAM) && !defined(ARDUINO_ESP32_PICO)
        // 16+17 = reserved for PSRAM, or reserved for FLASH on pico-D4
        case 16: FastLED.addLeds<ML_CHIPSET, 16 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 17: FastLED.addLeds<ML_CHIPSET, 17 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
    #endif
        case 18: FastLED.addLeds<ML_CHIPSET, 18 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 19: FastLED.addLeds<ML_CHIPSET, 19 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        // case 20: FastLED.addLeds<ML_CHIPSET, 20 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 21: FastLED.addLeds<ML_CHIPSET, 21 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 22: FastLED.addLeds<ML_CHIPSET, 22 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 23: FastLED.addLeds<ML_CHIPSET, 23 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        // case 24: FastLED.addLeds<ML_CHIPSET, 24 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 25: FastLED.addLeds<ML_CHIPSET, 25 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 26: FastLED.addLeds<ML_CHIPSET, 26 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 27: FastLED.addLeds<ML_CHIPSET, 27 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        // case 28: FastLED.addLeds<ML_CHIPSET, 28 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        // case 29: FastLED.addLeds<ML_CHIPSET, 29 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        // case 30: FastLED.addLeds<ML_CHIPSET, 30 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        // case 31: FastLED.addLeds<ML_CHIPSET, 31 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 32: FastLED.addLeds<ML_CHIPSET, 32 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 33: FastLED.addLeds<ML_CHIPSET, 33 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        // 34-39 input-only
        // case 34: FastLED.addLeds<ML_CHIPSET, 34 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        // case 35: FastLED.addLeds<ML_CHIPSET, 35 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        // case 36: FastLED.addLeds<ML_CHIPSET, 36 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        // case 37: FastLED.addLeds<ML_CHIPSET, 37 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        // case 38: FastLED.addLeds<ML_CHIPSET, 38 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        // case 39: FastLED.addLeds<ML_CHIPSET, 39 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
      #endif //CONFIG_IDF_TARGET_ESP32

      #if CONFIG_IDF_TARGET_ESP32S2
        case 0: FastLED.addLeds<ML_CHIPSET, 0 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 1: FastLED.addLeds<ML_CHIPSET, 1 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 2: FastLED.addLeds<ML_CHIPSET, 2 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 3: FastLED.addLeds<ML_CHIPSET, 3 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 4: FastLED.addLeds<ML_CHIPSET, 4 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 5: FastLED.addLeds<ML_CHIPSET, 5 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 6: FastLED.addLeds<ML_CHIPSET, 6 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 7: FastLED.addLeds<ML_CHIPSET, 7 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 8: FastLED.addLeds<ML_CHIPSET, 8 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 9: FastLED.addLeds<ML_CHIPSET, 9 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 10: FastLED.addLeds<ML_CHIPSET, 10 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 11: FastLED.addLeds<ML_CHIPSET, 11 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 12: FastLED.addLeds<ML_CHIPSET, 12 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 13: FastLED.addLeds<ML_CHIPSET, 13 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 14: FastLED.addLeds<ML_CHIPSET, 14 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 15: FastLED.addLeds<ML_CHIPSET, 15 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 16: FastLED.addLeds<ML_CHIPSET, 16 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 17: FastLED.addLeds<ML_CHIPSET, 17 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 18: FastLED.addLeds<ML_CHIPSET, 18 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
    #if !ARDUINO_USB_CDC_ON_BOOT
        // 19 + 20 = USB HWCDC. reserved for USB port when ARDUINO_USB_CDC_ON_BOOT=1
        case 19: FastLED.addLeds<ML_CHIPSET, 19 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 20: FastLED.addLeds<ML_CHIPSET, 20 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
    #endif
        case 21: FastLED.addLeds<ML_CHIPSET, 21 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        // 22 to 32: not connected, or reserved for SPI FLASH
        // case 22: FastLED.addLeds<ML_CHIPSET, 22 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        // case 23: FastLED.addLeds<ML_CHIPSET, 23 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        // case 24: FastLED.addLeds<ML_CHIPSET, 24 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        // case 25: FastLED.addLeds<ML_CHIPSET, 25 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
    #if !defined(BOARD_HAS_PSRAM)
        // 26-32 = reserved for PSRAM
        case 26: FastLED.addLeds<ML_CHIPSET, 26 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        // case 27: FastLED.addLeds<ML_CHIPSET, 27 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        // case 28: FastLED.addLeds<ML_CHIPSET, 28 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        // case 29: FastLED.addLeds<ML_CHIPSET, 29 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        // case 30: FastLED.addLeds<ML_CHIPSET, 30 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        // case 31: FastLED.addLeds<ML_CHIPSET, 31 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        // case 32: FastLED.addLeds<ML_CHIPSET, 32 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
    #endif
        case 33: FastLED.addLeds<ML_CHIPSET, 33 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 34: FastLED.addLeds<ML_CHIPSET, 34 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 35: FastLED.addLeds<ML_CHIPSET, 35 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 36: FastLED.addLeds<ML_CHIPSET, 36 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 37: FastLED.addLeds<ML_CHIPSET, 37 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 38: FastLED.addLeds<ML_CHIPSET, 38 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 39: FastLED.addLeds<ML_CHIPSET, 39 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 40: FastLED.addLeds<ML_CHIPSET, 40 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 41: FastLED.addLeds<ML_CHIPSET, 41 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 42: FastLED.addLeds<ML_CHIPSET, 42 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 43: FastLED.addLeds<ML_CHIPSET, 43 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 44: FastLED.addLeds<ML_CHIPSET, 44 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 45: FastLED.addLeds<ML_CHIPSET, 45 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        // 46 input-only
        // case 46: FastLED.addLeds<ML_CHIPSET, 46 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
      #endif //CONFIG_IDF_TARGET_ESP32S2

      #if CONFIG_IDF_TARGET_ESP32C3
        case 0: FastLED.addLeds<ML_CHIPSET, 0 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 1: FastLED.addLeds<ML_CHIPSET, 1 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 2: FastLED.addLeds<ML_CHIPSET, 2 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 3: FastLED.addLeds<ML_CHIPSET, 3 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 4: FastLED.addLeds<ML_CHIPSET, 4 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 5: FastLED.addLeds<ML_CHIPSET, 5 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 6: FastLED.addLeds<ML_CHIPSET, 6 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 7: FastLED.addLeds<ML_CHIPSET, 7 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 8: FastLED.addLeds<ML_CHIPSET, 8 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 9: FastLED.addLeds<ML_CHIPSET, 9 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 10: FastLED.addLeds<ML_CHIPSET, 10 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        // 11-17 reserved for SPI FLASH
        //case 11: FastLED.addLeds<ML_CHIPSET, 11 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        //case 12: FastLED.addLeds<ML_CHIPSET, 12 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        //case 13: FastLED.addLeds<ML_CHIPSET, 13 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        //case 14: FastLED.addLeds<ML_CHIPSET, 14 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        //case 15: FastLED.addLeds<ML_CHIPSET, 15 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        //case 16: FastLED.addLeds<ML_CHIPSET, 16 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        //case 17: FastLED.addLeds<ML_CHIPSET, 17 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
    #if !ARDUINO_USB_CDC_ON_BOOT
        // 18 + 19 = USB HWCDC. reserved for USB port when ARDUINO_USB_CDC_ON_BOOT=1
        case 18: FastLED.addLeds<ML_CHIPSET, 18 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 19: FastLED.addLeds<ML_CHIPSET, 19 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
    #endif
        // 20+21 = Serial RX+TX --> don't use for LEDS when serial-to-USB is needed
        case 20: FastLED.addLeds<ML_CHIPSET, 20 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 21: FastLED.addLeds<ML_CHIPSET, 21 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
      #endif //CONFIG_IDF_TARGET_ESP32S2

      #if CONFIG_IDF_TARGET_ESP32S3
        case 0: FastLED.addLeds<ML_CHIPSET, 0 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 1: FastLED.addLeds<ML_CHIPSET, 1 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 2: FastLED.addLeds<ML_CHIPSET, 2 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 3: FastLED.addLeds<ML_CHIPSET, 3 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 4: FastLED.addLeds<ML_CHIPSET, 4 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 5: FastLED.addLeds<ML_CHIPSET, 5 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 6: FastLED.addLeds<ML_CHIPSET, 6 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 7: FastLED.addLeds<ML_CHIPSET, 7 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 8: FastLED.addLeds<ML_CHIPSET, 8 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 9: FastLED.addLeds<ML_CHIPSET, 9 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 10: FastLED.addLeds<ML_CHIPSET, 10 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 11: FastLED.addLeds<ML_CHIPSET, 11 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 12: FastLED.addLeds<ML_CHIPSET, 12 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 13: FastLED.addLeds<ML_CHIPSET, 13 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 14: FastLED.addLeds<ML_CHIPSET, 14 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 15: FastLED.addLeds<ML_CHIPSET, 15 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 16: FastLED.addLeds<ML_CHIPSET, 16 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 17: FastLED.addLeds<ML_CHIPSET, 17 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 18: FastLED.addLeds<ML_CHIPSET, 18 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
      #if !ARDUINO_USB_CDC_ON_BOOT
        // 19 + 20 = USB-JTAG. Not recommended for other uses.
        case 19: FastLED.addLeds<ML_CHIPSET, 19 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 20: FastLED.addLeds<ML_CHIPSET, 20 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
      #endif
        case 21: FastLED.addLeds<ML_CHIPSET, 21 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        // // 22 to 32: not connected, or SPI FLASH
        // case 22: FastLED.addLeds<ML_CHIPSET, 22 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        // case 23: FastLED.addLeds<ML_CHIPSET, 23 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        // case 24: FastLED.addLeds<ML_CHIPSET, 24 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        // case 25: FastLED.addLeds<ML_CHIPSET, 25 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        // case 26: FastLED.addLeds<ML_CHIPSET, 26 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        // case 27: FastLED.addLeds<ML_CHIPSET, 27 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        // case 28: FastLED.addLeds<ML_CHIPSET, 28 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        // case 29: FastLED.addLeds<ML_CHIPSET, 29 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        // case 30: FastLED.addLeds<ML_CHIPSET, 30 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        // case 31: FastLED.addLeds<ML_CHIPSET, 31 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        // case 32: FastLED.addLeds<ML_CHIPSET, 32 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
      #if !defined(BOARD_HAS_PSRAM)
        // 33 to 37: reserved if using _octal_ SPI Flash or _octal_ PSRAM
        case 33: FastLED.addLeds<ML_CHIPSET, 33 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 34: FastLED.addLeds<ML_CHIPSET, 34 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 35: FastLED.addLeds<ML_CHIPSET, 35 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 36: FastLED.addLeds<ML_CHIPSET, 36 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 37: FastLED.addLeds<ML_CHIPSET, 37 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
      #endif
        case 38: FastLED.addLeds<ML_CHIPSET, 38 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 39: FastLED.addLeds<ML_CHIPSET, 39 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 40: FastLED.addLeds<ML_CHIPSET, 40 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 41: FastLED.addLeds<ML_CHIPSET, 41 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 42: FastLED.addLeds<ML_CHIPSET, 42 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        // 43+44 = Serial RX+TX --> don't use for LEDS when serial-to-USB is needed
        case 43: FastLED.addLeds<ML_CHIPSET, 43 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 44: FastLED.addLeds<ML_CHIPSET, 44 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 45: FastLED.addLeds<ML_CHIPSET, 45 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 46: FastLED.addLeds<ML_CHIPSET, 46 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 47: FastLED.addLeds<ML_CHIPSET, 47 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        case 48: FastLED.addLeds<ML_CHIPSET, 48 COLOR_ORDER_ARG>(layerV->layerP->lights.leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
      #endif //CONFIG_IDF_TARGET_ESP32S3

      default: ESP_LOGW(TAG, "FastLEDPin assignment: pin not supported %d", sortedPin.pin);
      } //switch pinNr

    } //sortedPins    
    }

    FastLED.setMaxPowerInMilliWatts(1000 * maxPower); // 5v, 2000mA, to protect usb while developing

  }

  void  FastLEDDriverMod::loop() {
    if (FastLED.count()) {
      if (FastLED.getBrightness() != layerV->layerP->lights.header.brightness)
        FastLED.setBrightness(layerV->layerP->lights.header.brightness);

      if (maxPowerSaved != maxPower) {
        FastLED.setMaxPowerInMilliWatts(1000 * maxPower); // 5v, 2000mA, to protect usb while developing
        maxPowerSaved = maxPower;
      }

      //FastLED Led Controllers
      CRGB correction = CRGB(layerV->layerP->lights.header.red, layerV->layerP->lights.header.green, layerV->layerP->lights.header.blue);
      CLEDController *pCur = CLEDController::head();
      while( pCur) {
          // ++x;
          if (pCur->getCorrection() != correction)
            pCur->setCorrection(correction);
          // pCur->size();
          pCur = pCur->next();
      }

      FastLED.show();
    }
  }



  void HUB75DriverMod::setup() {
    hasLayout = true; //so addLayout is called if needed
  }
  void HUB75DriverMod::addLayout() {
  }
  void HUB75DriverMod::loop() {
  }

  #if CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32S2
    #include "I2SClockLessLedDriveresp32s3.h"
    static I2SClocklessLedDriveresp32S3 driverP; //    sizeof(driver) = 1080K !
  #elif CONFIG_IDF_TARGET_ESP32
    #include "I2SClocklessLedDriver.h"
    static I2SClocklessLedDriver driverP;
  #endif

  void  PhysicalDriverMod::setup() {

    hasLayout = true; //so addLayout is called if needed

    JsonObject property = addControl(colorOrder, "colorOrder", "select"); 
    JsonArray values = property["values"].to<JsonArray>();
    values.add("GRBW");
    values.add("RGB");
    values.add("RBG");
    values.add("GRB");
    values.add("GBR");
    values.add("BRG");
    values.add("BGR");
  }

  void PhysicalDriverMod::addLayout() {
    if (initDone || layerV->layerP->sortedPins.size() == 0) return;

    if (layerV->layerP->pass == 1) { //physical
      initDone = true;
      ESP_LOGD(TAG, "sortedPins #:%d", layerV->layerP->sortedPins.size());

  #if CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32
      int pins[NUMSTRIPS]; //max 16 pins
      int lengths[NUMSTRIPS];
      int nb_pins=0;

      for (const SortedPin &sortedPin : layerV->layerP->sortedPins) {
        ESP_LOGD(TAG, "sortedPin s:%d #:%d p:%d", sortedPin.startLed, sortedPin.nrOfLights, sortedPin.pin);
        if (nb_pins < NUMSTRIPS) {
          pins[nb_pins] = sortedPin.pin;
          lengths[nb_pins] = sortedPin.nrOfLights;
          nb_pins++;
        }
      }

      //fill the rest of the pins with the pins already used
      //preferably NUMSTRIPS is a variable...
      if (nb_pins > 0) {
        for (uint8_t i = nb_pins; i < NUMSTRIPS; i++) {
          pins[i] = pins[i%nb_pins];
          lengths[i] = 0;
        }
      }
      ESP_LOGD(TAG, "pins[");
      for (int i=0; i<nb_pins; i++) {
        ESP_LOGD(TAG, ", %d", pins[i]);
      }
      ESP_LOGD(TAG, "]\n");

      if (nb_pins > 0) {
        #if CONFIG_IDF_TARGET_ESP32S3 | CONFIG_IDF_TARGET_ESP32S2
          driverP.initled((uint8_t*) layerV->layerP->lights.leds, pins, nb_pins, lengths[0]); //s3 doesn't support lengths so we pick the first
          // colorOrder ??
          //void initled( uint8_t * leds, int * pins, int numstrip, int NUM_LED_PER_STRIP)
        #else
          driverP.initled((uint8_t*) layerV->layerP->lights.leds, pins, lengths, nb_pins, (colorarrangment)colorOrder);
          #if ML_LIVE_MAPPING
            driver.setMapLed(&mapLed);
          #endif
          //void initled(uint8_t *leds, int *Pinsq, int *sizes, int num_strips, colorarrangment cArr)
        #endif
      //   Variable("Fixture", "brightness").triggerEvent(onChange, UINT8_MAX, true); //set brightness (init is true so bri value not send via udp)
        driverP.setBrightness(setMaxPowerBrightnessFactor / 256); //not brighter then the set limit (WIP)
      }
      #endif
    }
  }

  void PhysicalDriverMod::loop() {
    if (!initDone) return;

    if (layerV->layerP->lights.header.isPositions == 0) {

  #if CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32
      if (driverP._brightness != layerV->layerP->lights.header.brightness)
        driverP.setBrightness(layerV->layerP->lights.header.brightness * setMaxPowerBrightnessFactor >> 8);

      #if CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32S2
        if (driverP.ledsbuff != nullptr)
          driverP.show();
      #else
        if (driverP.total_leds > 0)
          driverP.showPixels(WAIT);
      #endif
      #endif
    }
  }

  // #include "I2SClocklessVirtualLedDriver.h"
  // static I2SClocklessVirtualLedDriver driverV;

  void VirtualDriverMod::setup() {
    hasLayout = true; //so addLayout is called if needed
  }
  void VirtualDriverMod::addLayout() {
  }
  void VirtualDriverMod::loop() {
  }

#endif