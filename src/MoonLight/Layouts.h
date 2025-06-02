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

//alphabetically from here

class MovingHeadLayout: public Node {
  public:

  static const char * name() {return "MovingHead🚥";}

  uint8_t width = 4; //default 4 moving heads
  uint8_t pin = 2;

  void setup() override {
    hasLayout = true;
    Node::setup();
    
    layerV->layerP->lights.header.channelsPerLight = 16;
    layerV->layerP->lights.header.offsetPan = 0;
    layerV->layerP->lights.header.offsetTilt = 1;
    layerV->layerP->lights.header.offsetBrightness = 3;
    layerV->layerP->lights.header.offsetRGB = 4;
    layerV->layerP->lights.header.offsetZoom = 9;
  }

  void addControls(JsonArray controls) override {
    addControl(controls, &width, "width", "range", 4, 1, 32);
    addControl(controls, &pin, "pin", "number", 16, 1, 48);
  }
  
  void addLayout() override {
    for (int x = 0; x<width; x++) {
      addLight({x, 0, 0});
    }
    addPin(pin); //needed to slow down the dmx stream ... wip
  }

};

class PanelLayout: public Node {
  public:

  static const char * name() {return "Panel🚥";}
  static uint8_t dim() {return _3D;}
  static const char * tags() {return "🚥💡";}

  uint8_t width = 16;
  uint8_t height = 16;
  uint8_t depth = 1;
  bool snake = true;
  uint8_t pin = 2;

  void addControls(JsonArray controls) override {
    addControl(controls, &width, "width", "range", 16, 1, 32);
    addControl(controls, &height, "height", "range", 16, 1, 32);
    addControl(controls, &depth, "depth", "range", 1, 1, 32);
    addControl(controls, &snake, "snake", "checkbox", true);
    addControl(controls, &pin, "pin", "number", 16, 1, 48);
  }

  void setup() override {
    hasLayout = true;
    Node::setup();
  }

  void addLayout() override {
    
    for (int x = 0; x<width; x++) {
      for (int y = 0; y<height; y++) {
        for (int z = 0; z<depth; z++) {
          addLight({x, (x%2 || !snake)?y:height-1-y, z});
        }
      }
    }
    addPin(pin);
  }

};

class RingsLayout: public Node {
  public:

  static const char * name() {return "Rings🚥";}
  static uint8_t dim() {return _2D;}
  static const char * tags() {return "🚥💡";}

  uint8_t width = 16;
  uint8_t height = 16;
  uint8_t pin = 2;

  void addControls(JsonArray controls) override {
    addControl(controls, &pin, "pin", "number", 2, 1, 48);
  }
  void setup() override {
    hasLayout = true;
    Node::setup();
  }

  void add(int leds, int radius) {
    for (int i = 0; i<leds; i++) {
      int x = width / 2.0 + ((sin8(255 * i / leds) - 127) / 127.0) * radius / 10.0;
      int y = height / 2.0 + ((cos8(255 * i / leds) - 127) / 127.0) * radius / 10.0;
      addLight({x, y, 0});
    }
  }

  void addLayout() override{
    
    add(1, 0);
    add(8, 13);
    add(12, 23);
    add(16, 33);
    add(24, 43);
    add(32, 53);
    add(40, 63);
    add(48, 73);
    add(60, 83);

    addPin(pin);
  }
};

#endif //FT_MOONLIGHT