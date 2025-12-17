/**
    @title     MoonLight
    @file      E_FastLED.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/moonlight/overview/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact us for more information.
**/

#if FT_MOONLIGHT

class RainbowEffect : public Node {
 public:
  static const char* name() { return "Rainbow"; }
  static uint8_t dim() { return _1D; }
  static const char* tags() { return "🔥⚡️"; }

  uint8_t deltaHue = 7;
  uint8_t speed = 8;  // default 8*32 = 256 / 256 = 1 = hue++

  void setup() {
    addControl(speed, "speed", "slider", 0, 32);
    addControl(deltaHue, "deltaHue", "slider", 0, 32);
  }

  uint16_t hue = 0;

  void loop() override {
    layer->fill_rainbow((hue += speed * 32) >> 8, deltaHue);  // hue back to uint8_t
  }
};

class RainbowWithGlitterEffect : public Node {
 public:
  static const char* name() { return "Rainbow with glitter"; }
  static uint8_t dim() { return _1D; }
  static const char* tags() { return "🔥⚡️"; }  // ⚡️ means FastLED origin

  bool glitter = false;
  uint8_t chance = 80;

  void setup() override {
    // no palette control is created
    addControl(glitter, "glitter", "checkbox");
    addControl(chance, "chance", "slider");
  }

  void loop() override {
    // built-in FastLED rainbow, plus some random sparkly glitter
    // FastLED's built-in rainbow generator
    layer->fill_rainbow(millis() / 50, 7);

    if (glitter) addGlitter(chance);
  }

  void addGlitter(fract8 chanceOfGlitter) {
    if (random8() < chanceOfGlitter) {
      layer->setRGB(random16(layer->size.x), CRGB::White);
    }
  }
};


#endif