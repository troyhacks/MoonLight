/**
    @title     MoonLight
    @file      Effects.h
    @repo      https://github.com/ewowi/MoonBase, submit changes to this file as PRs
    @Authors   https://github.com/ewowi/MoonBase/commits/main
    @Doc       https://ewowi.github.io/MoonBase/general/utilities/
    @Copyright © 2025 Github MoonBase Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
**/

class SolidEffect: public Node {
  public:

  const char * name() override {return "Solid";}
  uint8_t dim() override {return _1D;}
  const char * tags() override {return "💡";}

  void setup() override {}

  void loop() override {
      layerV->fill_solid(CRGB::White);
  }
};

class RandomEffect: public Node {
  public:

  const char * name() override {return "Random";}
  uint8_t dim() override {return _1D;}
  const char * tags() override {return "💡";}

  void setup() override {}

  void loop() override {
      layerV->fadeToBlackBy(70);
      layerV->setPixelColor(random16(layerV->nrOfLeds), CRGB(255, random8(), 0));
  }
};

class SinelonEffect: public Node {
public:

  uint8_t bpm = 60;

  void getControls(JsonArray controls) override {
    JsonObject control;
    control = controls.add<JsonObject>();     control["name"] = "BPM"; control["type"] = "Range"; control["value"] = bpm;
  }

  void setControls(JsonArray controls) override {
    ESP_LOGD(TAG, "%s", controls[0].as<String>().c_str());
    bpm = controls[0]["value"];
  }

  const char * name() override {return "Sinelon";}
  uint8_t dim() override {return _1D;}
  const char * tags() override {return "💡";}

  void setup() override {}

  void loop() override {
    layerV->fadeToBlackBy(20);
    int pos = beatsin16( bpm, 0, layerV->nrOfLeds );
    layerV->setPixelColor(pos, CHSV( millis()/50, 255, 255)); //= CRGB(255, random8(), 0);
  }
};

class RainbowEffect: public Node {
public:

  const char * name() override {return "Rainbow";}
  uint8_t dim() override {return _1D;}
  const char * tags() override {return "💡";}

  void setup() override {}

  void loop() override {
    static uint8_t hue = 0;
    layerV->fill_rainbow(hue++, 7);
  }
};

//AI generated
class SinusEffect: public Node {
public:

  uint8_t speed = 5;

  void getControls(JsonArray controls) override {
    JsonObject control;
    control = controls.add<JsonObject>();     control["name"] = "speed"; control["type"] = "Range"; control["value"] = speed;
  }

  void setControls(JsonArray controls) override {
    ESP_LOGD(TAG, "%s", controls[0].as<String>().c_str());
    speed = controls[0]["value"];
  }

  const char * name() override {return "Sinus";}
  uint8_t dim() override {return _1D;}
  const char * tags() override {return "💡";}

  void setup() override {}

  void loop() override {
    layerV->fadeToBlackBy(70);

    uint8_t hueOffset =  millis() / 10;
    static uint16_t phase = 0; // Tracks the phase of the sine wave
    uint8_t brightness = 255;
    
    for (uint16_t i = 0; i < layerV->nrOfLeds; i++) {
        // Calculate the sine wave value for the current LED
        uint8_t wave = sin8((i * 255 / layerV->nrOfLeds) + phase);
        // Map the sine wave value to a color hue
        uint8_t hue = wave + hueOffset;
        // Set the LED color using the calculated hue
        layerV->setPixelColor(i, CHSV(hue, 255, brightness));
    }

    // Increment the phase to animate the wave
    phase += speed;
  }
};

class LinesEffect: public Node {
public:

  const char * name() override {return "Lines";}
  uint8_t dim() override {return _1D;}
  const char * tags() override {return "💡";}

  uint8_t bpm = 120;

  void getControls(JsonArray controls) override {
    JsonObject control;
    control = controls.add<JsonObject>();     control["name"] = "BPM"; control["type"] = "Range"; control["value"] = bpm;
  }

  void setControls(JsonArray controls) override {
    ESP_LOGD(TAG, "%s", controls[0].as<String>().c_str());
    bpm = controls[0]["value"];
  }

  void setup() override {
  }

  void loop() override {
    int   frameNr;

    layerV->fadeToBlackBy(255);

    Coord3D pos = {0,0,0};
    pos.x = ::map(beat16( bpm), 0, UINT16_MAX, 0, layerV->size.x ); //instead of call%width

    for (pos.y = 0; pos.y < layerV->size.y; pos.y++) {
      int colorNr = (frameNr / layerV->size.y) % 3;
      layerV->setPixelColor(pos, colorNr == 0?CRGB::Red:colorNr == 1?CRGB::Green:CRGB::Blue);
    }

    pos = {0,0,0};
    pos.y = ::map(beat16( bpm), 0, UINT16_MAX, 0, layerV->size.y ); //instead of call%height
    for (pos.x = 0; pos.x <  layerV->size.x; pos.x++) {
      int colorNr = (frameNr / layerV->size.x) % 3;
      layerV->setPixelColor(pos, colorNr == 0?CRGB::Red:colorNr == 1?CRGB::Green:CRGB::Blue);
    }
    (frameNr)++;
  }
};

class LissajousEffect: public Node {
public:

  uint8_t xFrequency = 64;// = leds.effectControls.read<uint8_t>();
  uint8_t fadeRate = 128;// = leds.effectControls.read<uint8_t>();
  uint8_t speed = 128;// = leds.effectControls.read<uint8_t>();

  void getControls(JsonArray controls) override {
    JsonObject control;
    control = controls.add<JsonObject>(); control["name"] = "xFrequency"; control["type"] = "Range"; control["value"] = xFrequency;
    control = controls.add<JsonObject>(); control["name"] = "fadeRate"; control["type"] = "Range"; control["value"] = fadeRate;
    control = controls.add<JsonObject>(); control["name"] = "speed"; control["type"] = "Range"; control["value"] = speed;
  }

  void setControls(JsonArray controls) override {
    ESP_LOGD(TAG, "%s", controls[0].as<String>().c_str());
    ESP_LOGD(TAG, "%s", controls[1].as<String>().c_str());
    ESP_LOGD(TAG, "%s", controls[2].as<String>().c_str());
    xFrequency = controls[0]["value"];
    fadeRate = controls[1]["value"];
    speed = controls[2]["value"];
  }

  const char * name() override {return "Lissajous";}
  uint8_t dim() override {return _1D;}
  const char * tags() override {return "💡";}

  void setup() override {}

  void loop() override {
    CRGBPalette16 palette = PartyColors_p;

    layerV->fadeToBlackBy(fadeRate);
    uint_fast16_t phase = millis() * speed / 256;  // allow user to control rotation speed, speed between 0 and 255!
    Coord3D locn = {0,0,0};
    for (int i=0; i < 256; i ++) {
        //WLEDMM: stick to the original calculations of xlocn and ylocn
        locn.x = sin8(phase/2 + (i*xFrequency)/64);
        locn.y = cos8(phase/2 + i*2);
        locn.x = (layerV->size.x < 2) ? 1 : (::map(2*locn.x, 0,511, 0,2*(layerV->size.x-1)) +1) /2;    // softhack007: "*2 +1" for proper rounding
        locn.y = (layerV->size.y < 2) ? 1 : (::map(2*locn.y, 0,511, 0,2*(layerV->size.y-1)) +1) /2;    // "leds.size.y > 2" is needed to avoid div/0 in map()
        layerV->setPixelColor(locn, ColorFromPalette(palette, millis()/100+i, 255));
    }
  }
};
