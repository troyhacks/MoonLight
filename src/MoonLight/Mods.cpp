/**
    @title     MoonLight
    @file      Mods.cpp
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/moonlight/overview/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
**/

#if FT_MOONLIGHT

#include "Mods.h"

#include <ESP32SvelteKit.h>

#include <AsyncUDP.h>
#define ARTNET_DEFAULT_PORT 6454

void  ArtNetDriverMod::loop() {
  controllerIP[0] = WiFi.localIP()[0];
  controllerIP[1] = WiFi.localIP()[1];
  controllerIP[2] = WiFi.localIP()[2];
  controllerIP[3] = controllerIP3;

  if(!controllerIP) return;
  
  LightsHeader *header = &layerV->layerP->lights.header;
  if (header->isPositions != 0) return; //don't update if positions are sent

  //wait until the throttle FPS is reached
  unsigned long wait = 1000 / FPSLimiter + lastMillis - millis();
  if (wait > 0 && wait < 1000 / FPSLimiter)
    delay(wait); // delay in ms
  else
    ESP_LOGW(TAG, "wait %d", wait);
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
        packet_buffer[16] = packetSize >> 8;
        packet_buffer[17] = packetSize;

        // bulk copy the buffer range to the packet buffer after the header 
        memcpy(packet_buffer+18, (&layerV->layerP->lights.channels[0])+bufferOffset, packetSize); //start from the first byte of ledsP[0]

        //no brightness scaling for the time being
        for (int i = 18; i < packetSize+18; i+= header->channelsPerLight) {
            // set brightness all at once - seems slightly faster than scale8()?
            // for some reason, doing 3/4 at a time is 200 micros faster than 1 at a time.
            
            //if no brightness control and other colors, apply bri
            if (header->offsetBrightness == UINT8_MAX)
                for (int j=0; j < 3; j++) packet_buffer[i+j+header->offsetRGB] = (packet_buffer[i+j+header->offsetRGB] * header->brightness) >> 8;
            if (header->offsetRGB1 != UINT8_MAX && header->offsetBrightness == UINT8_MAX)
                for (int j=0; j < 3; j++) packet_buffer[i+j+header->offsetRGB1] = (packet_buffer[i+j+header->offsetRGB1] * header->brightness) >> 8;
            if (header->offsetRGB2 != UINT8_MAX && header->offsetBrightness == UINT8_MAX)
                for (int j=0; j < 3; j++) packet_buffer[i+j+header->offsetRGB2] = (packet_buffer[i+j+header->offsetRGB2] * header->brightness) >> 8;
            if (header->offsetRGB3 != UINT8_MAX && header->offsetBrightness == UINT8_MAX)
                for (int j=0; j < 3; j++) packet_buffer[i+j+header->offsetRGB3] = (packet_buffer[i+j+header->offsetRGB3] * header->brightness) >> 8;
            if (header->offsetWhite != UINT8_MAX && header->offsetBrightness == UINT8_MAX)
                packet_buffer[i+header->offsetWhite] = (packet_buffer[i+header->offsetWhite] * header->brightness) >> 8;

            //todo: correct values using RGB correction: header->red/green/blue. Need to know the color order first ...
        }

        bufferOffset += packetSize;
        
        if (!artnetudp.writeTo(packet_buffer, packetSize+18, controllerIP, ARTNET_DEFAULT_PORT)) {
            Serial.print("🐛");
            return; // borked
        }

        hardware_output_universe++;
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
        FastLED.setMaxPowerInMilliWatts(1000 * maxPower); // 5v, 2000mA, to protect usb while developing
        maxPowerSaved = maxPower;
      }

      //FastLED Led Controllers
      CRGB correction = CRGB(layerV->layerP->lights.header.red, layerV->layerP->lights.header.green, layerV->layerP->lights.header.blue);
      CLEDController *pCur = CLEDController::head();
      while( pCur) {
          // ++x;
          if (pCur->getCorrection() != correction) {
            ESP_LOGD(TAG, "setColorCorrection r:%d, g:%d, b:%d", layerV->layerP->lights.header.red, layerV->layerP->lights.header.green, layerV->layerP->lights.header.blue);
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

  #if HP_PHYSICAL_DRIVER || HP_PHYSICAL_DRIVER_S3
    #define NUMSTRIPS 16 //can this be changed e.g. when we have 20 pins?
    #define NUM_LEDS_PER_STRIP 256 //could this be removed from driver lib as makes not so much sense
    #if CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32S2
      #include "I2SClockLessLedDriveresp32s3.h"
      static I2SClocklessLedDriveresp32S3 driverP; //    sizeof(driver) = 1080K !
    #elif CONFIG_IDF_TARGET_ESP32
      #include "I2SClocklessLedDriver.h"
      static I2SClocklessLedDriver driverP;
    #endif
  #endif

  void  PhysicalDriverMod::setup() {

    hasLayout = true; //so addLayout is called if needed

    addControl(setMaxPowerBrightnessFactor, "maxPower", "number");
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

    #if HP_PHYSICAL_DRIVER || HP_PHYSICAL_DRIVER_S3
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
              driverP.initled((uint8_t*)  layerV->layerP->lights.channels, pins, nb_pins, lengths[0]); //s3 doesn't support lengths so we pick the first
              // colorOrder ??
              //void initled( uint8_t * leds, int * pins, int numstrip, int NUM_LED_PER_STRIP)
            #else
              driverP.initled((uint8_t*) layerV->layerP->lights.channels, pins, lengths, nb_pins, (colorarrangment)colorOrder);
              #if ML_LIVE_MAPPING
                driver.setMapLed(&mapLed);
              #endif
              //void initled(uint8_t *leds, int *Pinsq, int *sizes, int num_strips, colorarrangment cArr)
            #endif
          //   Variable("Fixture", "brightness").triggerEvent(onChange, UINT8_MAX, true); //set brightness (init is true so bri value not send via udp)
            driverP.setBrightness(setMaxPowerBrightnessFactor / 256); //not brighter then the set limit (WIP)
          }
        #endif
      #endif
    }
  }

  void PhysicalDriverMod::loop() {
    if (!initDone) return;

    if (layerV->layerP->lights.header.isPositions == 0) {

      #if HP_PHYSICAL_DRIVER || HP_PHYSICAL_DRIVER_S3
        #if CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32
          if (driverP._brightness != layerV->layerP->lights.header.brightness * setMaxPowerBrightnessFactor >> 8) {
            ESP_LOGD(TAG, "setBrightness %d", layerV->layerP->lights.header.brightness);
            driverP.setBrightness(layerV->layerP->lights.header.brightness * setMaxPowerBrightnessFactor >> 8);
          }

          if (driverP._gammar != layerV->layerP->lights.header.red/255.0 || driverP._gammag != layerV->layerP->lights.header.green/255.0 || driverP._gammab != layerV->layerP->lights.header.blue/255.0) {
            ESP_LOGD(TAG, "setColorCorrection r:%d, g:%d, b:%d", layerV->layerP->lights.header.red, layerV->layerP->lights.header.green, layerV->layerP->lights.header.blue);
            driverP.setGamma(layerV->layerP->lights.header.red/255.0, layerV->layerP->lights.header.green/255.0, layerV->layerP->lights.header.blue/255.0);
          }

          #if CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32S2
            if (driverP.ledsbuff != nullptr)
              driverP.show();
          #else
            if (driverP.total_leds > 0)
              driverP.showPixels(WAIT);
          #endif
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