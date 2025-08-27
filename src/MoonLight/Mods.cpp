/**
    @title     MoonLight
    @file      Mods.cpp
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/moonlight/overview/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact us for more information.
**/

#if FT_MOONLIGHT

#include "Mods.h"

#include <ESP32SvelteKit.h> // for safeModeMB and restartNeeded

#if HP_ALL_DRIVERS
  #define NUMSTRIPS 16 //not needed for non virtal... (see transpose...)
  // #define NUM_LEDS_PER_STRIP 256 not for non virtal... (only setting __delay when NO_WAIT)
  #define __NB_DMA_BUFFER 50
  #include "I2SClocklessLedDriver.h"
  static I2SClocklessLedDriver ledsDriver;
#else //ESP32_LEDSDRIVER  
  #include "ESP32-LedsDriver.h"
  #define MAX_PINS 20 // this is also defined in ESP32-LedsDriver.h...
  #if CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32S2
    static PhysicalDriverESP32S3 ledsDriver; //    sizeof(driver) = 1080K !
  #elif CONFIG_IDF_TARGET_ESP32
    static PhysicalDriverESP32D0 ledsDriver; //    sizeof(driver) = 1080K !
  #else
    static LedsDriver ledsDriver; //   only the core driver, for setBirghtness and setColorCorrection and LUT
  #endif
#endif

void DriverNode::setup() {
  addControl(maxPower, "maxPower", "number", 0, 100);
  JsonObject property = addControl(lightPreset, "lightPreset", "select"); 
  JsonArray values = property["values"].to<JsonArray>();
  values.add("RGB");
  values.add("RBG");
  values.add("GRB"); //default
  values.add("GBR");
  values.add("BRG");
  values.add("BGR");
  values.add("RGBW"); //4 channel par/dmx light
  values.add("GRBW"); //rgbw led eg. sk6812
  values.add("GRBW6"); //some curtains
  values.add("RGBWYP"); //6 channel par/dmx light with UV etc
  values.add("TroyMH15");
  values.add("TroyMH32");
  values.add("WowiMH24");
}

void DriverNode::loop() {

  LightsHeader *header = &layerV->layerP->lights.header;

  //use ledsDriver LUT for super efficient leds dimming 🔥 (used by reOrderAndDimRGBW)

  uint8_t brightness = (header->offsetBrightness == UINT8_MAX)?header->brightness:255; //set brightness to 255 if offsetBrightness is set (fixture will do its own brightness)

  if (brightness != brightnessSaved) {
    //Use FastLED for setMaxPowerInMilliWatts stuff
    uint8_t correctedBrightness = calculate_max_brightness_for_power_mW((CRGB *)&layerV->layerP->lights.channels, layerV->layerP->lights.header.nrOfLights, brightness, maxPower * 1000);
    MB_LOGD(ML_TAG, "setBrightness b:%d + p:%d -> cb:%d", brightness, maxPower, correctedBrightness);
    ledsDriver.setBrightness(correctedBrightness);
    brightnessSaved = brightness;
  }

  #if HP_ALL_DRIVERS
    if (savedColorCorrection.red != layerV->layerP->lights.header.red || savedColorCorrection.green != layerV->layerP->lights.header.green || savedColorCorrection.blue != layerV->layerP->lights.header.blue) {
      ledsDriver.setGamma(layerV->layerP->lights.header.red/255.0, layerV->layerP->lights.header.blue/255.0, layerV->layerP->lights.header.green/255.0);
      MB_LOGD(ML_TAG, "setColorCorrection r:%d, g:%d, b:%d (%d %d %d)", layerV->layerP->lights.header.red, layerV->layerP->lights.header.green, layerV->layerP->lights.header.blue, savedColorCorrection.red, savedColorCorrection.green, savedColorCorrection.blue);
      savedColorCorrection.red = layerV->layerP->lights.header.red;
      savedColorCorrection.green = layerV->layerP->lights.header.green;
      savedColorCorrection.blue = layerV->layerP->lights.header.blue;
    }
  #else //ESP32_LEDSDRIVER
    CRGB correction;
    uint8_t white;
    ledsDriver.getColorCorrection(correction.red, correction.green, correction.blue, white);
    if (correction.red != layerV->layerP->lights.header.red || correction.green != layerV->layerP->lights.header.green || correction.blue != layerV->layerP->lights.header.blue) {
      MB_LOGD(ML_TAG, "setColorCorrection r:%d, g:%d, b:%d (%d %d %d)", layerV->layerP->lights.header.red, layerV->layerP->lights.header.green, layerV->layerP->lights.header.blue, correction.red, correction.green, correction.blue);
      ledsDriver.setColorCorrection(layerV->layerP->lights.header.red, layerV->layerP->lights.header.green, layerV->layerP->lights.header.blue);
    }
  #endif
}

void DriverNode::updateControl(JsonObject control) {
  Node::updateControl(control);

  LightsHeader *header = &layerV->layerP->lights.header;

  MB_LOGD(ML_TAG, "%s: %s ", control["name"].as<String>().c_str(), control["value"].as<String>().c_str());

  if (control["name"] == "maxPower") {
    uint8_t brightness = (header->offsetBrightness == UINT8_MAX)?header->brightness:255; //set brightness to 255 if offsetBrightness is set (fixture will do its own brightness)
    uint8_t correctedBrightness = calculate_max_brightness_for_power_mW((CRGB *)&layerV->layerP->lights.channels, layerV->layerP->lights.header.nrOfLights, brightness, maxPower * 1000);
    MB_LOGD(ML_TAG, "setBrightness b:%d + p:%d -> cb:%d", brightness, maxPower, correctedBrightness);
    ledsDriver.setBrightness(correctedBrightness);
  }
  else if (control["name"] == "lightPreset") {
    uint8_t oldChannelsPerLight = header->channelsPerLight;

    header->resetOffsets();

    switch (lightPreset) {
      case 0: header->channelsPerLight = 3; header->offsetRed = 0; header->offsetGreen = 1; header->offsetBlue = 2; break; //RGB
      case 1: header->channelsPerLight = 3; header->offsetRed = 0; header->offsetGreen = 2; header->offsetBlue = 1; break; //RBG
      case 2: header->channelsPerLight = 3; header->offsetRed = 1; header->offsetGreen = 0; header->offsetBlue = 2; break; //GRB
      case 3: header->channelsPerLight = 3; header->offsetRed = 2; header->offsetGreen = 0; header->offsetBlue = 1; break; //GBR
      case 4: header->channelsPerLight = 3; header->offsetRed = 1; header->offsetGreen = 2; header->offsetBlue = 0; break; //BRG
      case 5: header->channelsPerLight = 3; header->offsetRed = 2; header->offsetGreen = 1; header->offsetBlue = 0; break; //BGR
      case 6: header->channelsPerLight = 4; header->offsetRed = 0; header->offsetGreen = 1; header->offsetBlue = 2; header->offsetWhite = 3; break; //RGBW - Par Lights
      case 7: header->channelsPerLight = 4; header->offsetRed = 1; header->offsetGreen = 0; header->offsetBlue = 2; header->offsetWhite = 3; break; //GRBW - RGBW Leds
      case 8: header->channelsPerLight = 6; header->offsetRed = 1; header->offsetGreen = 0; header->offsetBlue = 2; break; //GRBW6
      case 9: header->channelsPerLight = 6; header->offsetRed = 0; header->offsetGreen = 1; header->offsetBlue = 2; header->offsetWhite = 3; break; //RGBWYP - 6 channel Par/DMX Lights with UV etc
      case 10: //troy32
        layerV->layerP->lights.header.channelsPerLight = 32;
        header->offsetRed = 0; header->offsetGreen = 1; header->offsetBlue = 2;
        layerV->layerP->lights.header.offsetRGB = 9;
        layerV->layerP->lights.header.offsetRGB1 = 13;
        layerV->layerP->lights.header.offsetRGB2 = 17;
        layerV->layerP->lights.header.offsetRGB3 = 24;
        layerV->layerP->lights.header.offsetPan = 0;
        layerV->layerP->lights.header.offsetTilt = 2;
        layerV->layerP->lights.header.offsetZoom = 5;
        layerV->layerP->lights.header.offsetBrightness = 6;
        break;
      case 11: //troyMH15
        layerV->layerP->lights.header.channelsPerLight = 15; //set channels per light to 15 (RGB + Pan + Tilt + Zoom + Brightness)
        header->offsetRed = 0; header->offsetGreen = 1; header->offsetBlue = 2;
        layerV->layerP->lights.header.offsetRGB = 10; //set offset for RGB lights in DMX map
        layerV->layerP->lights.header.offsetPan = 0;
        layerV->layerP->lights.header.offsetTilt = 1;
        layerV->layerP->lights.header.offsetZoom = 7;
        layerV->layerP->lights.header.offsetBrightness = 8; //set offset for brightness
        layerV->layerP->lights.header.offsetGobo = 5; //set offset for color wheel in DMX map
        layerV->layerP->lights.header.offsetBrightness2 = 3; //set offset for color wheel brightness in DMX map    } //BGR
        break;
      case 12: //wowiMH24
        layerV->layerP->lights.header.channelsPerLight = 24;
        header->offsetRed = 0; header->offsetGreen = 1; header->offsetBlue = 2;
        layerV->layerP->lights.header.offsetPan = 0;
        layerV->layerP->lights.header.offsetTilt = 1;
        layerV->layerP->lights.header.offsetBrightness = 3;
        layerV->layerP->lights.header.offsetRGB = 4;
        layerV->layerP->lights.header.offsetRGB1 = 8;
        layerV->layerP->lights.header.offsetRGB2 = 12;
        layerV->layerP->lights.header.offsetZoom = 17;
        break;
    }


    MB_LOGI(ML_TAG, "setLightPreset %d (cPL:%d, o:%d,%d,%d,%d)", lightPreset, header->channelsPerLight, header->offsetRed, header->offsetGreen, header->offsetBlue, header->offsetWhite);
    
    // FASTLED_ASSERT(true, "oki");
    
    #if HP_ALL_DRIVERS
      if (initDone) {
        
        // ledsDriver.setOffsets(layerV->layerP->lights.header.offsetRed, layerV->layerP->lights.header.offsetGreen, layerV->layerP->lights.header.offsetBlue, layerV->layerP->lights.header.offsetWhite);

        if (oldChannelsPerLight != header->channelsPerLight)
          restartNeeded = true; //in case 
      }
    #else //ESP32_LEDSDRIVER
      if (ledsDriver.initLedsDone) {
        
        ledsDriver.setOffsets(layerV->layerP->lights.header.offsetRed, layerV->layerP->lights.header.offsetGreen, layerV->layerP->lights.header.offsetBlue, layerV->layerP->lights.header.offsetWhite);

        if (oldChannelsPerLight != header->channelsPerLight)
          restartNeeded = true; //in case 
      }
    #endif

    lightPresetSaved = true;
  }
}

inline void DriverNode::reOrderAndDimRGBW(uint8_t *packetRGBChannel, uint8_t *lightsRGBChannel) {
  //use ledsDriver.__rbg_map[0]; for super fast brightness and gamma correction! see secondPixel in ESP32-LedDriver!
  //apply the LUT to the RGB channels !
  
  packetRGBChannel[layerV->layerP->lights.header.offsetRed] = ledsDriver.__red_map[lightsRGBChannel[0]];
  packetRGBChannel[layerV->layerP->lights.header.offsetGreen] = ledsDriver.__green_map[lightsRGBChannel[1]];
  packetRGBChannel[layerV->layerP->lights.header.offsetBlue] = ledsDriver.__blue_map[lightsRGBChannel[2]];
  if (layerV->layerP->lights.header.offsetWhite != UINT8_MAX)
    packetRGBChannel[layerV->layerP->lights.header.offsetWhite] = ledsDriver.__white_map[lightsRGBChannel[3]];
}

void ArtNetDriverMod::setup() {

  DriverNode::setup();

  addControl(controllerIP3, "controllerIP", "number");
  addControl(port, "port", "number");
  addControl(FPSLimiter, "FPSLimiter", "number");
  addControl(nrOfOutputs, "nrOfOutputs", "number");
  addControl(universesPerOutput, "universesPerOutput", "number");
  addControl(channelsPerOutput, "channelsPerOutput", "number", 0, 65538);

  memcpy(packet_buffer, ART_NET_HEADER, sizeof(ART_NET_HEADER)); // copy in the Art-Net header.
}

bool ArtNetDriverMod::writePackage() {
  // set the parts of the Art-Net packet header that change:
  packet_buffer[14] = universe; //The low byte of the 15 bit Port-Address to which this packet is destined
  packet_buffer[15] = universe >> 8; //The top 7 bits of the 15 bit Port-Address to which this packet is destined
  packet_buffer[16] = packetSize >> 8; //The length of the DMX512 data array. High Byte
  packet_buffer[17] = packetSize; //Low Byte of above

  if (!artnetudp.writeTo(packet_buffer, MIN(packetSize, 512)+18, controllerIP, port)) {
      Serial.print("🐛");
      return false; // borked //no connection...
  } 
  // else Serial.printf(" %d", packetSize);

  packetSize = 0;
  universe++; //each packet is one universe
  return true;
}

void ArtNetDriverMod::loop() {

  DriverNode::loop();

  LightsHeader *header = &layerV->layerP->lights.header;

  //continue with Art-Net code
  controllerIP = WiFi.localIP();
  controllerIP[3] = controllerIP3;
  if(!controllerIP) return;
  
  if (header->isPositions != 0) return; //don't update if positions are sent

  //wait until the throttle FPS is reached
  //wait needed to avoid misalignment between packages sent and displayed - 🚧
  wait = 1000 / FPSLimiter + lastMillis - millis();
  if (wait > 0 && wait < 1000 / FPSLimiter)
    delay(wait); // delay in ms
  // else
  //   MB_LOGW(ML_TAG, "wait %d", wait);
  // if (millis() - lastMillis < 1000 / FPSLimiter)
  //   delay(1000 / FPSLimiter + lastMillis - millis()); // delay in ms
  //   return;
  lastMillis = millis();

  // only need to set once per frame
  packet_buffer[12] = (sequenceNumber++%254)+1; //The sequence number is used to ensure that ArtDmx packets are used in the correct order, ranging from 1..255
  packet_buffer[13] = 0; //The physical input port from which DMX512 data was input
  
  universe = 0;
  packetSize = 0;
  channels_remaining = channelsPerOutput;
  
  //send all the leds to artnet
  for (int indexP = 0; indexP < header->nrOfLights; indexP++) {
    //fill a package

    reOrderAndDimRGBW(&packet_buffer[packetSize+18+header->offsetRGB], &layerV->layerP->lights.channels[indexP*header->channelsPerLight + header->offsetRGB]);

    if (header->offsetRGB1 != UINT8_MAX )
      reOrderAndDimRGBW(&packet_buffer[packetSize+18+header->offsetRGB1], &layerV->layerP->lights.channels[indexP*header->channelsPerLight + header->offsetRGB1]);
    if (header->offsetRGB2 != UINT8_MAX )
      reOrderAndDimRGBW(&packet_buffer[packetSize+18+header->offsetRGB2], &layerV->layerP->lights.channels[indexP*header->channelsPerLight + header->offsetRGB2]);
    if (header->offsetRGB3 != UINT8_MAX )
      reOrderAndDimRGBW(&packet_buffer[packetSize+18+header->offsetRGB3], &layerV->layerP->lights.channels[indexP*header->channelsPerLight + header->offsetRGB3]);

    if (lightPreset == 9 && indexP < 36)
      packetSize += 4; // this config assumes first 36 lights are 4 channels per Light !!!!
    else
      packetSize += header->channelsPerLight;

    channels_remaining -= header->channelsPerLight;

    //if packet_buffer full, or output full, send the buffer
    if (packetSize + header->channelsPerLight > ARTNET_CHANNELS_PER_PACKET || channels_remaining < header->channelsPerLight) { //next light will not fit in the package, so send what we got
      // Serial.printf("; %d %d %d", header->nrOfLights, packetSize+18, header->nrOfLights*header->channelsPerLight);

      if (!writePackage()) return; //resets packagesize

      if (channels_remaining < header->channelsPerLight) {// jump to next output
        channels_remaining = channelsPerOutput; //reset for a new output

        while (universe%universesPerOutput != 0) universe++; //advance to next port
      }
    }
  }

  //send the last partially filled package
  if (packetSize > 0) {
    // Serial.printf(", %d %d %d", header->nrOfLights, packetSize+18, header->nrOfLights*header->channelsPerLight);

    writePackage(); //remaining
  }

}

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
    hasLayout = true; //so addLayout is called if layout changes (works for FASTLED)

    addControl(maxPower, "maxPower", "number", 0, 100);
    addControl(version, "Version", "text", 0, 20, true);
    addControl(chipSet, "chipSet", "text", 0, 20, true);
    addControl(colorOrder, "colorOrder", "text", 0, 20, true);
    addControl(usesI2S, "usesI2S", "checkbox", 0, 20, true);
  }

  void  FastLEDDriverMod::addLayout() {
    if (layerV->layerP->sortedPins.size() == 0) return;

    if (layerV->layerP->pass == 1) { //physical
      // if (safeModeMB) {
      //     MB_LOGW(ML_TAG, "Safe mode enabled, not adding FastLED driver");
      //     return;
      // }

      MB_LOGD(ML_TAG, "sortedPins #:%d", layerV->layerP->sortedPins.size());

      uint8_t pinCount = 0;
      for (const SortedPin &sortedPin : layerV->layerP->sortedPins) {
        pinCount++;
        if (pinCount > 4) {//FastLED RMT supports max 4 pins!
          MB_LOGW(ML_TAG, "Too many pins!");
          break;
        }
        MB_LOGD(ML_TAG, "sortpins s:%d #:%d p:%d", sortedPin.startLed, sortedPin.nrOfLights, sortedPin.pin);

        uint16_t startLed = sortedPin.startLed;
        uint16_t nrOfLights = sortedPin.nrOfLights;
        uint16_t pin = sortedPin.pin;

        CRGB *leds = (CRGB *)layerV->layerP->lights.channels;

        switch (sortedPin.pin) {
        #if CONFIG_IDF_TARGET_ESP32
          case 0: FastLED.addLeds<ML_CHIPSET, 0 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 1: FastLED.addLeds<ML_CHIPSET, 1 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 2: FastLED.addLeds<ML_CHIPSET, 2 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 3: FastLED.addLeds<ML_CHIPSET, 3 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 4: FastLED.addLeds<ML_CHIPSET, 4 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 5: FastLED.addLeds<ML_CHIPSET, 5 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          // case 6: FastLED.addLeds<ML_CHIPSET, 6 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          // case 7: FastLED.addLeds<ML_CHIPSET, 7 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          // case 8: FastLED.addLeds<ML_CHIPSET, 8 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          // case 9: FastLED.addLeds<ML_CHIPSET, 9 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          // case 10: FastLED.addLeds<ML_CHIPSET, 10 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 11: FastLED.addLeds<ML_CHIPSET, 11 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 12: FastLED.addLeds<ML_CHIPSET, 12 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 13: FastLED.addLeds<ML_CHIPSET, 13 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 14: FastLED.addLeds<ML_CHIPSET, 14 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 15: FastLED.addLeds<ML_CHIPSET, 15 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
      #if !defined(BOARD_HAS_PSRAM) && !defined(ARDUINO_ESP32_PICO)
          // 16+17 = reserved for PSRAM, or reserved for FLASH on pico-D4
          case 16: FastLED.addLeds<ML_CHIPSET, 16 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 17: FastLED.addLeds<ML_CHIPSET, 17 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
      #endif
          case 18: FastLED.addLeds<ML_CHIPSET, 18 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 19: FastLED.addLeds<ML_CHIPSET, 19 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          // case 20: FastLED.addLeds<ML_CHIPSET, 20 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 21: FastLED.addLeds<ML_CHIPSET, 21 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 22: FastLED.addLeds<ML_CHIPSET, 22 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 23: FastLED.addLeds<ML_CHIPSET, 23 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          // case 24: FastLED.addLeds<ML_CHIPSET, 24 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 25: FastLED.addLeds<ML_CHIPSET, 25 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 26: FastLED.addLeds<ML_CHIPSET, 26 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 27: FastLED.addLeds<ML_CHIPSET, 27 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          // case 28: FastLED.addLeds<ML_CHIPSET, 28 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          // case 29: FastLED.addLeds<ML_CHIPSET, 29 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          // case 30: FastLED.addLeds<ML_CHIPSET, 30 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          // case 31: FastLED.addLeds<ML_CHIPSET, 31 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 32: FastLED.addLeds<ML_CHIPSET, 32 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 33: FastLED.addLeds<ML_CHIPSET, 33 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          // 34-39 input-only
          // case 34: FastLED.addLeds<ML_CHIPSET, 34 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          // case 35: FastLED.addLeds<ML_CHIPSET, 35 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          // case 36: FastLED.addLeds<ML_CHIPSET, 36 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          // case 37: FastLED.addLeds<ML_CHIPSET, 37 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          // case 38: FastLED.addLeds<ML_CHIPSET, 38 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          // case 39: FastLED.addLeds<ML_CHIPSET, 39 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        #endif //CONFIG_IDF_TARGET_ESP32

        #if CONFIG_IDF_TARGET_ESP32S2
          case 0: FastLED.addLeds<ML_CHIPSET, 0 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 1: FastLED.addLeds<ML_CHIPSET, 1 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 2: FastLED.addLeds<ML_CHIPSET, 2 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 3: FastLED.addLeds<ML_CHIPSET, 3 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 4: FastLED.addLeds<ML_CHIPSET, 4 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 5: FastLED.addLeds<ML_CHIPSET, 5 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 6: FastLED.addLeds<ML_CHIPSET, 6 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 7: FastLED.addLeds<ML_CHIPSET, 7 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 8: FastLED.addLeds<ML_CHIPSET, 8 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 9: FastLED.addLeds<ML_CHIPSET, 9 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 10: FastLED.addLeds<ML_CHIPSET, 10 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 11: FastLED.addLeds<ML_CHIPSET, 11 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 12: FastLED.addLeds<ML_CHIPSET, 12 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 13: FastLED.addLeds<ML_CHIPSET, 13 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 14: FastLED.addLeds<ML_CHIPSET, 14 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 15: FastLED.addLeds<ML_CHIPSET, 15 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 16: FastLED.addLeds<ML_CHIPSET, 16 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 17: FastLED.addLeds<ML_CHIPSET, 17 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 18: FastLED.addLeds<ML_CHIPSET, 18 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
      #if !ARDUINO_USB_CDC_ON_BOOT
          // 19 + 20 = USB HWCDC. reserved for USB port when ARDUINO_USB_CDC_ON_BOOT=1
          case 19: FastLED.addLeds<ML_CHIPSET, 19 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 20: FastLED.addLeds<ML_CHIPSET, 20 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
      #endif
          case 21: FastLED.addLeds<ML_CHIPSET, 21 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          // 22 to 32: not connected, or reserved for SPI FLASH
          // case 22: FastLED.addLeds<ML_CHIPSET, 22 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          // case 23: FastLED.addLeds<ML_CHIPSET, 23 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          // case 24: FastLED.addLeds<ML_CHIPSET, 24 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          // case 25: FastLED.addLeds<ML_CHIPSET, 25 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
      #if !defined(BOARD_HAS_PSRAM)
          // 26-32 = reserved for PSRAM
          case 26: FastLED.addLeds<ML_CHIPSET, 26 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          // case 27: FastLED.addLeds<ML_CHIPSET, 27 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          // case 28: FastLED.addLeds<ML_CHIPSET, 28 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          // case 29: FastLED.addLeds<ML_CHIPSET, 29 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          // case 30: FastLED.addLeds<ML_CHIPSET, 30 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          // case 31: FastLED.addLeds<ML_CHIPSET, 31 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          // case 32: FastLED.addLeds<ML_CHIPSET, 32 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
      #endif
          case 33: FastLED.addLeds<ML_CHIPSET, 33 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 34: FastLED.addLeds<ML_CHIPSET, 34 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 35: FastLED.addLeds<ML_CHIPSET, 35 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 36: FastLED.addLeds<ML_CHIPSET, 36 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 37: FastLED.addLeds<ML_CHIPSET, 37 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 38: FastLED.addLeds<ML_CHIPSET, 38 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 39: FastLED.addLeds<ML_CHIPSET, 39 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 40: FastLED.addLeds<ML_CHIPSET, 40 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 41: FastLED.addLeds<ML_CHIPSET, 41 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 42: FastLED.addLeds<ML_CHIPSET, 42 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 43: FastLED.addLeds<ML_CHIPSET, 43 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 44: FastLED.addLeds<ML_CHIPSET, 44 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 45: FastLED.addLeds<ML_CHIPSET, 45 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          // 46 input-only
          // case 46: FastLED.addLeds<ML_CHIPSET, 46 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        #endif //CONFIG_IDF_TARGET_ESP32S2

        #if CONFIG_IDF_TARGET_ESP32C3
          case 0: FastLED.addLeds<ML_CHIPSET, 0 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 1: FastLED.addLeds<ML_CHIPSET, 1 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 2: FastLED.addLeds<ML_CHIPSET, 2 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 3: FastLED.addLeds<ML_CHIPSET, 3 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 4: FastLED.addLeds<ML_CHIPSET, 4 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 5: FastLED.addLeds<ML_CHIPSET, 5 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 6: FastLED.addLeds<ML_CHIPSET, 6 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 7: FastLED.addLeds<ML_CHIPSET, 7 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 8: FastLED.addLeds<ML_CHIPSET, 8 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 9: FastLED.addLeds<ML_CHIPSET, 9 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 10: FastLED.addLeds<ML_CHIPSET, 10 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          // 11-17 reserved for SPI FLASH
          //case 11: FastLED.addLeds<ML_CHIPSET, 11 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          //case 12: FastLED.addLeds<ML_CHIPSET, 12 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          //case 13: FastLED.addLeds<ML_CHIPSET, 13 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          //case 14: FastLED.addLeds<ML_CHIPSET, 14 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          //case 15: FastLED.addLeds<ML_CHIPSET, 15 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          //case 16: FastLED.addLeds<ML_CHIPSET, 16 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          //case 17: FastLED.addLeds<ML_CHIPSET, 17 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
      #if !ARDUINO_USB_CDC_ON_BOOT
          // 18 + 19 = USB HWCDC. reserved for USB port when ARDUINO_USB_CDC_ON_BOOT=1
          case 18: FastLED.addLeds<ML_CHIPSET, 18 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 19: FastLED.addLeds<ML_CHIPSET, 19 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
      #endif
          // 20+21 = Serial RX+TX --> don't use for LEDS when serial-to-USB is needed
          case 20: FastLED.addLeds<ML_CHIPSET, 20 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 21: FastLED.addLeds<ML_CHIPSET, 21 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        #endif //CONFIG_IDF_TARGET_ESP32S2

        #if CONFIG_IDF_TARGET_ESP32S3
          case 0: FastLED.addLeds<ML_CHIPSET, 0 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 1: FastLED.addLeds<ML_CHIPSET, 1 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 2: FastLED.addLeds<ML_CHIPSET, 2 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 3: FastLED.addLeds<ML_CHIPSET, 3 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 4: FastLED.addLeds<ML_CHIPSET, 4 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 5: FastLED.addLeds<ML_CHIPSET, 5 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 6: FastLED.addLeds<ML_CHIPSET, 6 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 7: FastLED.addLeds<ML_CHIPSET, 7 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 8: FastLED.addLeds<ML_CHIPSET, 8 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 9: FastLED.addLeds<ML_CHIPSET, 9 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 10: FastLED.addLeds<ML_CHIPSET, 10 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 11: FastLED.addLeds<ML_CHIPSET, 11 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 12: FastLED.addLeds<ML_CHIPSET, 12 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 13: FastLED.addLeds<ML_CHIPSET, 13 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 14: FastLED.addLeds<ML_CHIPSET, 14 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 15: FastLED.addLeds<ML_CHIPSET, 15 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 16: FastLED.addLeds<ML_CHIPSET, 16 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 17: FastLED.addLeds<ML_CHIPSET, 17 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 18: FastLED.addLeds<ML_CHIPSET, 18 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        #if !ARDUINO_USB_CDC_ON_BOOT
          // 19 + 20 = USB-JTAG. Not recommended for other uses.
          case 19: FastLED.addLeds<ML_CHIPSET, 19 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 20: FastLED.addLeds<ML_CHIPSET, 20 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        #endif
          case 21: FastLED.addLeds<ML_CHIPSET, 21 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          // // 22 to 32: not connected, or SPI FLASH
          // case 22: FastLED.addLeds<ML_CHIPSET, 22 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          // case 23: FastLED.addLeds<ML_CHIPSET, 23 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          // case 24: FastLED.addLeds<ML_CHIPSET, 24 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          // case 25: FastLED.addLeds<ML_CHIPSET, 25 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          // case 26: FastLED.addLeds<ML_CHIPSET, 26 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          // case 27: FastLED.addLeds<ML_CHIPSET, 27 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          // case 28: FastLED.addLeds<ML_CHIPSET, 28 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          // case 29: FastLED.addLeds<ML_CHIPSET, 29 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          // case 30: FastLED.addLeds<ML_CHIPSET, 30 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          // case 31: FastLED.addLeds<ML_CHIPSET, 31 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          // case 32: FastLED.addLeds<ML_CHIPSET, 32 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        #if !defined(BOARD_HAS_PSRAM)
          // 33 to 37: reserved if using _octal_ SPI Flash or _octal_ PSRAM
          case 33: FastLED.addLeds<ML_CHIPSET, 33 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 34: FastLED.addLeds<ML_CHIPSET, 34 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 35: FastLED.addLeds<ML_CHIPSET, 35 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 36: FastLED.addLeds<ML_CHIPSET, 36 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 37: FastLED.addLeds<ML_CHIPSET, 37 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        #endif
          case 38: FastLED.addLeds<ML_CHIPSET, 38 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 39: FastLED.addLeds<ML_CHIPSET, 39 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 40: FastLED.addLeds<ML_CHIPSET, 40 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 41: FastLED.addLeds<ML_CHIPSET, 41 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 42: FastLED.addLeds<ML_CHIPSET, 42 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          // 43+44 = Serial RX+TX --> don't use for LEDS when serial-to-USB is needed
          case 43: FastLED.addLeds<ML_CHIPSET, 43 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 44: FastLED.addLeds<ML_CHIPSET, 44 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 45: FastLED.addLeds<ML_CHIPSET, 45 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 46: FastLED.addLeds<ML_CHIPSET, 46 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 47: FastLED.addLeds<ML_CHIPSET, 47 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
          case 48: FastLED.addLeds<ML_CHIPSET, 48 COLOR_ORDER_ARG>(leds, startLed, nrOfLights).setCorrection(TypicalLEDStrip) RGBW_CALL; break;
        #endif //CONFIG_IDF_TARGET_ESP32S3

        default: MB_LOGW(ML_TAG, "FastLEDPin assignment: pin not supported %d", sortedPin.pin);
        } //switch pinNr

      } //sortedPins    
    }

    FastLED.setMaxPowerInMilliWatts(1000 * maxPower); // 5v, 2000mA, to protect usb while developing

  }

  void  FastLEDDriverMod::loop() {
    if (FastLED.count()) {
      if (FastLED.getBrightness() != layerV->layerP->lights.header.brightness) {
        MB_LOGD(ML_TAG, "setBrightness %d", layerV->layerP->lights.header.brightness);
        FastLED.setBrightness(layerV->layerP->lights.header.brightness);
      }

      //FastLED Led Controllers
      CRGB correction = CRGB(layerV->layerP->lights.header.red, layerV->layerP->lights.header.green, layerV->layerP->lights.header.blue);
      CLEDController *pCur = CLEDController::head();
      while( pCur) {
          // ++x;
          if (pCur->getCorrection() != correction) {
            MB_LOGD(ML_TAG, "setColorCorrection r:%d, g:%d, b:%d (#:%d)", layerV->layerP->lights.header.red, layerV->layerP->lights.header.green, layerV->layerP->lights.header.blue, pCur->size());
            pCur->setCorrection(correction);
          }
          // pCur->size();
          pCur = pCur->next();
      }

      FastLED.show();
    }
  }

  void FastLEDDriverMod::updateControl(JsonObject control) {
    Node::updateControl(control);

    if (control["name"] == "maxPower") {
        MB_LOGD(ML_TAG, "setMaxPowerInMilliWatts %d", maxPower);
        FastLED.setMaxPowerInMilliWatts(1000 * maxPower); // 5v, 2000mA, to protect usb while developing
    }
  }

  void HUB75DriverMod::setup() {
    hasLayout = true; //so addLayout is called if needed
  }
  void HUB75DriverMod::addLayout() {
  }
  void HUB75DriverMod::loop() {
  }

  void  PhysicalDriverMod::setup() {

    hasLayout = true; //so addLayout is called if needed (not working yet, will work if reverse of initLeds is implemented)

    DriverNode::setup();
    #if HP_ALL_DRIVERS
      addControl(version, "Version", "text", 0, 30, true);
    #endif
  }

  void PhysicalDriverMod::addLayout() {
    #if HP_ALL_DRIVERS
      if (!lightPresetSaved || initDone || layerV->layerP->sortedPins.size() == 0) return;

      if (layerV->layerP->pass == 1) { //physical
        initDone = true;
        MB_LOGV(ML_TAG, "sortedPins #:%d", layerV->layerP->sortedPins.size());
        if (safeModeMB) {
          MB_LOGW(ML_TAG, "Safe mode enabled, not adding Physical driver");
          return;
        }

        int pins[NUMSTRIPS]; //max 16 pins
        int lengths[NUMSTRIPS];
        int nb_pins=0;

        for (const SortedPin &sortedPin : layerV->layerP->sortedPins) {
          MB_LOGD(ML_TAG, "sortedPin s:%d #:%d p:%d", sortedPin.startLed, sortedPin.nrOfLights, sortedPin.pin);
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
        MB_LOGD(ML_TAG, "pins[");
        for (int i=0; i<nb_pins; i++) {
          MB_LOGD(ML_TAG, ", %d (%d)", pins[i], lengths[i]);
        }
        MB_LOGD(ML_TAG, "]\n");

        if (nb_pins > 0) {

          uint8_t colorOrder = ORDER_GRB; //default

          if (lightPreset == 0) colorOrder = ORDER_RGB;
          else if (lightPreset == 1) colorOrder = ORDER_RBG;
          else if (lightPreset == 2) colorOrder = ORDER_GRB;
          else if (lightPreset == 3) colorOrder = ORDER_GBR;
          else if (lightPreset == 4) colorOrder = ORDER_BRG;
          else if (lightPreset == 5) colorOrder = ORDER_BGR;
          // else if (lightPreset == 6) colorOrder = ORDER_RGBW; // doesn't exist!
          else
            colorOrder = ORDER_GRBW; //best we can offer is 4 channels GRBW, to do: driver accepts more channels

          ledsDriver.initled(layerV->layerP->lights.channels, pins, lengths, nb_pins, (colorarrangment)colorOrder);
          #if ML_LIVE_MAPPING
            driver.setMapLed(&mapLed);
          #endif
          //void initled(uint8_t *leds, int *Pinsq, int *sizes, int num_strips, colorarrangment cArr)
          initDone = true; //so loop is called
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

  void PhysicalDriverMod::loop() {
    #if HP_ALL_DRIVERS
      if (!initDone) return;

      if (layerV->layerP->lights.header.isPositions == 0) {

        DriverNode::loop();

        if (ledsDriver.total_leds > 0)
          ledsDriver.showPixels(WAIT);
        // #endif
    }
    #else //ESP32_LEDSDRIVER
      if (!ledsDriver.initLedsDone) return;

      if (layerV->layerP->lights.header.isPositions == 0) {

          DriverNode::loop();

          ledsDriver.show();
      }
    #endif
  }

  void VirtualDriverMod::setup() {
    hasLayout = true; //so addLayout is called if needed (not working yet, will work if reverse of initLeds is implemented)
    DriverNode::setup();
  }
  void VirtualDriverMod::addLayout() {
  }
  void VirtualDriverMod::loop() {
    DriverNode::loop();
  }

#endif