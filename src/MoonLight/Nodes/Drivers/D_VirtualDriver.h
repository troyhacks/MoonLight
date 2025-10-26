/**
    @title     MoonLight
    @file      VirtualDriver.h
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

class VirtualDriver: public DriverNode {
  public:

  static const char * name() {return "Virtual Driver";}
  static uint8_t dim() {return _NoD;}
  static const char * tags() {return "☸️🚧";}

  void setup() override {
    DriverNode::setup(); //adds maxPower and lights preset (rgb, rgbw, etc) control

    // add additional controls using addControl()
  }

  bool hasOnLayout() const override { return true; }
  void onLayout() override {
    if (layer->layerP->pass == 1 && !layer->layerP->monitorPass) { //physical

      if (!lightPresetSaved || layer->layerP->sortedPins.size() == 0) { //|| initDone can be done multiple times now...
        MB_LOGD(ML_TAG, "return: lightpresetsaved:%d initDone:%d #:%d", lightPresetSaved , initDone, layer->layerP->sortedPins.size());
        return;
      }

      MB_LOGD(ML_TAG, "sortedPins #:%d", layer->layerP->sortedPins.size());
      if (safeModeMB) {
        MB_LOGW(ML_TAG, "Safe mode enabled, not adding driver");
        return;
      }

      for (const SortedPin &sortedPin : layer->layerP->sortedPins) {
        // collect the definied pins 
      }

      if (layer->layerP->sortedPins.size() > 0) {

        //from lightPresetSaved, prepare LUT arrays:
        ledsDriver.nb_components = layer->layerP->lights.header.channelsPerLight;
        ledsDriver.p_r = layer->layerP->lights.header.offsetRed;
        ledsDriver.p_g = layer->layerP->lights.header.offsetGreen;
        ledsDriver.p_b = layer->layerP->lights.header.offsetBlue;

        if (initDone) {

          // do things after the driver has been initialized
          // eg change the pin assignment if needed

        } else {

          // init the driver for the first time, using pins, leds per pin etc.
          
          // set brightness again after initled

          // ledsDriver.initled(layer->layerP->lights.channels, pins, lengths, nb_pins);

          // ledsDriver.setBrightness(ledsDriver._brightness); //(initLed sets it to 255 and thats not what we want)

          initDone = true; //so loop is called and initled not called again if channelsPerLight or pins saved
        }
      }
    }
  }

  void onUpdate(String &oldValue, JsonObject control) override {

    if (control["name"] == "whatever") {
        //something changed
    }
  }

  void loop() override {
    DriverNode::loop(); //checks for changes in brightness and rgb color corrections and update the lut tables, also using maxPower.

    //driver.show();     // process layer->layerP->lights.channels (pka leds array) using LUT
    layer->layerP->lights.channels;
    ledsDriver.__red_map[0];
    ledsDriver.__green_map[0];
    ledsDriver.__blue_map[0];
    ledsDriver.__white_map[0];
  }

  ~VirtualDriver() override {
    // driver is deleted,so delete whatever was allocated
  }
  
};


#endif