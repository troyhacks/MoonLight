/**
    @title     MoonLight
    @file      VirtualLayer.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
           https://moonmodules.org/MoonLight/moonlight/overview/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact us for more information.
**/

#pragma once

#if FT_MOONLIGHT

#include <Arduino.h>
#include <vector>
#include <FastLED.h>

#include "PhysicalLayer.h"

enum mapType {
    m_zeroLights,
    m_oneLight,
    m_moreLights,
    m_count //keep as last entry
  };
  
struct PhysMap {
    union {
      struct {                 //condensed rgb
        uint16_t rgb14: 14;    //14 bits (554 RGB)
        uint8_t mapType:2;        //2 bits (4)
      }; //16 bits
      uint16_t indexP: 14;   //16384 one physical light (type==1) index to ledsP array
      uint16_t indexes:14;  //16384 multiple physical lights (type==2) index in std::vector<std::vector<uint16_t>> mappingTableIndexes;
    }; // 2 bytes  
    
    PhysMap() {
      // ESP_LOGV(TAG, "Constructor");
      mapType = m_zeroLights; // the default until indexP is added
      rgb14 = 0;
    }
  };

class VirtualLayer {

  public:

  uint16_t nrOfLights = 256;
  Coord3D size = {16,16,1}; //not 0,0,0 to prevent div0 eg in Octopus2D
  Coord3D middle = {8,8,1}; //not 0,0,0 to prevent div0 eg in Octopus2D

  //they will be reused to avoid fragmentation
  std::vector<PhysMap> mappingTable;
  std::vector<std::vector<uint16_t>> mappingTableIndexes;
  uint16_t mappingTableSizeUsed = 0; 
  uint16_t mappingTableIndexesSizeUsed = 0; 

  PhysicalLayer *layerP; //physical LEDs the virtual LEDs are mapped to
  std::vector<Node *> nodes;

  uint8_t fadeMin;

  VirtualLayer() {
    ESP_LOGV(TAG, "constructor");
  }
  
  ~VirtualLayer();

  void setup();
  void loop();

  void addIndexP(PhysMap &physMap, uint16_t indexP);

  uint16_t XYZ(Coord3D &position);
  
  uint16_t XYZUnModified(const Coord3D &position) const {
    return position.x + position.y*size.x + position.z * size.x * size.y;
  }

  void setRGB(const uint16_t indexV, CRGB color) {
    if (layerP->lights.header.offsetWhite != UINT8_MAX && layerP->lights.header.offsetWhite == layerP->lights.header.offsetRGB + 3) { //RGBW adjacent
      uint8_t rgbw[4];
      rgbw[3] = MIN(MIN(color.r, color.g), color.b) >> 1; //calc white channel, only do half of the minimum as not to turn off one of RGB completely (to be tweaked)
      rgbw[0] = color.red - rgbw[3]; //subtract from other channels
      rgbw[1] = color.green - rgbw[3];
      rgbw[2] = color.blue - rgbw[3];
      setLight(indexV, rgbw, layerP->lights.header.offsetRGB, sizeof(rgbw));
    }
    else 
      setLight(indexV, color.raw, layerP->lights.header.offsetRGB, sizeof(color));
  }
  void setRGB(Coord3D pos, CRGB color) { setRGB(XYZ(pos), color); }

  void setWhite(const uint16_t indexV, const uint8_t value) {
    if (layerP->lights.header.offsetWhite != UINT8_MAX)
      setLight(indexV, &value, layerP->lights.header.offsetWhite, sizeof(value));
  }
  void setWhite(Coord3D pos, const uint8_t value) { setWhite(XYZ(pos), value); }

  void setBrightness(const uint16_t indexV, uint8_t value) {
    value = (value * layerP->lights.header.brightness) / 255;
    if (layerP->lights.header.offsetBrightness != UINT8_MAX)
      setLight(indexV, &value, layerP->lights.header.offsetBrightness, sizeof(value));
  }
  void setBrightness(Coord3D pos, const uint8_t value) { setBrightness(XYZ(pos), value); }

  void setPan(const uint16_t indexV, const uint8_t value) {
    if (layerP->lights.header.offsetPan != UINT8_MAX)
      setLight(indexV, &value, layerP->lights.header.offsetPan, sizeof(value));
  }
  void setPan(Coord3D pos, const uint8_t value) { setPan(XYZ(pos), value); }

  void setTilt(const uint16_t indexV, const uint8_t value) {
    if (layerP->lights.header.offsetTilt != UINT8_MAX)
      setLight(indexV, &value, layerP->lights.header.offsetTilt, sizeof(value));
  }
  void setTilt(Coord3D pos, const uint8_t value) { setTilt(XYZ(pos), value); }

  void setZoom(const uint16_t indexV, const uint8_t value) {
    if (layerP->lights.header.offsetZoom != UINT8_MAX)
      setLight(indexV, &value, layerP->lights.header.offsetZoom, sizeof(value));
  }
  void setZoom(Coord3D pos, const uint8_t value) { setZoom(XYZ(pos), value); }

  void setRotate(const uint16_t indexV, const uint8_t value) {
    if (layerP->lights.header.offsetRotate != UINT8_MAX)
      setLight(indexV, &value, layerP->lights.header.offsetRotate, sizeof(value));
  }
  void setRotate(Coord3D pos, const uint8_t value) { setRotate(XYZ(pos), value); }

  void setGobo(const uint16_t indexV, const uint8_t value) {
    if (layerP->lights.header.offsetGobo != UINT8_MAX)
      setLight(indexV, &value, layerP->lights.header.offsetGobo, sizeof(value));
  }
  void setGobo(Coord3D pos, const uint8_t value) { setGobo(XYZ(pos), value); }

  void setRGB1(const uint16_t indexV, CRGB color) {
    if (layerP->lights.header.offsetRGB1 != UINT8_MAX)
      setLight(indexV, color.raw, layerP->lights.header.offsetRGB1, sizeof(color));
  }
  void setRGB1(Coord3D pos, CRGB color) { setRGB1(XYZ(pos), color); }

  void setRGB2(const uint16_t indexV, CRGB color) {
    if (layerP->lights.header.offsetRGB2 != UINT8_MAX)
      setLight(indexV, color.raw, layerP->lights.header.offsetRGB2, sizeof(color));
  }
  void setRGB2(Coord3D pos, CRGB color) { setRGB2(XYZ(pos), color); }

  void setRGB3(const uint16_t indexV, CRGB color) {
    if (layerP->lights.header.offsetRGB3 != UINT8_MAX)
      setLight(indexV, color.raw, layerP->lights.header.offsetRGB3, sizeof(color));
  }
  void setRGB3(Coord3D pos, CRGB color) { setRGB3(XYZ(pos), color); }

  void setBrightness2(const uint16_t indexV, uint8_t value) {
    value = (value * layerP->lights.header.brightness) / 255;
    if (layerP->lights.header.offsetBrightness2 != UINT8_MAX)
      setLight(indexV, &value, layerP->lights.header.offsetBrightness2, sizeof(value));
  }
  void setBrightness2(Coord3D pos, const uint8_t value) { setBrightness2(XYZ(pos), value); }

  void setLight(const uint16_t indexV, const uint8_t* channels, uint8_t offset, uint8_t length);

  CRGB getRGB(const uint16_t indexV) {
    return getLight<CRGB>(indexV, layerP->lights.header.offsetRGB);
  }
  CRGB getRGB(Coord3D pos) { return getRGB(XYZ(pos)); }

  uint8_t getWhite(const uint16_t indexV) {
    return getLight<uint8_t>(indexV, layerP->lights.header.offsetWhite);
  }
  uint8_t getWhite(Coord3D pos) { return getWhite(XYZ(pos)); }

  CRGB getRGB1(const uint16_t indexV) {
    return getLight<CRGB>(indexV, layerP->lights.header.offsetRGB1);
  }
  CRGB getRGB1(Coord3D pos) { return getRGB1(XYZ(pos)); }

  CRGB getRGB2(const uint16_t indexV) {
    return getLight<CRGB>(indexV, layerP->lights.header.offsetRGB2);
  }
  CRGB getRGB2(Coord3D pos) { return getRGB2(XYZ(pos)); }

  CRGB getRGB3(const uint16_t indexV) {
    return getLight<CRGB>(indexV, layerP->lights.header.offsetRGB3);
  }
  CRGB getRGB3(Coord3D pos) { return getRGB3(XYZ(pos)); }

  template <typename T>
  T getLight(const uint16_t indexV, uint8_t offset) const;

  //to be called in loop, if more then one effect
  // void setLightsToBlend(); //uses LEDs

  void fadeToBlackBy(const uint8_t fadeBy = 255);
  void fadeToBlackMin();

  void fill_solid(const CRGB& color);
  void fill_rainbow(const uint8_t initialhue, const uint8_t deltahue);

  void addLayoutPre();
  void addLayoutPost();
  
  //addLight is called by addLayout for each light in the layout
  void addLight(Coord3D position);

  void drawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, CRGB color, bool soft = false, uint8_t depth = UINT8_MAX);

  void drawLine3D(Coord3D a, Coord3D b, CRGB color, bool soft = false, uint8_t depth = UINT8_MAX) {
    drawLine3D(a.x, a.y, a.z, b.x, b.y, b.z, color, soft, depth);
  }
  //to do: merge with drawLine to support 2D and 3D
  void drawLine3D(uint8_t x1, uint8_t y1, uint8_t z1, uint8_t x2, uint8_t y2, uint8_t z2, CRGB color, bool soft = false, uint8_t depth = UINT8_MAX);

  void drawCircle(int cx, int cy, uint8_t radius, CRGB col, bool soft);

  //shift is used by drawText indicating which letter it is drawing
  void drawCharacter(unsigned char chr, int x = 0, int y = 0, uint8_t font = 0, CRGB col = CRGB::Red, uint16_t shiftPixel = 0, uint16_t shiftChr = 0);

  void drawText(const char * text, int x = 0, int y = 0, uint8_t font = 0, CRGB col = CRGB::Red, uint16_t shiftPixel = 0);

  Node *findLiveScriptNode(const char *animation);

};

#endif