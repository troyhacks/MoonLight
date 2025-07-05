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
    // node->destructor();
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

  //set brightness default to global brightness
  if ( layerP->lights.header.offsetBrightness != UINT8_MAX) {
    for (int i = 0; i < nrOfLights; i++) {
      setBrightness(i, 255); //will be corrected with globalbrighness
      setBrightness2(i, 255); //will be corrected with globalbrighness
    }
  }

  for (Node *node: nodes) {
    if (node->on)
      node->loop();
  }

  if (requestMapPhysical) {
    ESP_LOGD(TAG, "mapLayout physical requested");
    
    layerP->pass = 1;
    mapLayout();

    requestMapPhysical = false;
  }

  if (requestMapVirtual) {
    ESP_LOGD(TAG, "mapLayout virtual requested");

    layerP->pass = 2;
    mapLayout();
    
    requestMapVirtual = false;
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

  return XYZUnModified(position);
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

void VirtualLayer::setLight(const uint16_t indexV, const uint8_t* channels, uint8_t offset, uint8_t length) {
  if (indexV < mappingTableSizeUsed) {
    // ESP_LOGD(TAG, "setLightColor %d %d %d %d", indexV, color.r, color.g, color.b, mappingTableSizeUsed);
    switch (mappingTable[indexV].mapType) {
      case m_color:{
        //only room for storing colors
        if (length == 3) {
          mappingTable[indexV].rgb14 = ((min(channels[0] + 3, 255) >> 3) << 9) + 
                                      ((min(channels[1] + 3, 255) >> 3) << 4) + 
                                      (min(channels[2] + 7, 255) >> 4);
        }
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
        // Serial.printf(" (%d %d,%d,%d %d %d %d)", indexP, channels[0], channels[1], channels[2], layerP->lights.header.channelsPerLight, start, length);
        memcpy(&layerP->lights.channels[indexP*layerP->lights.header.channelsPerLight + offset], channels, length);
        // memcpy(&layerP->lights.channels[indexP*3], channels, 3);
  
        // &layerP->lights.channels[indexP*sizeof(T)] = valueAsBytes;
        break; }
      case m_moreLights:
        if (mappingTable[indexV].indexes < mappingTableIndexes.size())
          for (uint16_t indexP: mappingTableIndexes[mappingTable[indexV].indexes]) {
            // if (indexP > 2800) {
            //   fix->ledsP[indexP].r = color.b;
            //   fix->ledsP[indexP].g = color.g;
            //   fix->ledsP[indexP].b = color.r;
            // } else
            memcpy(&layerP->lights.channels[indexP*layerP->lights.header.channelsPerLight + offset], channels, length);
          }
        else
          ESP_LOGW(TAG, "dev setLightColor i:%d m:%d s:%d", indexV, mappingTable[indexV].indexes, mappingTableIndexes.size());
        break;
      default: ;
    }
  }
  else if (indexV * layerP->lights.header.channelsPerLight + offset + length < MAX_CHANNELS) {//no mapping
    memcpy(&layerP->lights.channels[indexV*layerP->lights.header.channelsPerLight + offset], channels, length);
  }
    // layerP->lights.dmxChannels[indexV] = (byte*)&color;
  // some operations will go out of bounds e.g. VUMeter, uncomment below lines if you wanna test on a specific effect
  // else //if (indexV != UINT16_MAX) //assuming UINT16_MAX is set explicitly (e.g. in XYZ)
  //   ESP_LOGW(TAG, " dev setLight %d >= %d", indexV, MAX_LEDS);
}

template <typename T>
T VirtualLayer::getLight(const uint16_t indexV, uint8_t offset) const {
  if (indexV < mappingTableSizeUsed) {
    switch (mappingTable[indexV].mapType) {
      case m_oneLight: {
        T *result = (T*)&layerP->lights.channels[mappingTable[indexV].indexP * layerP->lights.header.channelsPerLight + offset];
        return *result; //return the color as CRGB
        break; }
      case m_moreLights: {
        T *result = (T*)&layerP->lights.channels[mappingTableIndexes[mappingTable[indexV].indexes][0] * layerP->lights.header.channelsPerLight + offset]; //any will do as they are all the same
        return *result; //return the color as CRGB
        break; }
      default: // m_color:
        if (layerP->lights.header.channelsPerLight == 3) {
          T result;
          ((uint8_t*)&result)[0] = (mappingTable[indexV].rgb14 >> 9) << 3;
          ((uint8_t*)&result)[1] = (mappingTable[indexV].rgb14 >> 4) << 3;
          ((uint8_t*)&result)[2] = (mappingTable[indexV].rgb14)      << 4;
          return result;
        }
        else
          return T(); //not implemented yet
        break;
    }
  }
  else if (indexV * layerP->lights.header.channelsPerLight + offset + 3 < MAX_CHANNELS) { //no mapping
    T *result = (T*)&layerP->lights.channels[indexV * layerP->lights.header.channelsPerLight + offset];
    return *result; //return the color as CRGB
  } else {
    // some operations will go out of bounds e.g. VUMeter, uncomment below lines if you wanna test on a specific effect
    // ESP_LOGD(TAG, " dev gPC %d >= %d", indexV, MAX_LEDS);
    return T();
  }
}

//fadeToBlackBy will only do primary RGB colors
void VirtualLayer::fadeToBlackBy(const uint8_t fadeBy) {
  fadeMin = fadeMin?MIN(fadeMin, fadeBy): fadeBy;
}

void VirtualLayer::fadeToBlackMin() {
  if (fadeMin > 0) { //at least one fadeBy

    uint8_t fadeBy = fadeMin;// + fadeMax / div;
    // if (effectDimension < projectionDimension) { //only process the effect lights (so modifiers can do things with the other dimension)
    //   for (int y=0; y < ((effectDimension == _1D)?1:size.y); y++) { //1D effects only on y=0, 2D effects loop over y
    //     for (int x=0; x<size.x; x++) {
    //       CRGB color = getLight({x,y,0});
    //       color.nscale8(255-fadeBy);
    //       setRGB({x,y,0}, color);
    //     }
    //   }
    // } else 
    if (layerP->lights.header.channelsPerLight == 3 && layerP->layerV.size() == 1) { //CRGB lights
      fastled_fadeToBlackBy(layerP->lights.leds, layerP->lights.header.nrOfLights, fadeBy);
    } else { //multichannel lights
      for (uint16_t index = 0; index < nrOfLights; index++) {
        CRGB color = getRGB(index); //direct access to the channels
        color.nscale8(255-fadeBy);
        setRGB(index, color);
        if (layerP->lights.header.offsetWhite != UINT8_MAX) {
          uint8_t white = getWhite(index); //direct access to the channels
          white = (white * fadeBy) >> 8;
          setWhite(index, white);
        }
        if (layerP->lights.header.offsetRGB1 != UINT8_MAX) {
          CRGB color = getRGB1(index); //direct access to the channels
          color.nscale8(255-fadeBy);
          setRGB1(index, color);
        }
        if (layerP->lights.header.offsetRGB2 != UINT8_MAX) {
          CRGB color = getRGB2(index); //direct access to the channels
          color.nscale8(255-fadeBy);
          setRGB2(index, color);
        }
        if (layerP->lights.header.offsetRGB3 != UINT8_MAX) {
          CRGB color = getRGB3(index); //direct access to the channels
          color.nscale8(255-fadeBy);
          setRGB3(index, color);
        }
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
  //       setRGB({x,y,0}, color);
  //     }
  //   }
  // } else 
  if (layerP->lights.header.channelsPerLight == 3 && layerP->layerV.size() == 1) { //faster, else manual 
    fastled_fill_solid(layerP->lights.leds, layerP->lights.header.nrOfLights, color);
  } else {
    for (uint16_t index = 0; index < nrOfLights; index++)
      setRGB(index, color);
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
  //       setRGB({x,y,0}, hsv);
  //       hsv.hue += deltahue;
  //     }
  //   }
  // } else 
  if (layerP->lights.header.channelsPerLight == 3 && layerP->layerV.size() == 1) { //faster, else manual 
    fastled_fill_rainbow(layerP->lights.leds, layerP->lights.header.nrOfLights, initialhue, deltahue);
  } else {
    CHSV hsv;
    hsv.hue = initialhue;
    hsv.val = 255;
    hsv.sat = 240;

    for (uint16_t index = 0; index < nrOfLights; index++) {
      setRGB(index, hsv);
      hsv.hue += deltahue;
    }
  }
}

void VirtualLayer::mapLayout() {
  layerP->addLayoutPre();
  for (Node *node: nodes) {
    if (node->on && node->hasLayout)
      layerP->lights.header.resetOffsets();
      node->addLayout();
  }
  layerP->addLayoutPost();
}

void VirtualLayer::addLayoutPre() {

  resetMapping();

  nrOfLights = 0;
  size = layerP->lights.header.size; //start with the physical size

  //modifiers
  for (Node *node: nodes) {
    if (node->hasModifier && node->on)
      node->modifySize();
  }
}

void VirtualLayer::addLight(Coord3D position) {

  //modifiers
  for (Node *node: nodes) {
    if (node->hasModifier && node->on)
      node->modifyPosition(position);
  }

  uint16_t indexV = XYZUnModified(position);
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

    #if FT_LIVESCRIPT
        Node *VirtualLayer::findLiveScriptNode(const char *animation) {
            for (Node *node : nodes) {
                if (node && node->isLiveScriptNode()) {
                    LiveScriptNode *liveScriptNode = (LiveScriptNode *)node;
                    if (equal(liveScriptNode->animation, animation)) {
                        ESP_LOGD(TAG, "found %s", animation);
                        return liveScriptNode;
                    }
                }
            }
            return nullptr;
        }
    #endif
  

#endif //FT_MOONLIGHT