/**
    @title     MoonLight
    @file      Effects.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/general/utilities/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
**/

#if FT_MOONLIGHT

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

//BouncingBalls inspired by WLED
#define maxNumBalls 16
//each needs 12 bytes
struct Ball {
  unsigned long lastBounceTime;
  float impactVelocity;
  float height;
};

class BouncingBallsEffect: public Node {
  public:

  uint8_t grav = 128;
  uint8_t numBalls = 8;

  void getControls(JsonArray controls) override {
    JsonObject control;
    control = controls.add<JsonObject>(); control["name"] = "grav"; control["type"] = "range"; control["default"] = 128; control["value"] = grav;
    control = controls.add<JsonObject>(); control["name"] = "numBalls"; control["type"] = "range"; control["default"] = 8; control["min"] = 1; control["max"] = maxNumBalls; control["value"] = numBalls;
  }

  void setControl(JsonObject control) override {
    ESP_LOGD(TAG, "%s = %s", control["name"].as<String>().c_str(), control["value"].as<String>().c_str());
    if (control["name"] == "grav") grav = control["value"];
    if (control["name"] == "numBalls") numBalls = control["value"];
  }

  //binding of loop persistent values (pointers)
  Ball balls[16][maxNumBalls];

  const char * name() override {return "BouncingBalls";}
  uint8_t dim() override {return _1D;}
  const char * tags() override {return "💡";}

  void setup() override {}

  void loop() override {
    layerV->fadeToBlackBy(100);

    // non-chosen color is a random color
    const float gravity = -9.81f; // standard value of gravity
    // const bool hasCol2 = SEGCOLOR(2);
    const unsigned long time = millis();

    //not necessary as effectControls is cleared at setup(LedsLayer &leds)
    // if (call == 0) {
    //   for (size_t i = 0; i < maxNumBalls; i++) balls[i].lastBounceTime = time;
    // }

    for (int y =0; MIN(y<layerV->size.y,16); y++) { //Min for the time being
    for (size_t i = 0; i < numBalls; i++) {
      float timeSinceLastBounce = (time - balls[y][i].lastBounceTime)/((255-grav)/64 + 1);
      float timeSec = timeSinceLastBounce/1000.0f;
      balls[y][i].height = (0.5f * gravity * timeSec + balls[y][i].impactVelocity) * timeSec; // avoid use pow(x, 2) - its extremely slow !

      if (balls[y][i].height <= 0.0f) {
        balls[y][i].height = 0.0f;
        //damping for better effect using multiple balls
        float dampening = 0.9f - float(i)/float(numBalls * numBalls); // avoid use pow(x, 2) - its extremely slow !
        balls[y][i].impactVelocity = dampening * balls[y][i].impactVelocity;
        balls[y][i].lastBounceTime = time;

        if (balls[y][i].impactVelocity < 0.015f) {
          float impactVelocityStart = sqrtf(-2.0f * gravity) * random8(5,11)/10.0f; // randomize impact velocity
          balls[y][i].impactVelocity = impactVelocityStart;
        }
      } else if (balls[y][i].height > 1.0f) {
        continue; // do not draw OOB ball
      }

      // uint32_t color = SEGCOLOR(0);
      // if (leds.palette) {
      //   color = leds.color_wheel(i*(256/MAX(numBalls, 8)));
      // } 
      // else if (hasCol2) {
      //   color = SEGCOLOR(i % NUM_COLORS);
      // }

      int pos = roundf(balls[y][i].height * (layerV->size.x - 1));

      CRGBPalette16 palette = PartyColors_p;

      CRGB color = ColorFromPalette(palette, i*(256/max(numBalls, (uint8_t)8))); //error: no matching function for call to 'max(uint8_t&, int)'

      layerV->setLight({pos, y, 0}, color);
      // if (leds.size.x<32) leds.setPixelColor(indexToVStrip(pos, stripNr), color); // encode virtual strip into index
      // else           leds.setPixelColor(balls[i].height + (stripNr+1)*10.0f, color);
    } //balls      layerV->fill_solid(CRGB::White);
    }
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
      layerV->setLightColor(random16(layerV->nrOfLights), CRGB(255, random8(), 0));
  }
};

class SinelonEffect: public Node {
public:

  uint8_t bpm = 60;

  void getControls(JsonArray controls) override {
    JsonObject control;
    control = controls.add<JsonObject>(); control["name"] = "bpm"; control["type"] = "range"; control["default"] = 60; control["value"] = bpm;
  }

  void setControl(JsonObject control) override {
    ESP_LOGD(TAG, "%s = %s", control["name"].as<String>().c_str(), control["value"].as<String>().c_str());
    if (control["name"] == "bpm") bpm = control["value"];
  }

  const char * name() override {return "Sinelon";}
  uint8_t dim() override {return _1D;}
  const char * tags() override {return "💡";}

  void setup() override {}

  void loop() override {
    layerV->fadeToBlackBy(20);
    for (int y =0; MIN(y<layerV->size.y,16); y++) { //Min for the time being    
      int pos = beatsin16( bpm, 0, layerV->size.x-1, y * 100 );
      layerV->setLightColor({pos,y,0}, CHSV( millis()/50, 255, 255)); //= CRGB(255, random8(), 0);
    }
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
    control = controls.add<JsonObject>(); control["name"] = "speed"; control["type"] = "range"; control["default"] = 5; control["value"] = speed;
  }

  void setControl(JsonObject control) override {
    ESP_LOGD(TAG, "%s = %s", control["name"].as<String>().c_str(), control["value"].as<String>().c_str());
    if (control["name"] == "speed") speed = control["value"];
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
    
    for (uint16_t i = 0; i < layerV->nrOfLights; i++) {
        // Calculate the sine wave value for the current LED
        uint8_t wave = sin8((i * 255 / layerV->nrOfLights) + phase);
        // Map the sine wave value to a color hue
        uint8_t hue = wave + hueOffset;
        // Set the LED color using the calculated hue
        layerV->setLightColor(i, CHSV(hue, 255, brightness));
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
    control = controls.add<JsonObject>(); control["name"] = "BPM"; control["type"] = "range"; control["default"] = 120; control["value"] = bpm;
  }

  void setControl(JsonObject control) override {
    ESP_LOGD(TAG, "%s = %s", control["name"].as<String>().c_str(), control["value"].as<String>().c_str());
    if (control["name"] == "BPM") bpm = control["value"];
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
      layerV->setLightColor(pos, colorNr == 0?CRGB::Red:colorNr == 1?CRGB::Green:CRGB::Blue);
    }

    pos = {0,0,0};
    pos.y = ::map(beat16( bpm), 0, UINT16_MAX, 0, layerV->size.y ); //instead of call%height
    for (pos.x = 0; pos.x <  layerV->size.x; pos.x++) {
      int colorNr = (frameNr / layerV->size.x) % 3;
      layerV->setLightColor(pos, colorNr == 0?CRGB::Red:colorNr == 1?CRGB::Green:CRGB::Blue);
    }
    (frameNr)++;
  }
};

class LissajousEffect: public Node {
public:

  uint8_t xFrequency = 64;
  uint8_t fadeRate = 128;
  uint8_t speed = 128;

  void getControls(JsonArray controls) override {
    JsonObject control;
    control = controls.add<JsonObject>(); control["name"] = "xFrequency"; control["type"] = "range"; control["default"] = 64; control["value"] = xFrequency;
    control = controls.add<JsonObject>(); control["name"] = "fadeRate"; control["type"] = "range"; control["default"] = 128; control["value"] = fadeRate;
    control = controls.add<JsonObject>(); control["name"] = "speed"; control["type"] = "range"; control["default"] = 128; control["value"] = speed;
  }

  void setControl(JsonObject control) override {
    ESP_LOGD(TAG, "%s = %s", control["name"].as<String>().c_str(), control["value"].as<String>().c_str());
    if (control["name"] == "xFrequency") xFrequency = control["value"];
    if (control["name"] == "fadeRate") fadeRate = control["value"];
    if (control["name"] == "speed") speed = control["value"];
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
        // layerV->setLightColor(locn, ColorFromPalette(palette, millis()/100+i, 255));
        layerV->setLight(locn, ColorFromPalette(palette, millis()/100+i, 255));
    }
  }
};

class MovingHeadEffect: public Node {
  public:
  
    uint8_t bpm;
    uint8_t pan;
    uint8_t tilt;
  
    void getControls(JsonArray controls) override {
      JsonObject control;
      control = controls.add<JsonObject>(); control["name"] = "bpm"; control["type"] = "range"; control["default"] = 30; control["value"] = bpm;
      control = controls.add<JsonObject>(); control["name"] = "pan"; control["type"] = "range"; control["default"] = 0; control["value"] = pan;
      control = controls.add<JsonObject>(); control["name"] = "tilt"; control["type"] = "range"; control["default"] = 0; control["value"] = tilt; 
    }
  
    void setControl(JsonObject control) override {
      if (control["name"] == "bpm") bpm = control["value"];
      if (control["name"] == "pan") pan = control["value"];
      if (control["name"] == "tilt") tilt = control["value"];
    }
  
    void loop() override {
      for (int i=0; i<layerV->size.x; i++) {

        MovingHead mh;

        int pos = millis()*bpm/6000 % layerV->size.x; //beatsin16( bpm, 0, layerV->size.x-1);
        CRGB color = CHSV( millis()/50, 255, 255);

        if (i == pos) {
          mh.red = color.red;
          mh.green = color.green;
          mh.blue = color.blue;
          mh.white = 0;
        } else  {
          mh.red = 0;
          mh.green = 0;
          mh.blue = 0;
          mh.white = 0;
        }
  
        mh.x_move = pan;
        mh.x_move_fine = 255;
        mh.y_move = tilt;
        mh.y_move_fine = 255;
        mh.axis_slow_to_fast = 0;
        mh.dimmer = layerV->layerP->lights.header.brightness;
        mh.strobe = 0;

        layerV->setLight({i,0,0}, mh);
      }
    }
  };

class RGBWParEffect: public Node {
  public:
  
    uint8_t bpm;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t white;
  
    void getControls(JsonArray controls) override {
      JsonObject control;
      control = controls.add<JsonObject>(); control["name"] = "bpm"; control["type"] = "range"; control["default"] = 30; control["value"] = bpm;
      control = controls.add<JsonObject>(); control["name"] = "red"; control["type"] = "range"; control["default"] = 30; control["value"] = red; control["color"] = "Red";
      control = controls.add<JsonObject>(); control["name"] = "green"; control["type"] = "range"; control["default"] = 30; control["value"] = green; control["color"] = "Green";
      control = controls.add<JsonObject>(); control["name"] = "blue"; control["type"] = "range"; control["default"] = 30; control["value"] = blue; control["color"] = "Blue";
      control = controls.add<JsonObject>(); control["name"] = "white"; control["type"] = "range"; control["default"] = 0; control["value"] = white;
    }
  
    void setControl(JsonObject control) override {
      if (control["name"] == "bpm") bpm = control["value"];
      if (control["name"] == "red") red = control["value"];
      if (control["name"] == "green") green = control["value"];
      if (control["name"] == "blue") blue = control["value"];
      if (control["name"] == "white") white = control["value"];
    }
  
    void loop() override {
      layerV->fadeToBlackBy(255); //reset all channels
      for (int i=0; i<layerV->size.x * layerV->layerP->lights.header.channelsPerLight; i++) {
        layerV->layerP->lights.leds[i] = CRGB::Black;
      }

      int pos = millis()*bpm/6000 % layerV->size.x; //beatsin16( bpm, 0, layerV->size.x-1);

      CRGBW rgbw;
      rgbw.red = red;
      rgbw.green = green;
      rgbw.blue = blue;
      rgbw.white = white;
      layerV->setLight({pos,0,0}, rgbw);
    }
  };
  
#endif