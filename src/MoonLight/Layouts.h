/**
    @title     MoonLight
    @file      Layouts.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/general/utilities/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
**/

#if FT_MOONLIGHT

class LayoutNode: public Node {
public:

  //calls addLayout functions, non virtual, only addLayout can be redefined in derived class
  void map() {
    layerV->layerP->addLayoutPre();
    addLayout();
    layerV->layerP->addLayoutPost();
  }

  virtual void addLayout() {
  }

};

class Panel16Layout: public LayoutNode {
  const char * name() override {return "Panel🚥";}

  uint8_t width = 16;
  uint8_t height = 16;
  bool snake = true;

  void getControls(JsonArray controls) override {
    hasLayout = true;
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
    //if changed run map
    for (layerV->layerP->pass = 1; layerV->layerP->pass <= 2; layerV->layerP->pass++)
      map(); //calls also addLayout
  }

  void setup() override {
    for (layerV->layerP->pass = 1; layerV->layerP->pass <= 2; layerV->layerP->pass++)
      map(); //calls also addLayout

  }

  void addLayout() override {
    layerV->layerP->addPin(2); //not working yet

    for (int x = 0; x<width; x++) {
      for (int y = 0; y<height; y++) {
        layerV->layerP->addLight({x, (x%2 || !snake)?y:height-1-y, 0});
      }
    }
  }

};

#endif //FT_MOONLIGHT