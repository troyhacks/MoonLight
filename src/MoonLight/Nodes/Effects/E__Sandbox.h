/**
    @title     MoonLight
    @file      E__Sandbox.h (Effect sandbox)
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/moonlight/overview/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact us for more information.
**/


#if FT_MOONLIGHT

// #include "../Nodes.h"

// example template, do not remove!
// add this class in /src/MoonLight/ModuleEffects::addNodes()
// add documentation in /docs/moonlight/effects.md
class ExampleEffect: public Node {
  public:

  static const char * name() {return "Example Effect";}
  static uint8_t dim() {return _3D;} // Dimensions supported _3D prefered, _2D or _1D can be used for first phase
  static const char * tags() {return "🔥🎨⏳";} // use emojis see https://moonmodules.org/MoonLight/moonlight/overview/#emoji-coding, 🔥 for effect, 🎨 if palette used (recommended)

  uint8_t bpm = 60; // 1 beat per second
  uint8_t intensity = 128;

  void setup() override {
    // controls will show in the UI
    // for different type of controls see other Nodes
    addControl(bpm, "bpm", "range");
    addControl(intensity, "intensity", "range");
  }

  void onSizeChanged(Coord3D prevSize) override {} // e.g. realloc variables

  void onUpdate(String &oldValue, JsonObject control) {
    // add your custom onUpdate code here
    if (control["name"] == "bpm") {
      //...
    }
  }

  // each frame of effect
  // use of bpm functions like beatsin8 is recommended so independent from LEDs framerate (33000/nrOfLights)
  void loop() override {
    layerV->fadeToBlackBy(70);
    sharedData; // read shared data if needed, add sharedData if needed
    Coord3D pos;
    for (pos.z = 0; pos.z < layerV->size.z; pos.z++) {
        for (pos.y = 0; pos.y < layerV->size.y; pos.y++) {
            for (pos.x = 0; pos.x < layerV->size.x; pos.x++) {
                pos.x = beatsin8(bpm, 0, layerV->size.x - 1);
                pos.y = beatsin8(intensity, 0, layerV->size.y - 1);
                pos.z = beatsin8(intensity, 0, layerV->size.z - 1);
                layerV->setRGB(pos, ColorFromPalette(layerV->layerP->palette, beatsin8(12, 0, 255)));
            }
        }
    }
  }

  ~ExampleEffect() override {}; //e,g, to free allocated memory
};

#endif