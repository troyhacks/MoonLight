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

  static const char * name() {return "Solid🔥💡";}
  static uint8_t dim() {return _1D;}
  static const char * tags() {return "🔥💡";}

  void loop() override {
      layerV->fill_solid(CRGB::White);
  }
};

//alphabetically from here

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

  static const char * name() {return "BouncingBalls🔥💡";}
  static uint8_t dim() {return _1D;}
  static const char * tags() {return "🔥💡";}

  uint8_t grav = 128;
  uint8_t numBalls = 8;

  void addControls(JsonArray controls) override {
    addControl(controls, &grav, "grav", "range", 128);
    addControl(controls, &numBalls, "numBalls", "range", 8, 1, maxNumBalls);
    // ESP_LOGD(TAG, "");
    // serializeJson(controls, Serial); Serial.println();
  }

  //binding of loop persistent values (pointers)
  Ball balls[16][maxNumBalls];

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

    for (int y =0; y < MIN(layerV->size.y,16); y++) { //Min for the time being
      for (size_t i = 0; i < MIN(numBalls, maxNumBalls); i++) {
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
        // if (layerV->palette) {
        //   color = layerV->color_wheel(i*(256/MAX(numBalls, 8)));
        // } 
        // else if (hasCol2) {
        //   color = SEGCOLOR(i % NUM_COLORS);
        // }

        int pos = roundf(balls[y][i].height * (layerV->size.x - 1));

        CRGBPalette16 palette = PartyColors_p;

        CRGB color = ColorFromPalette(palette, i*(256/max(numBalls, (uint8_t)8))); //error: no matching function for call to 'max(uint8_t&, int)'

        layerV->setLight({pos, y, 0}, color);
        // if (layerV->size.x<32) layerV->setPixelColor(indexToVStrip(pos, stripNr), color); // encode virtual strip into index
        // else           layerV->setPixelColor(balls[i].height + (stripNr+1)*10.0f, color);
      } //balls      layerV->fill_solid(CRGB::White);
    }
  }
};

class GEQEffect: public Node {
public:

  static const char * name() {return "GEQ🔥 ♫💡";}
  static uint8_t dim() {return _1D;}
  static const char * tags() {return "";}

  uint8_t fadeOut = 255;
  uint8_t ripple = 128;
  bool colorBars = false;
  bool smoothBars = true;

  void addControls(JsonArray controls) override {
    addControl(controls, &fadeOut, "fadeOut", "range", 255, 0, 255);
    addControl(controls, &ripple, "ripple", "range", 128, 0, 255);
    addControl(controls, &colorBars, "colorBars", "checkbox", false);
    addControl(controls, &smoothBars, "smoothBars", "checkbox", true);
  }

  uint16_t *previousBarHeight; //array
  unsigned long step;

  void setup() override {
    Node::setup();
    previousBarHeight = (uint16_t*)malloc(layerV->size.x * sizeof(uint16_t));
    if (!previousBarHeight) {
      ESP_LOGE(TAG, "GEQEffect: malloc failed for previousBarHeight");
      return;
    }
    memset(previousBarHeight, 0, layerV->size.x * sizeof(uint16_t)); // initialize to 0
    step = millis();
  }
  void destructor() override {
    if (previousBarHeight) {
      free(previousBarHeight);
      previousBarHeight = nullptr;
    }
  }

  void loop() override {
    const int NUM_BANDS = 16 ; // map(layerV->custom1, 0, 255, 1, 16);

    #ifdef SR_DEBUG
    uint8_t samplePeak = *(uint8_t*)um_data->u_data[3];
    #endif

    bool rippleTime = false;
    if (millis() - step >= (256U - ripple)) {
      step = millis();
      rippleTime = true;
    }

    int fadeoutDelay = (256 - fadeOut) / 64; //256..1 -> 4..0
    size_t beat = map(beat16( fadeOut), 0, UINT16_MAX, 0, fadeoutDelay-1 ); // instead of call%fadeOutDelay

    if ((fadeoutDelay <= 1 ) || (beat == 0)) layerV->fadeToBlackBy(fadeOut);

    uint16_t lastBandHeight = 0;  // WLEDMM: for smoothing out bars

    //evenly distribute see also Funky Plank/By ewowi/From AXI
    float bandwidth = (float)layerV->size.x / NUM_BANDS;
    float remaining = bandwidth;
    uint8_t band = 0;
    Coord3D pos = {0,0,0};
    for (pos.x=0; pos.x < layerV->size.x; pos.x++) {
      //WLEDMM if not enough remaining
      if (remaining < 1) {band++; remaining+= bandwidth;} //increase remaining but keep the current remaining
      remaining--; //consume remaining

      // ppf("x %d b %d n %d w %f %f\n", x, band, NUM_BANDS, bandwidth, remaining);
      uint8_t frBand = ((NUM_BANDS < 16) && (NUM_BANDS > 1)) ? map(band, 0, NUM_BANDS - 1, 0, 15):band; // always use full range. comment out this line to get the previous behaviour.
      // frBand = constrain(frBand, 0, 15); //WLEDMM can never be out of bounds (I think...)
      uint16_t colorIndex = frBand * 17; //WLEDMM 0.255
      uint16_t bandHeight = geq[frBand];  // WLEDMM we use the original ffResult, to preserve accuracy

      // WLEDMM begin - smooth out bars
      if ((pos.x > 0) && (pos.x < (layerV->size.x-1)) && (smoothBars)) {
        // get height of next (right side) bar
        uint8_t nextband = (remaining < 1)? band +1: band;
        nextband = constrain(nextband, 0, 15);  // just to be sure
        frBand = ((NUM_BANDS < 16) && (NUM_BANDS > 1)) ? map(nextband, 0, NUM_BANDS - 1, 0, 15):nextband; // always use full range. comment out this line to get the previous behaviour.
        uint16_t nextBandHeight = geq[frBand];
        // smooth Band height
        bandHeight = (7*bandHeight + 3*lastBandHeight + 3*nextBandHeight) / 12;   // yeees, its 12 not 13 (10% amplification)
        bandHeight = constrain(bandHeight, 0, 255);   // remove potential over/underflows
        colorIndex = map(pos.x, 0, layerV->size.x-1, 0, 255); //WLEDMM
      }
      lastBandHeight = bandHeight; // remember BandHeight (left side) for next iteration
      uint16_t barHeight = map(bandHeight, 0, 255, 0, layerV->size.y); // Now we map bandHeight to barHeight. do not subtract -1 from layerV->size.y here
      // WLEDMM end

      if (barHeight > layerV->size.y) barHeight = layerV->size.y;                      // WLEDMM map() can "overshoot" due to rounding errors
      if (barHeight > previousBarHeight[pos.x]) previousBarHeight[pos.x] = barHeight; //drive the peak up

      CRGB ledColor = CRGB::Black;

      for (pos.y=0; pos.y < barHeight; pos.y++) {
        if (colorBars) //color_vertical / color bars toggle
          colorIndex = map(pos.y, 0, layerV->size.y-1, 0, 255);

        ledColor = ColorFromPalette(PartyColors_p, (uint8_t)colorIndex);

        layerV->setLight({pos.x, layerV->size.y - 1 - pos.y}, ledColor);
      }

      if ((ripple > 0) && (previousBarHeight[pos.x] > 0) && (previousBarHeight[pos.x] < layerV->size.y))  // WLEDMM avoid "overshooting" into other segments
        layerV->setLight({pos.x, layerV->size.y - previousBarHeight[pos.x]}, CHSV( millis()/50, 255, 255)); // take millis()/50 color for the time being

      if (rippleTime && previousBarHeight[pos.x]>0) previousBarHeight[pos.x]--;    //delay/ripple effect

    }
  }
};

class LinesEffect: public Node {
public:

  static const char * name() {return "Lines🔥💡";}
  static uint8_t dim() {return _1D;}
  static const char * tags() {return "🔥💡";}

  uint8_t bpm = 120;

  void addControls(JsonArray controls) override {
    addControl(controls, &bpm, "bpm", "range", 120);
  }

  void loop() override {
    int   frameNr;

    layerV->fadeToBlackBy(255);

    Coord3D pos = {0,0,0};
    pos.x = map(beat16( bpm), 0, UINT16_MAX, 0, layerV->size.x ); //instead of call%width

    for (pos.y = 0; pos.y < layerV->size.y; pos.y++) {
      int colorNr = (frameNr / layerV->size.y) % 3;
      layerV->setLight<CRGB>(pos, colorNr == 0?CRGB::Red:colorNr == 1?CRGB::Green:CRGB::Blue);
    }

    pos = {0,0,0};
    pos.y = map(beat16( bpm), 0, UINT16_MAX, 0, layerV->size.y ); //instead of call%height
    for (pos.x = 0; pos.x <  layerV->size.x; pos.x++) {
      int colorNr = (frameNr / layerV->size.x) % 3;
      layerV->setLight<CRGB>(pos, colorNr == 0?CRGB::Red:colorNr == 1?CRGB::Green:CRGB::Blue);
    }
    (frameNr)++;
  }
};

class LissajousEffect: public Node {
public:

  static const char * name() {return "Lissajous🔥";}
  static uint8_t dim() {return _1D;}
  static const char * tags() {return "🔥💡";}

  uint8_t xFrequency = 64;
  uint8_t fadeRate = 128;
  uint8_t speed = 128;

  void addControls(JsonArray controls) override {
    addControl(controls, &xFrequency, "xFrequency", "range", 64, 0, 255);
    addControl(controls, &fadeRate, "fadeRate", "range", 128, 0, 255);
    addControl(controls, &speed, "speed", "range", 128, 0, 255);
  }

  void loop() override {
    CRGBPalette16 palette = PartyColors_p;

    layerV->fadeToBlackBy(fadeRate);
    uint_fast16_t phase = millis() * speed / 256;  // allow user to control rotation speed, speed between 0 and 255!
    Coord3D locn = {0,0,0};
    for (int i=0; i < 256; i ++) {
        //WLEDMM: stick to the original calculations of xlocn and ylocn
        locn.x = sin8(phase/2 + (i*xFrequency)/64);
        locn.y = cos8(phase/2 + i*2);
        locn.x = (layerV->size.x < 2) ? 1 : (map(2*locn.x, 0,511, 0,2*(layerV->size.x-1)) +1) /2;    // softhack007: "*2 +1" for proper rounding
        locn.y = (layerV->size.y < 2) ? 1 : (map(2*locn.y, 0,511, 0,2*(layerV->size.y-1)) +1) /2;    // "layerV->size.y > 2" is needed to avoid div/0 in map()
        layerV->setLight(locn, ColorFromPalette(palette, millis()/100+i, 255));
    }
  }
};

class MovingHeadEffect: public Node {
  public:

  static const char * name() {return "MovingHead🔥";}

  uint8_t bpm;
  uint8_t pan;
  uint8_t tilt;
  uint8_t zoom;

  void addControls(JsonArray controls) override {
    addControl(controls, &bpm, "bpm", "range", 30);
    addControl(controls, &pan, "pan", "range", 0);
    addControl(controls, &tilt, "tilt", "range", 0);
    addControl(controls, &zoom, "zoom", "range", 0);
  }

  void loop() override {
    for (int i=0; i<layerV->size.x; i++) {

      int pos = millis()*bpm/6000 % layerV->size.x; //beatsin16( bpm, 0, layerV->size.x-1);
      CRGB color = CHSV( millis()/50, 255, 255);

      byte light[16]; //layerV->layerP->lights.header.channelsPerLight is not accepted by setLight
      memset(light, 0, sizeof(light)); //set light to 0
      
      if (i == pos) {
        layerV->layerP->lights.header.setRGB(light, color);
      }
      
      layerV->layerP->lights.header.setPan(light, pan);
      layerV->layerP->lights.header.setTilt(light, tilt);
      layerV->layerP->lights.header.setZoom(light, zoom);
      layerV->layerP->lights.header.setBrightness(light, layerV->layerP->lights.header.brightness);

      layerV->setLight({i,0,0}, light);
    }
  }
};

class RainbowEffect: public Node {
public:

  static const char * name() {return "Rainbow🔥";}
  static uint8_t dim() {return _1D;}
  static const char * tags() {return "🔥💡";}

  void loop() override {
    static uint8_t hue = 0;
    layerV->fill_rainbow(hue++, 7);
  }
};

// Author: @TroyHacks, from WLED MoonModules
// @license GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
class PaintBrushEffect: public Node {
public:

  static const char * name() {return "Paintbrush🔥 ♫💡";}
  static uint8_t dim() {return _1D;}
  static const char * tags() {return "";}

  uint8_t oscillatorOffset = 6 *  160/255;
  uint8_t numLines = 255;
  uint8_t fadeRate = 40;
  uint8_t minLength = 0;
  bool color_chaos = false;
  bool soft = true;
  bool phase_chaos = false;
  
  void addControls(JsonArray controls) override {
    addControl(controls, &oscillatorOffset, "oscillatorOffset", "range", 6 *  160/255, 0, 16);
    addControl(controls, &numLines, "numLines", "range", 255, 2, 255);
    addControl(controls, &fadeRate, "fadeRate", "range", 40, 0, 128);
    addControl(controls, &minLength, "minLength", "range", 0);
    addControl(controls, &color_chaos, "color_chaos", "checkbox", false);
    addControl(controls, &soft, "soft", "checkbox", true);
    addControl(controls, &phase_chaos, "phase_chaos", "checkbox", false);
  }

  uint16_t aux0Hue;
  uint16_t aux1Chaos;

  void loop() override {
    const uint16_t cols = layerV->size.x;
    const uint16_t rows = layerV->size.y;
    const uint16_t depth = layerV->size.z;

    CRGBPalette16 palette = PartyColors_p;

    // byte numLines = map8(nrOfLines,1,64);
    
    (aux0Hue)++;  // hue
    layerV->fadeToBlackBy(fadeRate);

    aux1Chaos = phase_chaos?random8():0;

    for (size_t i = 0; i < numLines; i++) {
      byte bin = map(i,0,numLines,0,15);
      
      byte x1 = beatsin8(oscillatorOffset*1 + geq[0]/16, 0, (cols-1), geq[bin], aux1Chaos);
      byte x2 = beatsin8(oscillatorOffset*2 + geq[0]/16, 0, (cols-1), geq[bin], aux1Chaos);
      byte y1 = beatsin8(oscillatorOffset*3 + geq[0]/16, 0, (rows-1), geq[bin], aux1Chaos);
      byte y2 = beatsin8(oscillatorOffset*4 + geq[0]/16, 0, (rows-1), geq[bin], aux1Chaos);
      byte z1;
      byte z2;
      int length;
      if (depth > 1) {
        z1 = beatsin8(oscillatorOffset*5 + geq[0]/16, 0, (depth-1), geq[bin], aux1Chaos);
        z2 = beatsin8(oscillatorOffset*6 + geq[0]/16, 0, (depth-1), geq[bin], aux1Chaos);

        length = sqrt((x2-x1) * (x2-x1) + (y2-y1) * (y2-y1) + (z2-z1) * (z2-z1));
      } else 
        length = sqrt((x2-x1) * (x2-x1) + (y2-y1) * (y2-y1));

      length = map8(geq[bin],0,length);

      if (length > max(1, (int)minLength)) {
        CRGB color;
        if (color_chaos)
          color = ColorFromPalette(palette, i * 255 / numLines + ((aux0Hue)&0xFF), 255);
        else
          color = ColorFromPalette(palette, map(i, 0, numLines, 0, 255), 255);
        if (depth > 1)
          layerV->drawLine3D(x1, y1, z1, x2, y2, z2, color, soft, length); // no soft implemented in 3D yet
        else
          layerV->drawLine(x1, y1, x2, y2, color, soft, length);
      }
    }
  }
};

class RandomEffect: public Node {
  public:

  static const char * name() {return "Random🔥";}
  static uint8_t dim() {return _1D;}
  static const char * tags() {return "";}

  void loop() override {
      layerV->fadeToBlackBy(70);
      layerV->setLight(random16(layerV->nrOfLights), CRGB(255, random8(), 0));
  }
};

class RipplesEffect: public Node {
  public:

  static const char * name() {return "Ripples🔥 🧊💫";}
  static uint8_t dim() {return _3D;}
  static const char * tags() {return "";}
  
  uint8_t speed;
  uint8_t interval;

  void addControls(JsonArray controls) override {
    addControl(controls, &speed, "speed", "range", 50);
    addControl(controls, &interval, "interval", "range", 128);
  }

  void loop() override {

    float ripple_interval = 1.3f * ((255.0f - interval)/128.0f) * sqrtf(layerV->size.y);
    float time_interval = millis()/(100.0 - speed)/((256.0f-128.0f)/20.0f);

    layerV->fadeToBlackBy(255);

    Coord3D pos = {0,0,0};
    for (pos.z=0; pos.z<layerV->size.z; pos.z++) {
      for (pos.x=0; pos.x<layerV->size.x; pos.x++) {

        float d = distance(layerV->size.x/2.0f, layerV->size.z/2.0f, 0.0f, (float)pos.x, (float)pos.z, 0.0f) / 9.899495f * layerV->size.y;
        pos.y = floor(layerV->size.y/2.0f * (1 + sinf(d/ripple_interval + time_interval))); //between 0 and layerV->size.y

        layerV->setLight(pos, CHSV( millis()/50 + random8(64), 200, 255));
      }
    }
  }
};

class RGBWParEffect: public Node {
  public:

  static const char * name() {return "RGBWPar🔥";}

  uint8_t bpm;
  uint8_t red;
  uint8_t green;
  uint8_t blue;
  uint8_t white;

  void addControls(JsonArray controls) override {
    addControl(controls, &bpm, "bpm", "range", 30);
    addControl(controls, &red, "red", "range", 30); //control["color"] = "Red"; ...
    addControl(controls, &green, "green", "range", 30);
    addControl(controls, &blue, "blue", "range", 30);
    addControl(controls, &white, "white", "range", 0);
  }

  void loop() override {
    layerV->fadeToBlackBy(255); //reset all channels
    for (int i=0; i<layerV->size.x * layerV->layerP->lights.header.channelsPerLight; i++) {
      layerV->layerP->lights.leds[i] = CRGB::Black;
    }

    int pos = millis()*bpm/6000 % layerV->size.x; //beatsin16( bpm, 0, layerV->size.x-1);

    byte light[4];
    memset(light, 0, sizeof(light)); //set light to 0
    
    layerV->layerP->lights.header.setRGB(light, CRGB(red, green, blue));
    layerV->layerP->lights.header.setWhite(light, white);

    layerV->setLight({pos,0,0}, light);
  }
};
  
class SinelonEffect: public Node {
public:

  static const char * name() {return "Sinelon🔥";}
  static uint8_t dim() {return _1D;}
  static const char * tags() {return "";}

  uint8_t bpm = 60;

  void addControls(JsonArray controls) override {
    addControl(controls, &bpm, "bpm", "range", 60);
  }

  void loop() override {
    layerV->fadeToBlackBy(20);
    for (int y =0; MIN(y<layerV->size.y,16); y++) { //Min for the time being    
      int pos = beatsin16( bpm, 0, layerV->size.x-1, y * 100 );
      layerV->setLight({pos,y,0}, CHSV( millis()/50, 255, 255)); //= CRGB(255, random8(), 0);
    }
  }
};

//AI generated
class SinusEffect: public Node {
  public:

  static const char * name() {return "Sinus🔥";}
  static uint8_t dim() {return _1D;}
  static const char * tags() {return "";}

  uint8_t speed = 5;

  void addControls(JsonArray controls) override {
    addControl(controls, &speed, "speed", "range", 5);
  }

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
        layerV->setLight(i, CHSV(hue, 255, brightness));
    }

    // Increment the phase to animate the wave
    phase += speed;
  }
};

class SphereMoveEffect: public Node {
  public:

  static const char * name() {return "SphereMove🔥 🧊💫";}
  static uint8_t dim() {return _3D;}
  static const char * tags() {return "";}

  uint8_t speed;
  
  void addControls(JsonArray controls) override {
    addControl(controls, &speed, "speed", "range", 50, 0, 99);
  }

  void loop() override {

    layerV->fadeToBlackBy(255);

    float time_interval = millis()/(100 - speed)/((256.0f-128.0f)/20.0f);

    Coord3D origin;
    origin.x = layerV->size.x / 2.0 * ( 1.0 + sinf(time_interval));
    origin.y = layerV->size.y / 2.0 * ( 1.0 + cosf(time_interval));
    origin.z = layerV->size.z / 2.0 * ( 1.0 + cosf(time_interval));

    float diameter = 2.0f+sinf(time_interval/3.0f);

    Coord3D pos;
    for (pos.x=0; pos.x<layerV->size.x; pos.x++) {
        for (pos.y=0; pos.y<layerV->size.y; pos.y++) {
            for (pos.z=0; pos.z<layerV->size.z; pos.z++) {
                float d = distance(pos.x, pos.y, pos.z, origin.x, origin.y, origin.z);

                if (d>diameter && d<diameter + 1.0) {
                  layerV->setLight(pos, CHSV( millis()/50 + random8(64), 200, 255));
                }
            }
        }
    }
  }
}; // SphereMove3DEffect

#endif