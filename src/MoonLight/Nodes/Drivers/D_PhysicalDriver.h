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

#define NUMSTRIPS 16 //max 16 strips for physical driver

class PhysicalDriver: public DriverNode {
  public:

  static const char * name() {return "Physical Driver ☸️";}
  static uint8_t dim() {return _3D;}
  static const char * tags() {return "";}

  uint8_t pins[NUMSTRIPS]; //max 16 pins
  int lengths[NUMSTRIPS];
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

      if (layerV->layerP->lights.header.isPositions == 0) {

        DriverNode::loop();

        #ifndef CONFIG_IDF_TARGET_ESP32P4
          if (ledsDriver.total_leds > 0)
            ledsDriver.showPixels(WAIT);
        #else
          show_parlio(pins, layerV->layerP->lights.header.nrOfLights, layerV->layerP->lights.channels
                      , ledsDriver._brightness, layerV->layerP->lights.header.channelsPerLight == 4, nb_pins, lengths[0]
                      , layerV->layerP->lights.header.offsetRed, layerV->layerP->lights.header.offsetGreen, layerV->layerP->lights.header.offsetBlue
                    );
        #endif

    }
    #else //ESP32_LEDSDRIVER
      if (!ledsDriver.initLedsDone) return;

      if (layerV->layerP->lights.header.isPositions == 0) {

          DriverNode::loop();

          ledsDriver.show();
      }
    #endif
  }

  bool hasLayout() const override { return true; }
  void onLayout() override {
    #if HP_ALL_DRIVERS
      if (layerV->layerP->pass == 1 && !layerV->layerP->monitorPass) { //physical

        if (!lightPresetSaved || layerV->layerP->sortedPins.size() == 0) { //|| initDone can be done multiple times now...
          MB_LOGD(ML_TAG, "return: lightpresetsaved:%d initDone:%d #:%d", lightPresetSaved , initDone, layerV->layerP->sortedPins.size());
          return;
        }

        MB_LOGD(ML_TAG, "sortedPins #:%d", layerV->layerP->sortedPins.size());
        if (safeModeMB) {
          MB_LOGW(ML_TAG, "Safe mode enabled, not adding Physical driver");
          return;
        }

        nb_pins = 0;

        for (const SortedPin &sortedPin : layerV->layerP->sortedPins) {
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
            if (initDone) {
              //don't call initled again as that will crash because if channelsPerLight (nb_components) change, the dma buffers are not big enough

              //so do what ledsDriver.initled is doing:

              //from lightPresetSaved
              ledsDriver.nb_components = layerV->layerP->lights.header.channelsPerLight;
              ledsDriver.p_r = layerV->layerP->lights.header.offsetRed;
              ledsDriver.p_g = layerV->layerP->lights.header.offsetGreen;
              ledsDriver.p_b = layerV->layerP->lights.header.offsetBlue;

              //from initled
              ledsDriver.num_strips = nb_pins;
              ledsDriver.total_leds = 0;
              for (int i = 0; i < ledsDriver.num_strips; i++)
              {
                  ledsDriver.stripSize[i] = lengths[i];
                  ledsDriver.total_leds += lengths[i];
              }
              int num_led_per_strip = ledsDriver.maxLength(lengths, ledsDriver.num_strips);

              //from __initled:

              ledsDriver.num_led_per_strip = num_led_per_strip;
              ledsDriver._offsetDisplay.offsetx = 0;
              ledsDriver._offsetDisplay.offsety = 0;
              ledsDriver._offsetDisplay.panel_width = num_led_per_strip;
              ledsDriver._offsetDisplay.panel_height = 9999;
              ledsDriver._defaultOffsetDisplay = ledsDriver._offsetDisplay;
              ledsDriver.linewidth = num_led_per_strip;

              // ledsDriver.setShowDelay(num_led_per_strip);
              ledsDriver.setShowDelay();
              ledsDriver.setGlobalNumStrips();

              ledsDriver.setPins(pins); //if pins and lengths changed, set that right

              // i2sInit(); //not necessary, initled did it, no need to change

              //P4 for PhysicalDriver not supported yet

              #if CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32
                //delete allocations done by physical driver if total channels changes (larger)
                for (int i = 0; i < __NB_DMA_BUFFER + 2; i++)
                {
                    heap_caps_free(ledsDriver.DMABuffersTampon[i]->buffer);
                    heap_caps_free(ledsDriver.DMABuffersTampon[i]);
                }
                heap_caps_free(ledsDriver.DMABuffersTampon);
              #endif

              __NB_DMA_BUFFER = dmaBuffer; // __NB_DMA_BUFFER is a variable now 🥳

              ledsDriver.initDMABuffers(); //create them again

              MB_LOGD(ML_TAG, "reinit physDriver %d x %d (%d)", ledsDriver.num_strips, num_led_per_strip, __NB_DMA_BUFFER);

              return; //bye bye initled, we did it ourselves ;-)
            } else {
              __NB_DMA_BUFFER = dmaBuffer; // __NB_DMA_BUFFER is a variable now 🥳

              uint8_t savedBrightness = ledsDriver._brightness; //(initLed sets it to 255 and thats not what we want)

              ledsDriver.initled(layerV->layerP->lights.channels, pins, lengths, nb_pins);

              //overwrite what initled has done as we don't use colorarrangment but assign offsets directly
              ledsDriver.nb_components = layerV->layerP->lights.header.channelsPerLight;
              ledsDriver.p_r = layerV->layerP->lights.header.offsetRed;
              ledsDriver.p_g = layerV->layerP->lights.header.offsetGreen;
              ledsDriver.p_b = layerV->layerP->lights.header.offsetBlue;

              ledsDriver.setBrightness(savedBrightness); //(initLed sets it to 255 and thats not what we want)

              #if ML_LIVE_MAPPING
                ledsDriver.setMapLed(&mapLed);
              #endif

              initDone = true; //so loop is called and initled not called again if channelsPerLight or pins saved
            }
          #else // P4: Parlio Troy Driver
            initDone = true; //so loop is called and initled not called again if channelsPerLight or pins saved
          #endif
        }
      }
    #else //ESP32_LEDSDRIVER
      if (!lightPresetSaved || ledsDriver.initLedsDone || layerV->layerP->sortedPins.size() == 0) return;

      if (layerV->layerP->pass == 1) { //physical
        MB_LOGD(ML_TAG, "sortedPins #:%d", layerV->layerP->sortedPins.size());
        if (safeModeMB) {
          MB_LOGW(ML_TAG, "Safe mode enabled, not adding Physical driver");
          return;
        }

        #if CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32
          int numPins = 0;
          PinConfig pinConfig[MAX_PINS];


          for (const SortedPin &sortedPin : layerV->layerP->sortedPins) {
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

            ledsDriver.initLeds(layerV->layerP->lights.channels, pinConfig, numPins, layerV->layerP->lights.header.channelsPerLight, layerV->layerP->lights.header.offsetRed, layerV->layerP->lights.header.offsetGreen, layerV->layerP->lights.header.offsetBlue, layerV->layerP->lights.header.offsetWhite); //102 is GRB

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

      //P4 for PhysicalDriver not supported yet
      #if CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32
            for (int i = 0; i < __NB_DMA_BUFFER + 2; i++)
            {
                heap_caps_free(ledsDriver.DMABuffersTampon[i]->buffer);
                heap_caps_free(ledsDriver.DMABuffersTampon[i]);
            }
            heap_caps_free(ledsDriver.DMABuffersTampon);
      #endif
      //anything else to delete? I2S ...

      // if recreating the Physical driver later, still initled cannot be done again ?
    #endif
  }

};

#endif