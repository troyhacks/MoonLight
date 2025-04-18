/**
    @title     MoonBase
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

#include <Arduino.h>
#include <vector>
#include "FastLED.h"
#include "Utilities.h"

#define NUM_LEDS 8192 //physical leds

#include "VirtualLayer.h"

class Node {

};

class EffectNode : public Node {

    public:
   
};

class PhysicalLayer {

    public:

    CRGB leds[NUM_LEDS];
    uint16_t nrOfLeds = 256;

    std::vector<bool> pixelsToBlend; //this is a 1-bit vector !!! overlapping effects will blend
    uint8_t globalBlend = 128; // to do add as UI control...

    uint32_t indexP;

    std::vector<VirtualLayer> layerV;

    PhysicalLayer() {
        ESP_LOGD(TAG, "constructor");
        initPixelsToBlend();

        layerV.push_back(VirtualLayer());
        layerV[0].layerP = this;
    }

    static void addPin(uint8_t pinNr) {
        ESP_LOGD(TAG, "addPin %d", pinNr);
    }

    void addPixelsPre() {
        ESP_LOGD(TAG, "addPixelsPre");
        indexP = 0;
        //init mappingtable
        //dealloc pins

        //correct ledsP if > 255 (in case of circles)
        // maxFactor = max(max(max(fixSize2.x, fixSize2.y), fixSize2.z) / 256.0, 1.0);

        // set ledsPExtended
        // ledsPExtended.type = 1; //definition, stop effects loop
        // ledsPExtended.ledFactor *= maxFactor;
    
        //for each effect node
        //  set virtual size based on physical start and end, ... scaling ...
        //  modifiers can change the size...
    }

    void addPixel(Coord3D pixel) {
        ESP_LOGD(TAG, "addPixel %d,%d,%d", pixel.x, pixel.y, pixel.z);

        //add the pixel in ledsP (for preview)
        // ledsP[indexP].r = pixel.x / maxFactor;
        // ledsP[indexP].g = pixel.y / maxFactor;
        // ledsP[indexP].b = pixel.z / maxFactor;

        //for each effect node
        //  modifiers can change the pixel...
        //  indexV = XYZUnprojected(pixel); //using virtual size first...
        //  mappingTable[indexV].addIndexP(effect Node, indexP);

        indexP++;
    }

    void addPixelsPost() {
        ESP_LOGD(TAG, "addPixelsPost");
        //calculate physical size (pass1, pass2)

        initPixelsToBlend();
        //driver init // alloc pins
    }

    //AI generated
    void sinusEffect(CRGB* leds, uint16_t numLeds, uint8_t hueOffset = 0, uint8_t brightness = 255, uint16_t speed = 10) {
        static uint16_t phase = 0; // Tracks the phase of the sine wave
        // ESP_LOGD(TAG, "sinusEffect %d %d %d %d", numLeds, hueOffset, brightness, speed);
    
        for (uint16_t i = 0; i < numLeds; i++) {
            // Calculate the sine wave value for the current LED
            uint8_t wave = sin8((i * 255 / numLeds) + phase);
    
            // Map the sine wave value to a color hue
            uint8_t hue = wave + hueOffset;
    
            // Set the LED color using the calculated hue
            leds[i] = CHSV(hue, 255, brightness);
        }
    
        // Increment the phase to animate the wave
        phase += speed;
    }

    bool effectFrame(const char * animation) {
        bool showLeds = false;
        if (equal(animation, "Solid")) {
            fill_solid(leds, nrOfLeds, CRGB::White);
            showLeds = true;
        } else if (equal(animation, "Random")) {
            fadeToBlackBy(leds, nrOfLeds, 70);
            leds[random16(nrOfLeds)] = CRGB(255, random8(), 0);
            showLeds = true;
        } else if (equal(animation, "Sinelon")) {
            fadeToBlackBy(leds, nrOfLeds, 20);
            uint8_t bpm = 60;
            int pos = beatsin16( bpm, 0, 255 );
            leds[pos] += CHSV( millis()/50, 255, 255); //= CRGB(255, random8(), 0);
            showLeds = true;
        } else if (equal(animation, "Rainbow")) {
            static uint8_t hue = 0;
            fill_rainbow(leds, nrOfLeds, hue++, 7);
            showLeds = true;
        } else if (equal(animation, "Sinus")) {
            fadeToBlackBy(leds, nrOfLeds, 70);
            sinusEffect(leds, nrOfLeds, millis() / 10, 255, 5);
            showLeds = true;
        } else if (equal(animation, "Lissajous")) {

            uint8_t xFrequency = 64;// = leds.effectControls.read<uint8_t>();
            uint8_t fadeRate = 128;// = leds.effectControls.read<uint8_t>();
            uint8_t speed = 128;// = leds.effectControls.read<uint8_t>();
            CRGBPalette16 palette = PartyColors_p;

            // layerV[0].fadeToBlackBy(fadeRate);
            fadeToBlackBy(leds, nrOfLeds, fadeRate);
            uint_fast16_t phase = millis() * speed / 256;  // allow user to control rotation speed, speed between 0 and 255!
            Coord3D locn = {0,0,0};
            for (int i=0; i < 256; i ++) {
                //WLEDMM: stick to the original calculations of xlocn and ylocn
                locn.x = sin8(phase/2 + (i*xFrequency)/64);
                locn.y = cos8(phase/2 + i*2);
                locn.x = (layerV[0].size.x < 2) ? 1 : (map(2*locn.x, 0,511, 0,2*(layerV[0].size.x-1)) +1) /2;    // softhack007: "*2 +1" for proper rounding
                locn.y = (layerV[0].size.y < 2) ? 1 : (map(2*locn.y, 0,511, 0,2*(layerV[0].size.y-1)) +1) /2;    // "leds.size.y > 2" is needed to avoid div/0 in map()
                layerV[0].setPixelColor(locn, ColorFromPalette(palette, millis()/100+i, 255));
            }
            showLeds = true;
        } else if (equal(animation, "Lines")) {
            uint8_t bpm = 60;
            int   frameNr;

            fadeToBlackBy(leds, nrOfLeds, 255);

            Coord3D pos = {0,0,0};
            pos.x = map(beat16( bpm), 0, UINT16_MAX, 0, layerV[0].size.x ); //instead of call%width

            // ESP_LOGE(TAG, "pos %d %d %d", layerV[0].size.x, layerV[0].size.y, layerV[0].size.z);
      
            for (pos.y = 0; pos.y < layerV[0].size.y; pos.y++) {
              int colorNr = (frameNr / layerV[0].size.y) % 3;
            //   leds[pos] = colorNr == 0?CRGB::Red:colorNr == 1?CRGB::Green:CRGB::Blue;
              layerV[0].setPixelColor(pos, colorNr == 0?CRGB::Red:colorNr == 1?CRGB::Green:CRGB::Blue);
            }
      
            pos = {0,0,0};
            pos.y = map(beat16( bpm), 0, UINT16_MAX, 0, layerV[0].size.y ); //instead of call%height
            for (pos.x = 0; pos.x <  layerV[0].size.x; pos.x++) {
              int colorNr = (frameNr / layerV[0].size.x) % 3;
            //   leds[pos] = colorNr == 0?CRGB::Red:colorNr == 1?CRGB::Green:CRGB::Blue;
              layerV[0].setPixelColor(pos, colorNr == 0?CRGB::Red:colorNr == 1?CRGB::Green:CRGB::Blue);
            }
            (frameNr)++;

            showLeds = true;
        } else {
            //Done by live script (Yves)
        }
        return showLeds;
    }

    // to be called in setup, if more then one effect
    void initPixelsToBlend() {
        pixelsToBlend.reserve(nrOfLeds);

        for (uint16_t indexP=0; indexP < pixelsToBlend.size(); indexP++)
          pixelsToBlend[indexP] = false;
    }

};