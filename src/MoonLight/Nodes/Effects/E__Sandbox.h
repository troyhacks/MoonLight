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
    layer->fadeToBlackBy(70);
    sharedData; // read shared data if needed, add sharedData if needed
    Coord3D pos;
    for (pos.z = 0; pos.z < layer->size.z; pos.z++) {
        for (pos.y = 0; pos.y < layer->size.y; pos.y++) {
            for (pos.x = 0; pos.x < layer->size.x; pos.x++) {
                pos.x = beatsin8(bpm, 0, layer->size.x - 1);
                pos.y = beatsin8(intensity, 0, layer->size.y - 1);
                pos.z = beatsin8(intensity, 0, layer->size.z - 1);
                layer->setRGB(pos, ColorFromPalette(layer->layerP->palette, beatsin8(12, 0, 255)));
            }
        }
    }
  }

  ~ExampleEffect() override {}; //e,g, to free allocated memory
};

class BlackholeEffect: public Node {
  public:

  static const char * name() {return "Blackhole";}
  static uint8_t dim() {return _2D;}
  static const char * tags() {return "🔥🎨⏳🐙";}

  uint8_t fadeRate = 128; //speed
  uint8_t outerYfreq = 128; //intensity
  uint8_t outerXfreq = 128; //custom1
  uint8_t innerXfreq = 128;//custom2
  uint8_t innerYfreq = 128; //custom3
  uint8_t blur = 16; //check3

  void setup() override {
    addControl(fadeRate, "fadeRate", "range");
    addControl(outerYfreq, "outerYfreq", "range");
    addControl(outerXfreq, "outerXfreq", "range");
    addControl(innerXfreq, "innerXfreq", "range");
    addControl(innerYfreq, "innerYfreq", "range");
    addControl(blur, "blur", "range");
  }

  void loop() override {

    const int cols = layer->size.x;
    const int rows = layer->size.y;
    int x, y;

    layer->fadeToBlackBy(16 + (fadeRate>>3)); // create fading trails
    unsigned long t = millis()/128;                 // timebase
    // outer stars
    for (size_t i = 0; i < 8; i++) {
      x = beatsin8(outerXfreq>>3,   0, cols - 1, 0, ((i % 2) ? 128 : 0) + t * i);
      y = beatsin8(outerYfreq>>3, 0, rows - 1, 0, ((i % 2) ? 192 : 64) + t * i);
      layer->addRGB(Coord3D(x, y), ColorFromPalette(layer->layerP->palette, i*32));
    }
    // inner stars
    for (size_t i = 0; i < 4; i++) {
      x = beatsin8(innerXfreq>>3, cols/4, cols - 1 - cols/4, 0, ((i % 2) ? 128 : 0) + t * i);
      y = beatsin8(innerYfreq>>3   , rows/4, rows - 1 - rows/4, 0, ((i % 2) ? 192 : 64) + t * i);
      layer->addRGB(Coord3D(x, y), ColorFromPalette(layer->layerP->palette, 255-i*64));
    }
    // central white dot
    layer->setRGB(Coord3D(cols/2, rows/2), CRGB::White);
    // blur everything a bit
    if (blur) layer->blur2d(blur);
  }
};


#endif