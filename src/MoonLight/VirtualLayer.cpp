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

#if FT_MOONLIGHT

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

VirtualLayer::~VirtualLayer() {
  ESP_LOGD(TAG, "destructor");
  fadeToBlackBy(255); //clear the leds

  for (Node *node: nodes) {
    node->destructor();
    delete node;
  }
  nodes.clear();

  //clear array of array of indexes
  for (std::vector<uint16_t> mappingTableIndex: mappingTableIndexes) {
    mappingTableIndex.clear();
  }
  mappingTableIndexes.clear();
  //clear mapping table
  mappingTable.clear();
}

void VirtualLayer::setup() {
  for (Node *node: nodes) {
    node->setup();
  }
}

void VirtualLayer::loop() {
  fadeToBlackMin();

  for (Node *node: nodes) {
    if (node->on)
      node->loop();
  }
};

void VirtualLayer::resetMapping() {

  for (std::vector<uint16_t> mappingTableIndex: mappingTableIndexes) {
    mappingTableIndex.clear();
  }
  mappingTableIndexesSizeUsed = 0; //do not clear mappingTableIndexes, reuse it

  for (size_t i = 0; i < mappingTable.size(); i++) { //this cannot be removed ...
    mappingTable[i] = PhysMap();
  }
  mappingTableSizeUsed = 0;

}

void VirtualLayer::addIndexP(PhysMap &physMap, uint16_t indexP) {
  // ESP_LOGD(TAG, "i:%d t:%d s:%d i:%d", indexP, physMap.mapType, mappingTableIndexes.size(), physMap.indexes);
  switch (physMap.mapType) {
    case m_color:
    // case m_rgbColor:
      physMap.indexP = indexP;
      physMap.mapType = m_oneLight;
      break;
    case m_oneLight: {
      uint16_t oldIndexP = physMap.indexP;
      // change to m_moreLights and add the old indexP and new indexP to the multiple indexP array
      mappingTableIndexesSizeUsed++; //add a new slot in the mappingTableIndexes
      if (mappingTableIndexes.size() < mappingTableIndexesSizeUsed)
        mappingTableIndexes.push_back({oldIndexP, indexP});
      else
        mappingTableIndexes[mappingTableIndexesSizeUsed-1] = {oldIndexP, indexP};

        physMap.indexes = mappingTableIndexesSizeUsed - 1; //array position
        physMap.mapType = m_moreLights;
      break; }
    case m_moreLights:
      // mappingTableIndexes.reserve(physMap.indexes+1);
      mappingTableIndexes[physMap.indexes].push_back(indexP);
      // ESP_LOGD(TAG, " more %d", mappingTableIndexes.size());
      break;
  }
  // ESP_LOGD(TAG, "\n");
}
uint16_t VirtualLayer::XYZ(Coord3D &position) {

  //XYZ modifiers
  for (Node *node: nodes) { //e.g. random or scrolling or rotate modifier
    if (node->hasModifier && node->on)
      node->modifyXYZ(position); //modifies the position
  }

  return XYZUnprojected(position);
}

void VirtualLayer::setLightColor(const uint16_t indexV, const CRGB& color) {
  // Serial.printf(" %d: %d,%d,%d", indexV, color.r, color.g, color.b);
  if (indexV < mappingTableSizeUsed) {
    // ESP_LOGD(TAG, "setLightColor %d %d %d %d", indexV, color.r, color.g, color.b, mappingTableSizeUsed);
    switch (mappingTable[indexV].mapType) {
      case m_color:{
        mappingTable[indexV].rgb14 = ((min(color.r + 3, 255) >> 3) << 9) + 
                                     ((min(color.g + 3, 255) >> 3) << 4) + 
                                      (min(color.b + 7, 255) >> 4);
        break;
      }
      case m_oneLight: {
        uint16_t indexP = mappingTable[indexV].indexP;
        //temp fix for cube202020 (some curtains are bgr)
        // if (indexP > 2800) {
        //   fix->ledsP[indexP].r = color.b;
        //   fix->ledsP[indexP].g = color.g;
        //   fix->ledsP[indexP].b = color.r;
        // }
        // else
        // layerP->lights.leds[indexP] = layerP->lightsToBlend[indexP]?blend(color, layerP->lights.leds[indexP], layerP->globalBlend):color;
        layerP->lights.leds[indexP] = color;
        break; }
      case m_moreLights:
        if (mappingTable[indexV].indexes < mappingTableIndexes.size())
          for (uint16_t indexP: mappingTableIndexes[mappingTable[indexV].indexes]) {
            // if (indexP > 2800) {
            //   fix->ledsP[indexP].r = color.b;
            //   fix->ledsP[indexP].g = color.g;
            //   fix->ledsP[indexP].b = color.r;
            // } else
            // layerP->lights.leds[indexP] = layerP->lightsToBlend[indexP]?blend(color, layerP->lights.leds[indexP], layerP->globalBlend): color;
            layerP->lights.leds[indexP] = color;
          }
        else
          ESP_LOGW(TAG, "dev setLightColor i:%d m:%d s:%d", indexV, mappingTable[indexV].indexes, mappingTableIndexes.size());
        break;
      default: ;
    }
  }
  else if (indexV < MAX_LEDS) {//no mapping
    // layerP->lights.leds[indexV] = layerP->lightsToBlend[indexV]?blend(color, layerP->lights.leds[indexV], layerP->globalBlend): color;
    layerP->lights.leds[indexV] = color;
    // layerP->lights.dmxChannels[indexV] = (byte*)&color;
  }
  // some operations will go out of bounds e.g. VUMeter, uncomment below lines if you wanna test on a specific effect
  // else //if (indexV != UINT16_MAX) //assuming UINT16_MAX is set explicitly (e.g. in XYZ)
  //   ESP_LOGW(TAG, " dev setLight %d >= %d", indexV, MAX_LEDS);
}

CRGB VirtualLayer::getLightColor(const uint16_t indexV) const {
  if (indexV < mappingTableSizeUsed) {
    switch (mappingTable[indexV].mapType) {
      case m_oneLight:
        return layerP->lights.leds[mappingTable[indexV].indexP]; 
        break;
      case m_moreLights:
        return layerP->lights.leds[mappingTableIndexes[mappingTable[indexV].indexes][0]]; //any will do as they are all the same
        break;
      default: // m_color:
        return CRGB((mappingTable[indexV].rgb14 >> 9) << 3, 
                    (mappingTable[indexV].rgb14 >> 4) << 3, 
                     mappingTable[indexV].rgb14       << 4);
        break;
    }
  }
  else if (indexV < MAX_LEDS) //no mapping
    return layerP->lights.leds[indexV];
  else {
    // some operations will go out of bounds e.g. VUMeter, uncomment below lines if you wanna test on a specific effect
    // ESP_LOGD(TAG, " dev gPC %d >= %d", indexV, MAX_LEDS);
    return CRGB::Black;
  }
}

// void VirtualLayer::setLightsToBlend() {
//   for (const std::vector<uint16_t>& mappingTableIndex: mappingTableIndexes) {
//       for (const uint16_t indexP: mappingTableIndex)
//       layerP->lightsToBlend[indexP] = true;
//     }
//     for (const PhysMap &physMap: mappingTable) {
//       if (physMap.mapType == m_oneLight)
//       layerP->lightsToBlend[physMap.indexP] = true;
//     }
// }

void VirtualLayer::fadeToBlackBy(const uint8_t fadeBy) {
  fadeMin = fadeMin?MIN(fadeMin, fadeBy): fadeBy;
}

void VirtualLayer::fadeToBlackMin() {
  if (fadeMin > 0) { //at least one fadeBy

    uint8_t fadeBy = fadeMin;// + fadeMax / div;
    // if (effectDimension < projectionDimension) { //only process the effect lights (so modifiers can do things with the other dimension)
    //   for (int y=0; y < ((effectDimension == _1D)?1:size.y); y++) { //1D effects only on y=0, 2D effects loop over y
    //     for (int x=0; x<size.x; x++) {
    //       CRGB color = getLightColor({x,y,0});
    //       color.nscale8(255-fadeBy);
    //       setLightColor({x,y,0}, color);
    //     }
    //   }
    // } else 
    if (layerP->layerV.size() == 1) { //faster, else manual 
      fastled_fadeToBlackBy(layerP->lights.leds, layerP->lights.header.nrOfLights, fadeBy);
    } else {
      for (uint16_t index = 0; index < nrOfLights; index++) {
        CRGB color = getLightColor(index);
        color.nscale8(255-fadeBy);
        setLightColor(index, color);
      }
    }
    //reset fade
    fadeMin = 0; //no fade
  }
}

void VirtualLayer::fill_solid(const CRGB& color) {
  // if (effectDimension < projectionDimension) { //only process the effect lights (so modifiers can do things with the other dimension)
  //   for (int y=0; y < ((effectDimension == _1D)?1:size.y); y++) { //1D effects only on y=0, 2D effects loop over y
  //     for (int x=0; x<size.x; x++) {
  //       setLightColor({x,y,0}, color);
  //     }
  //   }
  // } else 
  if (layerP->layerV.size() == 1) { //faster, else manual 
    fastled_fill_solid(layerP->lights.leds, layerP->lights.header.nrOfLights, color);
  } else {
    for (uint16_t index = 0; index < nrOfLights; index++)
      setLightColor(index, color);
  }
}

void VirtualLayer::fill_rainbow(const uint8_t initialhue, const uint8_t deltahue) {
  // if (effectDimension < projectionDimension) { //only process the effect lights (so modifiers can do things with the other dimension)
  //   CHSV hsv;
  //   hsv.hue = initialhue;
  //   hsv.val = 255;
  //   hsv.sat = 240;
  //   for (int y=0; y < ((effectDimension == _1D)?1:size.y); y++) { //1D effects only on y=0, 2D effects loop over y
  //     for (int x=0; x<size.x; x++) {
  //       setLightColor({x,y,0}, hsv);
  //       hsv.hue += deltahue;
  //     }
  //   }
  // } else 
  if (layerP->layerV.size() == 1) { //faster, else manual 
    fastled_fill_rainbow(layerP->lights.leds, layerP->lights.header.nrOfLights, initialhue, deltahue);
  } else {
    CHSV hsv;
    hsv.hue = initialhue;
    hsv.val = 255;
    hsv.sat = 240;

    for (uint16_t index = 0; index < nrOfLights; index++) {
      setLightColor(index, hsv);
      hsv.hue += deltahue;
    }
  }
}

void VirtualLayer::addLayoutPre() {

  resetMapping();

  nrOfLights = 0;
  size = layerP->lights.header.size; //start with the physical size

  //modifiers
  for (Node *node: nodes) {
    if (node->hasModifier && node->on)
      node->modifyLayout();
  }
}

void VirtualLayer::addLight(Coord3D position) {

  //modifiers
  for (Node *node: nodes) {
    if (node->hasModifier && node->on)
      node->modifyLight(position);
  }

  uint16_t indexV = XYZUnprojected(position);
  if (indexV >= nrOfLights) {
    mappingTable.resize(indexV + 1); //make sure the index fits
    nrOfLights = indexV + 1;
    mappingTableSizeUsed = nrOfLights;
  }
  addIndexP(mappingTable[indexV], layerP->lights.header.nrOfLights);
}

void VirtualLayer::addLayoutPost() {
  // prepare logging:
  uint16_t nrOfPhysical = 0;
  uint16_t nrOfPhysicalM = 0;
  uint16_t nrOfColor = 0;
  for (size_t i = 0; i< nrOfLights; i++) {
    PhysMap &map = mappingTable[i];
    switch (map.mapType) {
      case m_color:
        nrOfColor++;
        break;
      case m_oneLight:
        // ESP_LOGD(TAG,"%d mapping =1: #ledsP : %d", i, map.indexP);
        nrOfPhysical++;
        break;
      case m_moreLights:
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

  ESP_LOGD(TAG, "V:%d x %d x %d = v:%d = 0:%d + 1:%d + m:%d (p:%d)", size.x, size.y, size.z, nrOfLights, nrOfColor, nrOfPhysical, mappingTableIndexesSizeUsed, nrOfPhysicalM);

}

#endif //FT_MOONLIGHT