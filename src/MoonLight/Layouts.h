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

  void setup() override {
    layerV->layerP->lights.header.channelsPerLight = sizeof(CRGB); //default

    //redundant?
    for (layerV->layerP->pass = 1; layerV->layerP->pass <= 2; layerV->layerP->pass++)
      map(); //calls also addLayout
  }

  void updateControl(JsonObject control) override {

    Node::updateControl(control);

    //if changed run map
    setup();
  }

  //calls addLayout functions, non virtual, only addLayout can be redefined in derived class
  void map() {

    if (on) {
      layerV->layerP->addLayoutPre();
      addLayout();
      layerV->layerP->addLayoutPost();
    } else {
      layerV->resetMapping();
    }
  }

  virtual void addLayout() {
  }

  virtual void destructor() {
    layerV->resetMapping();
  }

};

//alphabetically from here

class DMXLayout: public LayoutNode {
  public:

  static const char * name() {return "DMX🚥";}

  uint8_t width = 4; //default 4 moving heads
  char type[32] = "CRGBW";

  void addControls(JsonArray controls) override {
    hasLayout = true;
    addControl(controls, &width, "width", "range", 4, 1, 32);
    JsonObject control = addControl(controls, &type, "type", "select", "CRGBW", 1, 32);
    JsonArray values = control["values"].to<JsonArray>();
    values.add("CRGB");
    values.add("CRGBW");
    values.add("CrazyCurtain");
    values.add("Movinghead");
  }
  
  void setup() override {
    LayoutNode::setup();
    if (equal(type, "CRGBW")) layerV->layerP->lights.header.channelsPerLight = sizeof(CRGBW);
    else if (equal(type, "CrazyCurtain")) layerV->layerP->lights.header.channelsPerLight = sizeof(CrazyCurtain);
    else if (equal(type, "Movinghead")) layerV->layerP->lights.header.channelsPerLight = sizeof(MovingHead);
    else layerV->layerP->lights.header.channelsPerLight = sizeof(CRGB);
  }

  void addLayout() override {
    for (int x = 0; x<width; x++) {
      layerV->layerP->addLight({x, 0, 0});
    }
  }

};

class PanelLayout: public LayoutNode {
  public:

  static const char * name() {return "Panel🚥";}
  static uint8_t dim() {return _3D;}
  static const char * tags() {return "🚥💡";}

  uint8_t width = 16;
  uint8_t height = 16;
  uint8_t depth = 1;
  bool snake = true;

  void addControls(JsonArray controls) override {
    hasLayout = true;
    addControl(controls, &width, "width", "range", 16, 1, 32);
    addControl(controls, &height, "height", "range", 16, 1, 32);
    addControl(controls, &depth, "depth", "range", 1, 1, 32);
    addControl(controls, &snake, "snake", "checkbox", true);
  }
  
  void addLayout() override {
    layerV->layerP->addPin(2); //not working yet

    for (int x = 0; x<width; x++) {
      for (int y = 0; y<height; y++) {
        for (int z = 0; z<depth; z++) {
          layerV->layerP->addLight({x, (x%2 || !snake)?y:height-1-y, z});
        }
      }
    }
  }

};

class RingsLayout: public LayoutNode {
  public:

  static const char * name() {return "Rings🚥";}
  static uint8_t dim() {return _2D;}
  static const char * tags() {return "🚥💡";}

  uint8_t width = 16;
  uint8_t height = 16;

  void add(int leds, int radius) {
    for (int i = 0; i<leds; i++) {
      int x = width / 2.0 + ((sin8(255 * i / leds) - 127) / 127.0) * radius / 10.0;
      int y = height / 2.0 + ((cos8(255 * i / leds) - 127) / 127.0) * radius / 10.0;
      layerV->layerP->addLight({x, y, 0});
    }
  }

  void addLayout() override{
    layerV->layerP->addPin(2); //not working yet

    add(1, 0);
    add(8, 13);
    add(12, 23);
    add(16, 33);
    add(24, 43);
    add(32, 53);
    add(40, 63);
    add(48, 73);
    add(60, 83);
  }
};

#endif //FT_MOONLIGHT