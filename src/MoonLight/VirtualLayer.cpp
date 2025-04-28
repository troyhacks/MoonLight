/**
    @title     MoonLight
    @file      VirtualLayer.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/general/utilities/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
**/


#include "VirtualLayer.h"
#include "PhysicalLayer.h"
#include "Nodes.h"

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

void VirtualLayer::setup() {
  for (Node *node: nodes) {
    node->setup();
  }
}

void VirtualLayer::loop() {
  for (Node *node: nodes) {
    if (node->on)
      node->loop();
  }

  //fadeToBlackBy(lowest value);

};

void VirtualLayer::addIndexP(PhysMap &physMap, uint16_t indexP) {
  // ESP_LOGD(TAG, "i:%d t:%d s:%d i:%d", indexP, physMap.mapType, mappingTableIndexes.size(), physMap.indexes);
  switch (physMap.mapType) {
    case m_color:
    // case m_rgbColor:
      physMap.indexP = indexP;
      physMap.mapType = m_onePixel;
      break;
    case m_onePixel: {
      uint16_t oldIndexP = physMap.indexP;
      // change to more_pixels and add the old indexP and new indexP to the multiple indexP array
      mappingTableIndexesSizeUsed++; //add a new slot in the mappingTableIndexes
      if (mappingTableIndexes.size() < mappingTableIndexesSizeUsed)
        mappingTableIndexes.push_back({oldIndexP, indexP});
      else
        mappingTableIndexes[mappingTableIndexesSizeUsed-1] = {oldIndexP, indexP};

        physMap.indexes = mappingTableIndexesSizeUsed - 1; //array position
        physMap.mapType = m_morePixels;
      break; }
    case m_morePixels:
      // mappingTableIndexes.reserve(physMap.indexes+1);
      mappingTableIndexes[physMap.indexes].push_back(indexP);
      // ESP_LOGD(TAG, " more %d", mappingTableIndexes.size());
      break;
  }
  // ESP_LOGD(TAG, "\n");
}
uint16_t VirtualLayer::XYZ(Coord3D &pixel) {

  //modifiers
  for (Node *node: nodes) { //e.g. random or scrolling or rotate modifier
    if (node->hasModifier && node->on)
      node->modifyXYZ(pixel); //modifies the pixel
  }

  return XYZUnprojected(pixel);
}

void VirtualLayer::setPixelColor(const uint16_t indexV, const CRGB& color) {
  // Serial.printf(" %d: %d,%d,%d", indexV, color.r, color.g, color.b);
  if (indexV < mappingTable.size()) {
    // ESP_LOGD(TAG, "setPixelColor %d %d %d %d", indexV, color.r, color.g, color.b, mappingTable.size());
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
          ESP_LOGW(TAG, "dev setPixelColor i:%d m:%d s:%d", indexV, mappingTable[indexV].indexes, mappingTableIndexes.size());
        break;
      default: ;
    }
  }
  else if (indexV < NUM_LEDS) {//no mapping
    layerP->leds[indexV] = layerP->pixelsToBlend[indexV]?blend(color, layerP->leds[indexV], layerP->globalBlend): color;
  }
  // some operations will go out of bounds e.g. VUMeter, uncomment below lines if you wanna test on a specific effect
  // else //if (indexV != UINT16_MAX) //assuming UINT16_MAX is set explicitly (e.g. in XYZ)
  //   ESP_LOGW(TAG, " dev sPC %d >= %d", indexV, STARLIGHT_NUM_LEDS);
}

CRGB VirtualLayer::getPixelColor(const uint16_t indexV) const {
  if (indexV < mappingTable.size()) {
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
    // ESP_LOGD(TAG, " dev gPC %d >= %d", indexV, STARLIGHT_MAXLEDS);
    return CRGB::Black;
  }
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

void VirtualLayer::fadeToBlackBy(const uint8_t fadeBy) {
  // if (effectDimension < projectionDimension) { //only process the effect pixels (so modifiers can do things with the other dimension)
  //   for (int y=0; y < ((effectDimension == _1D)?1:size.y); y++) { //1D effects only on y=0, 2D effects loop over y
  //     for (int x=0; x<size.x; x++) {
  //       CRGB color = getPixelColor({x,y,0});
  //       color.nscale8(255-fadeBy);
  //       setPixelColor({x,y,0}, color);
  //     }
  //   }
  // } else 
  if (layerP->layerV.size() == 1) { //faster, else manual 
    fastled_fadeToBlackBy(layerP->leds, layerP->nrOfLeds, fadeBy);
  } else {
    for (uint16_t index = 0; index < nrOfLeds; index++) {
      CRGB color = getPixelColor(index);
      color.nscale8(255-fadeBy);
      setPixelColor(index, color);
    }
  }
}

void VirtualLayer::fill_solid(const CRGB& color) {
  // if (effectDimension < projectionDimension) { //only process the effect pixels (so modifiers can do things with the other dimension)
  //   for (int y=0; y < ((effectDimension == _1D)?1:size.y); y++) { //1D effects only on y=0, 2D effects loop over y
  //     for (int x=0; x<size.x; x++) {
  //       setPixelColor({x,y,0}, color);
  //     }
  //   }
  // } else 
  if (layerP->layerV.size() == 1) { //faster, else manual 
    fastled_fill_solid(layerP->leds, layerP->nrOfLeds, color);
  } else {
    for (uint16_t index = 0; index < nrOfLeds; index++)
      setPixelColor(index, color);
  }
}

void VirtualLayer::fill_rainbow(const uint8_t initialhue, const uint8_t deltahue) {
  // if (effectDimension < projectionDimension) { //only process the effect pixels (so modifiers can do things with the other dimension)
  //   CHSV hsv;
  //   hsv.hue = initialhue;
  //   hsv.val = 255;
  //   hsv.sat = 240;
  //   for (int y=0; y < ((effectDimension == _1D)?1:size.y); y++) { //1D effects only on y=0, 2D effects loop over y
  //     for (int x=0; x<size.x; x++) {
  //       setPixelColor({x,y,0}, hsv);
  //       hsv.hue += deltahue;
  //     }
  //   }
  // } else 
  if (layerP->layerV.size() == 1) { //faster, else manual 
    fastled_fill_rainbow(layerP->leds, layerP->nrOfLeds, initialhue, deltahue);
  } else {
    CHSV hsv;
    hsv.hue = initialhue;
    hsv.val = 255;
    hsv.sat = 240;

    for (uint16_t index = 0; index < nrOfLeds; index++) {
      setPixelColor(index, hsv);
      hsv.hue += deltahue;
    }
  }
}

void VirtualLayer::addPixelsPre() {
  for (std::vector<uint16_t> mappingTableIndex: mappingTableIndexes) {
    mappingTableIndex.clear();
  }
  mappingTableIndexesSizeUsed = 0; //do not clear mappingTableIndexes, reuse it

  for (size_t i = 0; i < mappingTable.size(); i++) {
    mappingTable[i] = PhysMap();
  }
  nrOfLeds = 0;

  size = layerP->size; //physical size

  //modifiers
  for (Node *node: nodes) {
    if (node->hasModifier && node->on)
      node->modifyPixelsPre();
  }
}

void VirtualLayer::addPixel(Coord3D pixel) {

  //modifiers
  for (Node *node: nodes) {
    if (node->hasModifier && node->on)
      node->modifyPixel(pixel);
  }

  uint16_t indexV = XYZUnprojected(pixel);
  if (indexV >= nrOfLeds) {
    mappingTable.resize(indexV + 1); //make sure the index fits
    nrOfLeds = indexV + 1;
  }
  addIndexP(mappingTable[indexV], layerP->indexP);
}

void VirtualLayer::addPixelsPost() {
  // prepare logging:
  uint16_t nrOfPhysical = 0;
  uint16_t nrOfPhysicalM = 0;
  uint16_t nrOfColor = 0;
  for (size_t i = 0; i< nrOfLeds; i++) {
    PhysMap &map = mappingTable[i];
    switch (map.mapType) {
      case m_color:
        nrOfColor++;
        break;
      case m_onePixel:
        // ESP_LOGD(TAG,"%d mapping =1: #ledsP : %d", i, map.indexP);
        nrOfPhysical++;
        break;
      case m_morePixels:
        // Char<32> str;
        for (uint16_t indexP: mappingTableIndexes[map.indexes]) {
          // str += indexP;
          nrOfPhysicalM++;
        }
        // ESP_LOGD(TAG, "%d mapping >1: #ledsP : %s", i, str.c_str());
        break;
    }
    // else
    //   ESP_LOGD(TAG, "%d no mapping\n", x);
  }

  ESP_LOGD(TAG, "V:%d x %d x %d = v:%d = 0:%d + 1:%d + m:%d (p:%d)", size.x, size.y, size.z, nrOfLeds, nrOfColor, nrOfPhysical, mappingTableIndexesSizeUsed, nrOfPhysicalM);

}
