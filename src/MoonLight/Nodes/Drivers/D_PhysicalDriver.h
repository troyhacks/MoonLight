/**
    @title     MoonLight
    @file      PhysicalDriver.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/moonlight/overview/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact us for more information.
**/

#pragma once

#if FT_MOONLIGHT

#include <ESP32SvelteKit.h> // for safeModeMB and restartNeeded

#ifdef CONFIG_IDF_TARGET_ESP32P4
  #include "parlio.h"
#endif

#define NUMSTRIPS 20 //max strips for physical driver

class PhysicalDriver: public DriverNode {
  public:

  static const char * name() {return "Physical Driver";}
  static uint8_t dim() {return _NoD;}
  static const char * tags() {return "☸️";}

  uint8_t pins[NUMSTRIPS];
  uint16_t lengths[NUMSTRIPS];
  int nb_pins = 0;

  #if HP_ALL_DRIVERS
    char version[30] = HP_ALL_VERSION;
    #ifndef BOARD_HAS_PSRAM
      uint8_t dmaBuffer = 6;
    #else
      uint8_t dmaBuffer = 75;
    #endif
  #endif

  void setup() override {
    DriverNode::setup();
    #if HP_ALL_DRIVERS
      addControl(dmaBuffer, "dmaBuffer", "range", 1, 100);
      addControl(version, "Version", "text", 0, 30, true); //read only
    #endif
  }

  void loop() override {
    #if HP_ALL_DRIVERS
      if (!initDone) return;

      if (layer->layerP->lights.header.isPositions == 0) {

        DriverNode::loop();

        #ifndef CONFIG_IDF_TARGET_ESP32P4
          if (ledsDriver.total_leds > 0)
            ledsDriver.showPixels(WAIT);
        #else
          show_parlio(pins, layer->layerP->lights.header.nrOfLights, layer->layerP->lights.channels
                      , ledsDriver._brightness, layer->layerP->lights.header.channelsPerLight == 4, nb_pins, lengths[0]
                      , layer->layerP->lights.header.offsetRed, layer->layerP->lights.header.offsetGreen, layer->layerP->lights.header.offsetBlue
                    );
        #endif

    }
    #else //ESP32_LEDSDRIVER
      if (!ledsDriver.initLedsDone) return;

      if (layer->layerP->lights.header.isPositions == 0) {

          DriverNode::loop();

          ledsDriver.show();
      }
    #endif
  }

  bool hasOnLayout() const override { return true; }
  void onLayout() override {
    #if HP_ALL_DRIVERS
      if (layer->layerP->pass == 1 && !layer->layerP->monitorPass) { //physical

        if (!lightPresetSaved || layer->layerP->sortedPins.size() == 0) { //|| initDone can be done multiple times now...
          MB_LOGD(ML_TAG, "return: lightpresetsaved:%d initDone:%d #:%d", lightPresetSaved , initDone, layer->layerP->sortedPins.size());
          return;
        }

        MB_LOGD(ML_TAG, "sortedPins #:%d", layer->layerP->sortedPins.size());
        if (safeModeMB) {
          MB_LOGW(ML_TAG, "Safe mode enabled, not adding Physical driver");
          return;
        }

        nb_pins = 0;

        for (const SortedPin &sortedPin : layer->layerP->sortedPins) {
          // MB_LOGD(ML_TAG, "sortedPin s:%d #:%d p:%d", sortedPin.startLed, sortedPin.nrOfLights, sortedPin.pin);
          if (nb_pins < NUMSTRIPS) {
            if (GPIO_IS_VALID_OUTPUT_GPIO(sortedPin.pin)) {
              pins[nb_pins] = sortedPin.pin;
              lengths[nb_pins] = sortedPin.nrOfLights;
              nb_pins++;
            }
            else MB_LOGW(ML_TAG, "Pin %d (%d lights) not added as not valid for output", sortedPin.pin, sortedPin.nrOfLights);
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
        MB_LOGD(ML_TAG, "pins[");
        for (int i=0; i<nb_pins; i++) {
          MB_LOGD(ML_TAG, ", %d (%d)", pins[i], lengths[i]);
        }
        MB_LOGD(ML_TAG, "]");

        if (nb_pins > 0) {

          #ifndef CONFIG_IDF_TARGET_ESP32P4 // Non P4: Yves driver
            if (!initDone) {
              __NB_DMA_BUFFER = dmaBuffer; // __NB_DMA_BUFFER is a variable

              uint8_t savedBrightness = ledsDriver._brightness; //(initLed sets it to 255 and thats not what we want)

              ledsDriver.initled(layer->layerP->lights.channels, pins, lengths, nb_pins);

              ledsDriver.setBrightness(savedBrightness); //(initLed sets it to 255 and thats not what we want)

              #if ML_LIVE_MAPPING
                ledsDriver.setMapLed(&mapLed);
              #endif

              initDone = true; //so loop is called and initled not called again if channelsPerLight or pins saved
            } else {
              //don't call initled again as that will crash because if channelsPerLight (nb_components) change, the dma buffers are not big enough

              ledsDriver.updateDriver(pins, lengths, nb_pins, dmaBuffer);
            }

            //from lightPresetSaved
            //overwrite what initled has done as we don't use colorarrangment but assign offsets directly
            ledsDriver.nb_components = layer->layerP->lights.header.channelsPerLight;
            ledsDriver.p_r = layer->layerP->lights.header.offsetRed;
            ledsDriver.p_g = layer->layerP->lights.header.offsetGreen;
            ledsDriver.p_b = layer->layerP->lights.header.offsetBlue;

          #else // P4: Parlio Troy Driver
            initDone = true; //so loop is called and initled not called again if channelsPerLight or pins saved
          #endif
        }
      }
    #else //ESP32_LEDSDRIVER
      if (!lightPresetSaved || ledsDriver.initLedsDone || layer->layerP->sortedPins.size() == 0) return;

      if (layer->layerP->pass == 1) { //physical
        MB_LOGD(ML_TAG, "sortedPins #:%d", layer->layerP->sortedPins.size());
        if (safeModeMB) {
          MB_LOGW(ML_TAG, "Safe mode enabled, not adding Physical driver");
          return;
        }

        #if CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32
          int numPins = 0;
          PinConfig pinConfig[MAX_PINS];


          for (const SortedPin &sortedPin : layer->layerP->sortedPins) {
            MB_LOGD(ML_TAG, "sortedPin s:%d #:%d p:%d", sortedPin.startLed, sortedPin.nrOfLights, sortedPin.pin);
            if (numPins < MAX_PINS) {
              pinConfig[numPins].gpio = sortedPin.pin;
              pinConfig[numPins].nrOfLeds =  sortedPin.nrOfLights;
              numPins++;
            }
          }

          MB_LOGD(ML_TAG, "pins[");
          for (int i=0; i<numPins; i++) {
            MB_LOGD(ML_TAG, ", %d", pinConfig[i].gpio);
          }

          if (numPins > 0) {

            ledsDriver.initLeds(layer->layerP->lights.channels, pinConfig, numPins, layer->layerP->lights.header.channelsPerLight, layer->layerP->lights.header.offsetRed, layer->layerP->lights.header.offsetGreen, layer->layerP->lights.header.offsetBlue, layer->layerP->lights.header.offsetWhite); //102 is GRB

            #if ML_LIVE_MAPPING
              driver.setMapLed(&mapLed);
            #endif
          }
        #endif
      }
    #endif
  }

  ~PhysicalDriver() override {
    #if HP_ALL_DRIVERS
      MB_LOGD(ML_TAG, "Destroy %d + 1 dma buffers", __NB_DMA_BUFFER);

      ledsDriver.deleteDriver();

      // if recreating the Physical driver later, still initled cannot be done again ?
    #endif
  }

};

#endif