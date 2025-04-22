/**
    @title     MoonLight
    @file      VirtualLayer.h
    @repo      https://github.com/ewowi/MoonBase, submit changes to this file as PRs
    @Authors   https://github.com/ewowi/MoonBase/commits/main
    @Doc       https://ewowi.github.io/MoonBase/general/utilities/
    @Copyright © 2025 Github MoonBase Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
**/

#pragma once

#include <Arduino.h>
#include <vector>
#include <FastLED.h>

#include "PhysicalLayer.h"

// #include "Effect.h"

// class PhysicalLayer; //Forward as VirtualLayer refers back to PhysicalLayer

enum mapType {
    m_color,
    m_onePixel,
    m_morePixels,
    m_count //keep as last entry
  };
  
struct PhysMap {
    union {
      struct {                 //condensed rgb
        uint16_t rgb14: 14;    //14 bits (554 RGB)
        byte mapType:2;        //2 bits (4)
      }; //16 bits
      uint16_t indexP: 14;   //16384 one physical pixel (type==1) index to ledsP array
      uint16_t indexes:14;  //16384 multiple physical pixels (type==2) index in std::vector<std::vector<uint16_t>> mappingTableIndexes;
    }; // 2 bytes  
    
    PhysMap() {
      // ESP_LOGD(TAG, "Constructor");
      mapType = m_color; // the default until indexP is added
      rgb14 = 0;
    }
  };

class VirtualLayer {

  public:

    Coord3D size = {16,16,1}; //not 0,0,0 to prevent div0 eg in Octopus2D

    std::vector<PhysMap> mappingTable;
    std::vector<std::vector<uint16_t>> mappingTableIndexes;

    //they will be reused to avoid fragmentation
    uint16_t nrOfLeds = 0;
    uint16_t mappingTableIndexesSizeUsed = 0; 

    PhysicalLayer *layerP; //physical leds the virtual leds are mapped to
    std::vector<Effect *> effects;
    // Effect *liveEffect = nullptr;
    std::vector<Projection *> projections;
  

    VirtualLayer() {
      ESP_LOGD(TAG, "constructor");
    }
    ~VirtualLayer() {
      ESP_LOGD(TAG, "destructor");
      fadeToBlackBy(255); //clear the leds

      effects.clear(); //call effect destructors
      projections.clear(); //call projection destructors

      //clear array of array of indexes
      for (std::vector<uint16_t> mappingTableIndex: mappingTableIndexes) {
        mappingTableIndex.clear();
      }
      mappingTableIndexes.clear();
      //clear mapping table
      mappingTable.clear();
    }
  
    bool loop();

    void addIndexP(PhysMap &physMap, uint16_t indexP);

    int XYZ(Coord3D pixel);
    
    int XYZUnprojected(const Coord3D &pixel) const {
      // if (pixel.x%2)
      //   return pixel.x + pixel.y * size.x + pixel.z * size.x * size.y;
      // else
      //   return pixel.x + (size.y-pixel.y-1)*size.x + pixel.z * size.x * size.y;
      // if (pixel.y%2)
      //   return (size.x - pixel.x - 1) + pixel.y * size.x + pixel.z * size.x * size.y;
      // else
        return pixel.x + pixel.y*size.x + pixel.z * size.x * size.y;
    }

    void setPixelColor(const Coord3D &pixel, const CRGB& color) {setPixelColor(XYZUnprojected(pixel), color);}
    void setPixelColor(const int indexV, const CRGB& color); //uses leds
    CRGB getPixelColor(const int indexV) const;
    void fadeToBlackBy(const uint8_t fadeBy);

    //to be called in loop, if more then one effect
    void setPixelsToBlend(); //uses leds

    void fill_solid(const CRGB& color);
    void fill_rainbow(const uint8_t initialhue, const uint8_t deltahue);

    void addPixelsPre();

    void addPixel(Coord3D pixel);

    void addPixelsPost();

};