/**
    @title     MoonLight
    @file      Modifiers.h
    @repo      https://github.com/ewowi/MoonBase, submit changes to this file as PRs
    @Authors   https://github.com/ewowi/MoonBase/commits/main
    @Doc       https://ewowi.github.io/MoonBase/general/utilities/
    @Copyright © 2025 Github MoonBase Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
**/

class MultiplyModifier: public Node {
  const char * name() override {return "Multiply";}

  Coord3D proMulti = {2,2,2};
  bool    mirror = false;
  Coord3D originalSize;

  void modifyPixelsPre() override {
    layerV->size = (layerV->size + proMulti - Coord3D({1,1,1})) / proMulti; // Round up
    originalSize = layerV->size;
  }

  void modifyPixel(Coord3D &pixel) override {
    if (mirror) {
      Coord3D mirrors = pixel / originalSize; // Place the pixel in the right quadrant
      pixel = pixel % originalSize;
      if (mirrors.x %2 != 0) pixel.x = originalSize.x - 1 - pixel.x;
      if (mirrors.y %2 != 0) pixel.y = originalSize.y - 1 - pixel.y;
      if (mirrors.z %2 != 0) pixel.z = originalSize.z - 1 - pixel.z;
    }
    else pixel = pixel % originalSize;
  }
};

class MirrorModifier: public Node {
  const char * name() override {return "Mirror";}

  bool mirrorX = true;
  bool mirrorY = false;
  bool mirrorZ = false;
  Coord3D originalSize;

  void getControls(JsonArray controls) override {
    JsonObject control;
    control = controls.add<JsonObject>(); control["name"] = "mirrorX"; control["type"] = "Checkbox"; control["value"] = mirrorX;
    control = controls.add<JsonObject>(); control["name"] = "mirrorY"; control["type"] = "Checkbox"; control["value"] = mirrorY;
    control = controls.add<JsonObject>(); control["name"] = "mirrorZ"; control["type"] = "Checkbox"; control["value"] = mirrorZ;
  }
  
  void setControls(JsonArray controls) override {
    ESP_LOGD(TAG, "%s", controls[0].as<String>().c_str());
    ESP_LOGD(TAG, "%s", controls[1].as<String>().c_str());
    ESP_LOGD(TAG, "%s", controls[2].as<String>().c_str());
    mirrorX = controls[0]["value"];
    mirrorY = controls[1]["value"];
    mirrorZ = controls[2]["value"];
  }

  void modifyPixelsPre() override {
    if (mirrorX) layerV->size.x = (layerV->size.x + 1) / 2;
    if (mirrorY) layerV->size.y = (layerV->size.y + 1) / 2;
    if (mirrorZ) layerV->size.z = (layerV->size.z + 1) / 2;
    originalSize = layerV->size;
  }

  void modifyPixel(Coord3D &pixel) override {
    if (mirrorX && pixel.x >= originalSize.x) pixel.x = originalSize.x * 2 - 1 - pixel.x;
    if (mirrorY && pixel.y >= originalSize.y) pixel.y = originalSize.y * 2 - 1 - pixel.y;
    if (mirrorZ && pixel.z >= originalSize.z) pixel.z = originalSize.z * 2 - 1 - pixel.z;
  }
};

class PinwheelModifier: public Node {
  const char * name() override {return "Pinwheel";}

  uint8_t petals   = 60; // Initalize petal first for modifyPixel
  uint8_t swirlVal = 30;
  bool    reverse  = false;
  uint8_t symmetry = 1;
  uint8_t zTwist   = 0;

  void getControls(JsonArray controls) override {
    JsonObject control;
    control = controls.add<JsonObject>(); control["name"] = "petals"; control["type"] = "Range"; control["value"] = petals;
    control = controls.add<JsonObject>(); control["name"] = "swirlVal"; control["type"] = "Range"; control["value"] = swirlVal;
    control = controls.add<JsonObject>(); control["name"] = "reverse"; control["type"] = "Checkbox"; control["value"] = reverse;
    control = controls.add<JsonObject>(); control["name"] = "symmetry"; control["type"] = "Range"; control["value"] = symmetry;
    control = controls.add<JsonObject>(); control["name"] = "zTwist"; control["type"] = "Range"; control["value"] = zTwist;
  }
  
  void setControls(JsonArray controls) override {
    ESP_LOGD(TAG, "%s", controls[0].as<String>().c_str());
    ESP_LOGD(TAG, "%s", controls[1].as<String>().c_str());
    ESP_LOGD(TAG, "%s", controls[2].as<String>().c_str());
    ESP_LOGD(TAG, "%s", controls[3].as<String>().c_str());
    ESP_LOGD(TAG, "%s", controls[4].as<String>().c_str());
    petals = controls[0]["value"];
    swirlVal = controls[1]["value"];
    reverse = controls[2]["value"];
    symmetry = controls[3]["value"];
    zTwist = controls[4]["value"];
  }
  
  void modifyPixelsPre() override {
    // if (leds.projectionDimension > _1D && leds.effectDimension > _1D) {
      layerV->size.y = sqrt(sq(max(layerV->size.x - layerV->middle.x, layerV->middle.x)) + 
                            sq(max(layerV->size.y - layerV->middle.y, layerV->middle.y))) + 1; // Adjust y before x
      layerV->size.x = petals;
      layerV->size.z = 1;
    // }
    // else {
    //   layerV->size.x = petals;
    //   layerV->size.y = 1;
    //   layerV->size.z = 1;
    // }

  }

  void modifyPixel(Coord3D &pixel) override {
    // Coord3D mapped;
    // factors of 360
    const int FACTORS[24] = {360, 180, 120, 90, 72, 60, 45, 40, 36, 30, 24, 20, 18, 15, 12, 10, 9, 8, 6, 5, 4, 3, 2};
    // UI Variables
         
    const int dx = pixel.x - layerV->middle.x;
    const int dy = pixel.y - layerV->middle.y;
    const int swirlFactor = swirlVal == 0 ? 0 : hypot(dy, dx) * abs(swirlVal); // Only calculate if swirlVal != 0
    int angle = degrees(atan2(dy, dx)) + 180;  // 0 - 360
    
    if (swirlVal < 0) angle = 360 - angle; // Reverse Swirl

    int value = angle + swirlFactor + (zTwist * pixel.z);
    float petalWidth = symmetry / float(petals);
    value /= petalWidth;
    value %= petals;

    if (reverse) value = petals - value - 1; // Reverse Movement

    pixel.x = value;
    pixel.y = 0;
    // if (leds.effectDimension > _1D && leds.projectionDimension > _1D) {
      pixel.y = int(sqrt(sq(dx) + sq(dy))); // Round produced blank pixel
    // }
    pixel.z = 0;

    // if (pixel.x == 0 && pixel.y == 0 && pixel.z == 0) ppf("Pinwheel  Center: (%d, %d) SwirlVal: %d Symmetry: %d Petals: %d zTwist: %d\n", layerV->middle.x, layerV->middle.y, swirlVal, symmetry, petals, zTwist);
    // ppf("pixel %2d,%2d,%2d -> %2d,%2d,%2d Angle: %3d Petal: %2d\n", pixel.x, pixel.y, pixel.z, mapped.x, mapped.y, mapped.z, angle, value);
  }
};
