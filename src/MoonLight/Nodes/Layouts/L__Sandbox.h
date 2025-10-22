/**
    @title     MoonLight
    @file      L__Sandbox.h (Layout sandbox)
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/moonlight/overview/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact us for more information.
**/


#if FT_MOONLIGHT

// example template, do not remove
//a dd effects in ModuleDrivers::addNodes()
class NewLayout: public Node {
  public:

  static const char * name() {return "New Layout 🚥";} // 🚥 for layout
  static uint8_t dim() {return _3D;} // dimensions 🚧
  static const char * tags() {return "";} // tags for search, comma separated, no spaces 🚧

  uint8_t width = 12;
  uint8_t height = 12;
  uint8_t depth = 12;
  uint8_t pin = 16;
  
  void setup() override {
    // controls will show in the UI
    // for different type of controls see other Nodes
    addControl(width, "width", "range");
    addControl(height, "height", "range");
    addControl(depth, "depth", "range");
    addControl(pin, "pin", "pin", 1, SOC_GPIO_PIN_COUNT);
  }

  bool hasOnLayout() const override { return true; } // so the mapping system knows this node has onLayout, eg each time a modifier changes
  void onLayout() override {
    Coord3D pos;
    for (pos.z = 0; pos.z < layerV->size.z; pos.z++) {
        for (pos.y = 0; pos.y < layerV->size.y; pos.y++) {
            for (pos.x = 0; pos.x < layerV->size.x; pos.x++) {
                addLight(pos);
            }
        }
    }
    addPin(pin);
  }
};

#endif