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

#include <ESP32SvelteKit.h>

#if HP_PHYSICAL_DRIVER
  #include "ESP32-LedsDriver.h"
  #define MAX_PINS 20 // this is also defined in ESP32-LedsDriver.h...
  #if CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32S2
    static PhysicalDriverESP32S3 ledsDriver; //    sizeof(driver) = 1080K !
  #elif CONFIG_IDF_TARGET_ESP32
    static PhysicalDriverESP32D0 ledsDriver; //    sizeof(driver) = 1080K !
  #endif
#endif

#include <AsyncUDP.h>
#define ARTNET_DEFAULT_PORT 6454

void DriverNode::setup() {
  hasLayout = true; //so addLayout is called if needed

  addControl(maxPower, "maxPower", "number", 0, 100);
  JsonObject property = addControl(lightPreset, "lightPreset", "select"); 
  JsonArray values = property["values"].to<JsonArray>();
  values.add("GRBW");
  values.add("RGB");
  values.add("RBG");
  values.add("GRB");
  values.add("GBR");
  values.add("BRG");
  values.add("BGR");
  values.add("GRBW6");
  values.add("TroyMH15");
  values.add("TroyMH32");
  values.add("WowiMH24");
}

void DriverNode::loop() {

  LightsHeader *header = &layerV->layerP->lights.header;

  //use ledsDriver LUT for super efficient leds dimming 🔥 (used by reOrderAndDimRGB)

  uint8_t brightness = (header->offsetBrightness == UINT8_MAX)?header->brightness:255; //set brightness to 255 if offsetBrightness is set (fixture will do its own brightness)

  if (brightness != brightnessSaved || maxPowerSaved != maxPower) {
    //Use FastLED for setMaxPowerInMilliWatts stuff
    uint8_t correctedBrightness = calculate_max_brightness_for_power_mW((CRGB *)&layerV->layerP->lights.channels, layerV->layerP->lights.header.nrOfLights, brightness, maxPower * 1000);
    ESP_LOGD(TAG, "setBrightmess b:%d + p:%d -> cb:%d", brightness, maxPower, correctedBrightness);
    ledsDriver.setBrightness(correctedBrightness);
    brightnessSaved = brightness;
    maxPowerSaved = maxPower;
  }

  CRGB correction;
  uint8_t white;
  ledsDriver.getColorCorrection(correction.red, correction.green, correction.blue, white);
  if (correction.red != layerV->layerP->lights.header.red || correction.green != layerV->layerP->lights.header.green || correction.blue != layerV->layerP->lights.header.blue) {
    ESP_LOGD(TAG, "setColorCorrection r:%d, g:%d, b:%d (%d %d %d)", layerV->layerP->lights.header.red, layerV->layerP->lights.header.green, layerV->layerP->lights.header.blue, correction.red, correction.green, correction.blue);
    ledsDriver.setColorCorrection(layerV->layerP->lights.header.red, layerV->layerP->lights.header.green, layerV->layerP->lights.header.blue);
  }

}

void DriverNode::updateControl(JsonObject control) {
  Node::updateControl(control);

  LightsHeader *header = &layerV->layerP->lights.header;

  if (control["name"] == "lightPreset") {
    uint8_t oldChannelsPerLight = header->channelsPerLight;

    header->resetOffsets(); //after this addLayout is called of different layout nodes which might set additional offsets (WIP)

    if (lightPreset == 0) { header->channelsPerLight = 4; header->offsetRed = 1; header->offsetGreen = 0; header->offsetBlue = 2; header->offsetWhite = 3;} //GRBW
    else if (lightPreset == 1) { header->channelsPerLight = 3; header->offsetRed = 0; header->offsetGreen = 1; header->offsetBlue = 2; header->offsetWhite = UINT8_MAX;} //RGB
    else if (lightPreset == 2) { header->channelsPerLight = 3; header->offsetRed = 0; header->offsetGreen = 2; header->offsetBlue = 1; header->offsetWhite = UINT8_MAX;} //RBG
    else if (lightPreset == 3) { header->channelsPerLight = 3; header->offsetRed = 1; header->offsetGreen = 0; header->offsetBlue = 2; header->offsetWhite = UINT8_MAX;} //GRB
    else if (lightPreset == 4) { header->channelsPerLight = 3; header->offsetRed = 2; header->offsetGreen = 0; header->offsetBlue = 1; header->offsetWhite = UINT8_MAX;} //GBR
    else if (lightPreset == 5) { header->channelsPerLight = 3; header->offsetRed = 1; header->offsetGreen = 2; header->offsetBlue = 0; header->offsetWhite = UINT8_MAX;} //BRG
    else if (lightPreset == 6) { header->channelsPerLight = 3; header->offsetRed = 2; header->offsetGreen = 1; header->offsetBlue = 0; header->offsetWhite = UINT8_MAX;} //BGR
    else if (lightPreset == 7) { header->channelsPerLight = 6; header->offsetRed = 1; header->offsetGreen = 0; header->offsetBlue = 2; header->offsetWhite = 3;} //GRBW6
    else if (lightPreset == 8) { //troy32
      layerV->layerP->lights.header.channelsPerLight = 32;
      layerV->layerP->lights.header.offsetRGB = 9;
      layerV->layerP->lights.header.offsetRGB1 = 13;
      layerV->layerP->lights.header.offsetRGB2 = 17;
      layerV->layerP->lights.header.offsetRGB3 = 24;
      layerV->layerP->lights.header.offsetPan = 0;
      layerV->layerP->lights.header.offsetTilt = 2;
      layerV->layerP->lights.header.offsetZoom = 5;
      layerV->layerP->lights.header.offsetBrightness = 6;
    }
    else if (lightPreset == 9) { //troyMH15
      layerV->layerP->lights.header.channelsPerLight = 15; //set channels per light to 15 (RGB + Pan + Tilt + Zoom + Brightness)
      layerV->layerP->lights.header.offsetRGB = 10; //set offset for RGB lights in DMX map
      layerV->layerP->lights.header.offsetPan = 0;
      layerV->layerP->lights.header.offsetTilt = 1;
      layerV->layerP->lights.header.offsetZoom = 7;
      layerV->layerP->lights.header.offsetBrightness = 8; //set offset for brightness
      layerV->layerP->lights.header.offsetGobo = 5; //set offset for color wheel in DMX map
      layerV->layerP->lights.header.offsetBrightness2 = 3; //set offset for color wheel brightness in DMX map    } //BGR
    }
    else if (lightPreset == 10) { //wowiMH24
      layerV->layerP->lights.header.channelsPerLight = 24;
      layerV->layerP->lights.header.offsetPan = 0;
      layerV->layerP->lights.header.offsetTilt = 1;
      layerV->layerP->lights.header.offsetBrightness = 3;
      layerV->layerP->lights.header.offsetRGB = 4;
      layerV->layerP->lights.header.offsetRGB1 = 8;
      layerV->layerP->lights.header.offsetRGB2 = 12;
      layerV->layerP->lights.header.offsetZoom = 17;
    }

    ESP_LOGD(TAG, "setLightPreset %d (cPL:%d, o:%d,%d,%d,%d)", lightPreset, header->channelsPerLight, header->offsetRed, header->offsetGreen, header->offsetBlue, header->offsetWhite);

    if (ledsDriver.initLedsDone) {
      
      ledsDriver.setOffsets(layerV->layerP->lights.header.offsetRed, layerV->layerP->lights.header.offsetGreen, layerV->layerP->lights.header.offsetBlue, layerV->layerP->lights.header.offsetWhite);

      if (oldChannelsPerLight != header->channelsPerLight)
        restartNeeded = true; //in case 
    }

    lightPresetSaved = true;
  }
}

void DriverNode::reOrderAndDimRGB(uint8_t *packetRGBChannel, uint8_t *lightsRGBChannel) {
  LightsHeader *header = &layerV->layerP->lights.header;
  // uint8_t brightness = (header->offsetBrightness == UINT8_MAX)?header->brightness:255;

  //use ledsDriver.__rbg_map[0]; for super fast brightness and gamma correction! see secondPixel in ESP32-LedDriver!
  //apply the LUT to the RGB channels !

  packetRGBChannel[header->offsetRed] = ledsDriver.__red_map[lightsRGBChannel[0]];
  packetRGBChannel[header->offsetGreen] = ledsDriver.__green_map[lightsRGBChannel[1]];
  packetRGBChannel[header->offsetBlue] = ledsDriver.__blue_map[lightsRGBChannel[2]];
}

void ArtNetDriverMod::setup() {

  DriverNode::setup();

  addControl(controllerIP3, "controllerIP", "number");
  addControl(FPSLimiter, "FPSLimiter", "number");
  addControl(nrOfOutputs, "nrOfOutputs", "number");
  addControl(channelsPerOutput, "channelsPerOutput", "number", 0, 65538);
  addControl(universesPerOutput, "universesPerOutput", "number");
}

void ArtNetDriverMod::loop() {

  DriverNode::loop();

  LightsHeader *header = &layerV->layerP->lights.header;

  //continue with Art-Net code
  controllerIP[0] = WiFi.localIP()[0];
  controllerIP[1] = WiFi.localIP()[1];
  controllerIP[2] = WiFi.localIP()[2];
  controllerIP[3] = controllerIP3;

  if(!controllerIP) return;
  
  if (header->isPositions != 0) return; //don't update if positions are sent

  //wait until the throttle FPS is reached
  unsigned long wait = 1000 / FPSLimiter + lastMillis - millis();
  if (wait > 0 && wait < 1000 / FPSLimiter)
    delay(wait); // delay in ms
  // else
  //   ESP_LOGW(TAG, "wait %d", wait);
  lastMillis = millis();

  // calculate the number of UDP packets we need to send

  uint8_t packet_buffer[ART_NET_HEADER_SIZE + 6 + 512];
  memcpy(packet_buffer, ART_NET_HEADER, 12); // copy in the Art-Net header.

  AsyncUDP artnetudp;// AsyncUDP so we can just blast packets.

  uint_fast16_t bufferOffset = 0;
  uint_fast16_t hardware_output_universe = 0;
  
  sequenceNumber++;

  if (sequenceNumber == 0) sequenceNumber = 1; // just in case, as 0 is considered "Sequence not in use"
  if (sequenceNumber > 255) sequenceNumber = 1;
  
  for (uint_fast16_t hardware_output = 0; hardware_output < nrOfOutputs; hardware_output++) { //loop over all outputs
      
    if (bufferOffset > header->nrOfLights * header->channelsPerLight) {
        // This stop is reached if we don't have enough pixels for the defined Art-Net output.
        Serial.print("🥒");
        return; // stop when we hit end of LEDs
    }

    hardware_output_universe = hardware_output * universesPerOutput;

    uint_fast16_t channels_remaining = channelsPerOutput;

    while (channels_remaining > 0) { //loop per universe
        const uint_fast16_t ARTNET_CHANNELS_PER_PACKET = 510; // 512/4=128 RGBW LEDs, 510/3=170 RGB LEDs

        uint_fast16_t packetSize = ARTNET_CHANNELS_PER_PACKET;

        if (channels_remaining < ARTNET_CHANNELS_PER_PACKET) {
            packetSize = channels_remaining;
            channels_remaining = 0;
        } else {
            channels_remaining -= packetSize;
        }

        // set the parts of the Art-Net packet header that change:
        packet_buffer[12] = sequenceNumber;
        packet_buffer[14] = hardware_output_universe;
        packet_buffer[16] = packetSize >> 8; //uint16_t over 2 bytes
        packet_buffer[17] = packetSize;//uint16_t over 2 bytes

        // bulk copy the buffer range to the packet buffer after the header 
        memcpy(packet_buffer+18, (&layerV->layerP->lights.channels[0])+bufferOffset, packetSize); //start from the first byte of ledsP[0]

        //no brightness scaling for the time being
        for (int i = 18; i < packetSize+18; i+= header->channelsPerLight) { //fill the packet with lights (all channels of the light)
            // set brightness all at once - seems slightly faster than scale8()?
            // for some reason, doing 3/4 at a time is 200 micros faster than 1 at a time.

            // uint8_t *firstPacketBufferChannel = &packet_buffer[i];
            // uint8_t *firstLightsChannel = &layerV->layerP->lights.channels[bufferOffset];
            
            //reorder RGB and apply brightness and gamma correction

            //use ledsDriver LUT for super efficient leds dimming 🔥 (used by reOrderAndDimRGB)

            reOrderAndDimRGB(&packet_buffer[i+header->offsetRGB], &layerV->layerP->lights.channels[bufferOffset + header->offsetRGB]);
            //   firstPacketBufferChannel[header->offsetRGB + header->offsetRed] = (firstLightsChannel[header->offsetRGB] * brightness * header->red) >> 16;
            //   firstPacketBufferChannel[header->offsetRGB + header->offsetGreen] = (firstLightsChannel[header->offsetRGB+1] * brightness * header->green) >> 16;
            //   firstPacketBufferChannel[header->offsetRGB + header->offsetBlue] = (firstLightsChannel[header->offsetRGB+2] * brightness * header->blue) >> 16;

            if (header->offsetRGB1 != UINT8_MAX )
              reOrderAndDimRGB(&packet_buffer[i+header->offsetRGB1], &layerV->layerP->lights.channels[bufferOffset + header->offsetRGB1]);
            if (header->offsetRGB2 != UINT8_MAX )
              reOrderAndDimRGB(&packet_buffer[i+header->offsetRGB2], &layerV->layerP->lights.channels[bufferOffset + header->offsetRGB2]);
            if (header->offsetRGB3 != UINT8_MAX )
              reOrderAndDimRGB(&packet_buffer[i+header->offsetRGB3], &layerV->layerP->lights.channels[bufferOffset + header->offsetRGB3]);

            if (header->offsetWhite != UINT8_MAX && header->offsetBrightness == UINT8_MAX && header->brightness != 255)
                packet_buffer[i+header->offsetWhite] = (packet_buffer[i+header->offsetWhite] * header->brightness) >> 8;

            //todo: correct values using RGB correction: header->red/green/blue. Need to know the color order first ...
        }

        bufferOffset += packetSize; //multiple of channelsPerLight
        
        if (!artnetudp.writeTo(packet_buffer, packetSize+18, controllerIP, ARTNET_DEFAULT_PORT)) {
            Serial.print("🐛");
            return; // borked
        }

        hardware_output_universe++; //each packet is one universe
    }
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
    hasLayout = true; //so addLayout is called if needed

    addControl(maxPower, "maxPower", "number", 0, 100);
  }

  void  FastLEDDriverMod::addLayout() {
    if (layerV->layerP->sortedPins.size() == 0) return;

    if (layerV->layerP->pass == 1) { //physical
      // if (safeModeMB) {
      //     ESP_LOGW(TAG, "Safe mode enabled, not adding FastLED driver");
      //     return;
      // }

      ESP_LOGD(TAG, "sortedPins #:%d", layerV->layerP->sortedPins.size());
      for (const SortedPin &sortedPin : layerV->layerP->sortedPins) {
        ESP_LOGD(TAG, "sortpins s:%d #:%d p:%d", sortedPin.startLed, sortedPin.nrOfLights, sortedPin.pin);

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

        default: ESP_LOGW(TAG, "FastLEDPin assignment: pin not supported %d", sortedPin.pin);
        } //switch pinNr

      } //sortedPins    
    }

    FastLED.setMaxPowerInMilliWatts(1000 * maxPower); // 5v, 2000mA, to protect usb while developing

  }

  void  FastLEDDriverMod::loop() {
    if (FastLED.count()) {
      if (FastLED.getBrightness() != layerV->layerP->lights.header.brightness) {
        ESP_LOGD(TAG, "setBrightness %d", layerV->layerP->lights.header.brightness);
        FastLED.setBrightness(layerV->layerP->lights.header.brightness);
      }

      if (maxPowerSaved != maxPower) {
        ESP_LOGD(TAG, "setMaxPowerInMilliWatts %d", maxPower);
        FastLED.setMaxPowerInMilliWatts(1000 * maxPower); // 5v, 2000mA, to protect usb while developing
        maxPowerSaved = maxPower;
      }

      //FastLED Led Controllers
      CRGB correction = CRGB(layerV->layerP->lights.header.red, layerV->layerP->lights.header.green, layerV->layerP->lights.header.blue);
      CLEDController *pCur = CLEDController::head();
      while( pCur) {
          // ++x;
          if (pCur->getCorrection() != correction) {
            ESP_LOGD(TAG, "setColorCorrection r:%d, g:%d, b:%d (#:%d)", layerV->layerP->lights.header.red, layerV->layerP->lights.header.green, layerV->layerP->lights.header.blue, pCur->size());
            pCur->setCorrection(correction);
          }
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

  void  PhysicalDriverMod::setup() {

    hasLayout = true; //so addLayout is called if needed

    DriverNode::setup();
  }

  void PhysicalDriverMod::addLayout() {
    if (!lightPresetSaved || ledsDriver.initLedsDone || layerV->layerP->sortedPins.size() == 0) return;

    if (layerV->layerP->pass == 1) { //physical
      ESP_LOGD(TAG, "sortedPins #:%d", layerV->layerP->sortedPins.size());
      if (safeModeMB) {
          ESP_LOGW(TAG, "Safe mode enabled, not adding Physical driver");
          return;
      }

    #if HP_PHYSICAL_DRIVER
        #if CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32
          int numPins = 0;
          PinConfig pinConfig[MAX_PINS];


          for (const SortedPin &sortedPin : layerV->layerP->sortedPins) {
            ESP_LOGD(TAG, "sortedPin s:%d #:%d p:%d", sortedPin.startLed, sortedPin.nrOfLights, sortedPin.pin);
            if (numPins < MAX_PINS) {
              pinConfig[numPins].gpio = sortedPin.pin;
              pinConfig[numPins].nrOfLeds =  sortedPin.nrOfLights;
              numPins++;
            }
          }

          ESP_LOGD(TAG, "pins[");
          for (int i=0; i<numPins; i++) {
            ESP_LOGD(TAG, ", %d", pinConfig[i].gpio);
          }

          if (numPins > 0) {

            ledsDriver.initLeds(layerV->layerP->lights.channels, pinConfig, numPins, layerV->layerP->lights.header.channelsPerLight, layerV->layerP->lights.header.offsetRed, layerV->layerP->lights.header.offsetGreen, layerV->layerP->lights.header.offsetBlue, layerV->layerP->lights.header.offsetWhite); //102 is GRB

            #if ML_LIVE_MAPPING
              driver.setMapLed(&mapLed);
            #endif
          }
        #endif
      #endif
    }
  }

  void PhysicalDriverMod::loop() {
    #if HP_PHYSICAL_DRIVER
      if (!ledsDriver.initLedsDone) return;

      if (layerV->layerP->lights.header.isPositions == 0) {

          DriverNode::loop();

          ledsDriver.show();
      }
    #endif
  }

  void VirtualDriverMod::setup() {
    hasLayout = true; //so addLayout is called if needed
    DriverNode::setup();
  }
  void VirtualDriverMod::addLayout() {
  }
  void VirtualDriverMod::loop() {
    DriverNode::loop();
  }

#endif