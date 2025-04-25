/**
    @title     MoonLight
    @file      Fixtures.h
    @repo      https://github.com/ewowi/MoonBase, submit changes to this file as PRs
    @Authors   https://github.com/ewowi/MoonBase/commits/main
    @Doc       https://ewowi.github.io/MoonBase/general/utilities/
    @Copyright © 2025 Github MoonBase Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
**/

class Panel16fixture: public Node {
  const char * name() override {return "Panel16";}

  uint8_t width = 16;
  uint8_t height = 16;

  void getControls(JsonArray controls) override {
    JsonObject control;
    control = controls.add<JsonObject>(); control["name"] = "width"; control["type"] = "Range"; control["value"] = width;
    control = controls.add<JsonObject>(); control["name"] = "height"; control["type"] = "Range"; control["value"] = height;
  }
  
  void setControls(JsonArray controls) override {
    ESP_LOGD(TAG, "%s", controls[0].as<String>().c_str());
    ESP_LOGD(TAG, "%s", controls[1].as<String>().c_str());
    width = controls[0]["value"];
    height = controls[1]["value"];
  }

  void setup() override {

    //create panel 16
    layerV->layerP->addPixelsPre();
    for (int x = 0; x<width; x++) {
      for (int y = 0; y<height; y++) {
        layerV->layerP->addPixel({x, (x%2)?y:height-1-y, 0});
      }
    }
    layerV->layerP->addPixelsPost();
  }
};
