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
  const char * name() override {return "Panel🚥";}

  uint8_t width = 16;
  uint8_t height = 16;
  bool snake = true;

  void getControls(JsonArray controls) override {
    hasFixDef = true;
    JsonObject control;
    control = controls.add<JsonObject>(); control["name"] = "width"; control["type"] = "range"; control["default"] = 16; control["max"] = 32; control["value"] = width;
    control = controls.add<JsonObject>(); control["name"] = "height"; control["type"] = "range"; control["default"] = 16; control["max"] = 32; control["value"] = height;
    control = controls.add<JsonObject>(); control["name"] = "snake"; control["type"] = "checkbox"; control["default"] = true; control["value"] = snake;
  }
  
  void setControl(JsonObject control) override {
    ESP_LOGD(TAG, "%s = %s", control["name"].as<String>().c_str(), control["value"].as<String>().c_str());
    if (control["name"] == "width") width = control["value"];
    if (control["name"] == "height") height = control["value"];
    if (control["name"] == "snake") snake = control["value"];
    //if changed run setup
    setup();
  }

  void setup() override {

    //create panel 16
    layerV->layerP->addPixelsPre();
    for (int x = 0; x<width; x++) {
      for (int y = 0; y<height; y++) {
        layerV->layerP->addPixel({x, (x%2 || !snake)?y:height-1-y, 0});
      }
    }
    layerV->layerP->addPixelsPost();
  }
};
