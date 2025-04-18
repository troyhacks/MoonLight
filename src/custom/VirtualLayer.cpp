/**
    @title     MoonBase
    @file      VirtualLayer.h
    @repo      https://github.com/ewowi/MoonBase, submit changes to this file as PRs
    @Authors   https://github.com/ewowi/MoonBase/commits/main
    @Doc       https://ewowi.github.io/MoonBase/general/utilities/
    @Copyright © 2025 Github MoonBase Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
**/


#include "VirtualLayer.h"
#include "PhysicalLayer.h"

//convenience functions to call fastled functions out of the Leds namespace (there naming conflict)
void fastled_fadeToBlackBy(CRGB* leds, uint16_t num_leds, uint8_t fadeBy) {
  fadeToBlackBy(leds, num_leds, fadeBy);
}
void fastled_fill_solid( struct CRGB * targetArray, int numToFill, const CRGB& color) {
  fill_solid(targetArray, numToFill, color);
}
void fastled_fill_rainbow(struct CRGB * targetArray, int numToFill, uint8_t initialhue, uint8_t deltahue) {
  fill_rainbow(targetArray, numToFill, initialhue, deltahue);
}

void VirtualLayer::setPixelColor(const int indexV, const CRGB& color) {
  if (indexV < 0)
    return;
  else if (indexV < mappingTableSizeUsed) {
    switch (mappingTable[indexV].mapType) {
      case m_color:{
        mappingTable[indexV].rgb14 = ((min(color.r + 3, 255) >> 3) << 9) + 
                                     ((min(color.g + 3, 255) >> 3) << 4) + 
                                      (min(color.b + 7, 255) >> 4);
        break;
      }
      case m_onePixel: {
        uint16_t indexP = mappingTable[indexV].indexP;
        //temp fix for cube202020 (some curtains are bgr)
        // if (indexP > 2800) {
        //   fix->ledsP[indexP].r = color.b;
        //   fix->ledsP[indexP].g = color.g;
        //   fix->ledsP[indexP].b = color.r;
        // }
        // else
        layerP->leds[indexP] = layerP->pixelsToBlend[indexP]?blend(color, layerP->leds[indexP], layerP->globalBlend):color;
        break; }
      case m_morePixels:
        if (mappingTable[indexV].indexes < mappingTableIndexes.size())
          for (uint16_t indexP: mappingTableIndexes[mappingTable[indexV].indexes]) {
            // if (indexP > 2800) {
            //   fix->ledsP[indexP].r = color.b;
            //   fix->ledsP[indexP].g = color.g;
            //   fix->ledsP[indexP].b = color.r;
            // } else
            layerP->leds[indexP] = layerP->pixelsToBlend[indexP]?blend(color, layerP->leds[indexP], layerP->globalBlend): color;
          }
        else
          ESP_LOGW(TAG, "dev setPixelColor i:%d m:%d s:%d\n", indexV, mappingTable[indexV].indexes, mappingTableIndexes.size());
        break;
      default: ;
    }
  }
  else if (indexV < NUM_LEDS) //no projection
    layerP->leds[indexV] = layerP->pixelsToBlend[indexV]?blend(color, layerP->leds[indexV], layerP->globalBlend): color;
  // some operations will go out of bounds e.g. VUMeter, uncomment below lines if you wanna test on a specific effect
  // else //if (indexV != UINT16_MAX) //assuming UINT16_MAX is set explicitly (e.g. in XYZ)
  //   ESP_LOGW(TAG, " dev sPC %d >= %d", indexV, STARLIGHT_NUM_LEDS);
}

void VirtualLayer::setPixelsToBlend() {
  for (const std::vector<uint16_t>& mappingTableIndex: mappingTableIndexes) {
      for (const uint16_t indexP: mappingTableIndex)
      layerP->pixelsToBlend[indexP] = true;
    }
    for (const PhysMap &physMap: mappingTable) {
      if (physMap.mapType == m_onePixel)
      layerP->pixelsToBlend[physMap.indexP] = true;
    }
}

CRGB VirtualLayer::getPixelColor(const int indexV) const {
  if (indexV < 0)
    return CRGB::Black;
  else if (indexV < mappingTableSizeUsed) {
    switch (mappingTable[indexV].mapType) {
      case m_onePixel:
        return layerP->leds[mappingTable[indexV].indexP]; 
        break;
      case m_morePixels:
        return layerP->leds[mappingTableIndexes[mappingTable[indexV].indexes][0]]; //any will do as they are all the same
        break;
      default: // m_color:
        return CRGB((mappingTable[indexV].rgb14 >> 9) << 3, 
                    (mappingTable[indexV].rgb14 >> 4) << 3, 
                     mappingTable[indexV].rgb14       << 4);
        break;
    }
  }
  else if (indexV < NUM_LEDS) //no mapping
    return layerP->leds[indexV];
  else {
    // some operations will go out of bounds e.g. VUMeter, uncomment below lines if you wanna test on a specific effect
    // ppf(" dev gPC %d >= %d", indexV, STARLIGHT_MAXLEDS);
    return CRGB::Black;
  }
}

void VirtualLayer::fadeToBlackBy(const uint8_t fadeBy) {
  // if (effectDimension < projectionDimension) { //only process the effect pixels (so projections can do things with the other dimension)
  //   for (int y=0; y < ((effectDimension == _1D)?1:size.y); y++) { //1D effects only on y=0, 2D effects loop over y
  //     for (int x=0; x<size.x; x++) {
  //       CRGB color = getPixelColor({x,y,0});
  //       color.nscale8(255-fadeBy);
  //       setPixelColor({x,y,0}, color);
  //     }
  //   }
  // } else if (!projection || (layerP->layerV.size() == 1)) { //faster, else manual 
  //   fastled_fadeToBlackBy(layerP->leds, layerP->nrOfLeds, fadeBy);
  // } else {
    for (uint16_t index = 0; index < mappingTableSizeUsed; index++) {
      CRGB color = getPixelColor(index);
      color.nscale8(255-fadeBy);
      setPixelColor(index, color);
    }
  // }
}
