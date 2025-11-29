/**
    @title     MoonLight
    @file      E_WLED.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/moonlight/overview/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact us for more information.
**/

#if FT_MOONLIGHT

class BouncingBallsEffect : public Node {
 public:
  static const char* name() { return "Bouncing Balls"; }
  static uint8_t dim() { return _2D; }
  static const char* tags() { return "🔥🎨🐙"; }

  uint8_t grav = 128;
  uint8_t numBalls = 8;

  void setup() override {
    addControl(grav, "grav", "slider");
    addControl(numBalls, "numBalls", "slider", 1, maxNumBalls);
  }

  // binding of loop persistent values (pointers)
  Ball (*balls)[maxNumBalls] = nullptr;  //[maxColumns][maxNumBalls];

  ~BouncingBallsEffect() override { freeMB(balls); }

  void onSizeChanged(const Coord3D& prevSize) override {
    freeMB(balls);
    balls = allocMB<Ball[maxNumBalls]>(layer->size.y);

    if (!balls) {
      EXT_LOGE(ML_TAG, "allocate balls failed");
    }
  }

  void loop() override {
    if (!balls) return;

    layer->fadeToBlackBy(100);

    // non-chosen color is a random color
    const float gravity = -9.81f;  // standard value of gravity
    // const bool hasCol2 = SEGCOLOR(2);
    const unsigned long time = millis();

    // not necessary as effectControls is cleared at setup()
    //  if (call == 0) {
    //    for (size_t i = 0; i < maxNumBalls; i++) balls[i].lastBounceTime = time;
    //  }

    for (int y = 0; y < layer->size.y; y++) {
      for (size_t i = 0; i < MIN(numBalls, maxNumBalls); i++) {
        float timeSinceLastBounce = (time - balls[y][i].lastBounceTime) / ((255 - grav) / 64 + 1);
        float timeSec = timeSinceLastBounce / 1000.0f;
        balls[y][i].height = (0.5f * gravity * timeSec + balls[y][i].impactVelocity) * timeSec;  // avoid use pow(x, 2) - its extremely slow !

        if (balls[y][i].height <= 0.0f) {
          balls[y][i].height = 0.0f;
          // damping for better effect using multiple balls
          float dampening = 0.9f - float(i) / float(numBalls * numBalls);  // avoid use pow(x, 2) - its extremely slow !
          balls[y][i].impactVelocity = dampening * balls[y][i].impactVelocity;
          balls[y][i].lastBounceTime = time;

          if (balls[y][i].impactVelocity < 0.015f) {
            float impactVelocityStart = sqrtf(-2.0f * gravity) * random8(5, 11) / 10.0f;  // randomize impact velocity
            balls[y][i].impactVelocity = impactVelocityStart;
          }
        } else if (balls[y][i].height > 1.0f) {
          continue;  // do not draw OOB ball
        }

        // uint32_t color = SEGCOLOR(0);
        // if (layer->layerP->palette) {
        //   color = layer->color_wheel(i*(256/MAX(numBalls, 8)));
        // }
        // else if (hasCol2) {
        //   color = SEGCOLOR(i % NUM_COLORS);
        // }

        uint8_t pos = roundf(balls[y][i].height * (layer->size.x - 1));

        CRGB color = ColorFromPalette(layer->layerP->palette, i * (256 / max(numBalls, (uint8_t)8)));  // error: no matching function for call to 'max(uint8_t&, int)'

        layer->setRGB(Coord3D(pos, y), color);
        // if (layer->size.x<32) layer->setRGB(indexToVStrip(pos, stripNr), color); // encode virtual strip into index
        // else           layer->setRGB(balls[i].height + (stripNr+1)*10.0f, color);
      }  // balls      layer->fill_solid(CRGB::White);
    }
  }
};

  // original version from SR 0.13, with some enhancements by @softhack007
  // Blurz. By Andrew Tuline.
  // Hint: Looks best with segment brightness set to max (use global brightness to reduce brightness)
  // even with 1D effect we have to take logic for 2D segments for allocation as fill_solid() fills whole segment
  #define MAX_FREQ_LOG10 4.04238f  // log10(MAX_FREQUENCY)
class BlurzEffect : public Node {
 public:
  static const char* name() { return "Blurz"; }
  static uint8_t dim() { return _3D; }  // test...
  static const char* tags() { return "🔥🎵🎨☾"; }

  // static const char _data_FX_MODE_BLURZ[] PROGMEM = "Blurz Plus ☾@Fade rate,Blur,,,,FreqMap ☾,GEQ Scanner ☾,;!,Color mix;!;01f;sx=48,ix=127,m12=7,si=0"; // Pinwheel, Beatsin

  uint16_t aux0 = 0;
  uint16_t aux1 = 65535;  // last pixel postion. 65535 = none
  CRGB step = {0, 0, 0};  // last pixel color
  uint32_t call = 0;      // last pixel color

  uint8_t fadeRate = 48;
  uint8_t blur = 127;
  bool freqMap;
  bool geqScanner;

  void setup() override {
    addControl(fadeRate, "fadeRate", "slider", 1, 255);
    addControl(blur, "blur", "slider", 1, 255);
    addControl(freqMap, "freqMap", "checkbox");
    addControl(geqScanner, "geqScanner", "checkbox");

    layer->fadeToBlackBy(255);
  }

  void loop() override {
    random16_add_entropy(esp_random() & 0xFFFF);  // improves randonmess

    int fadeoutDelay = (256 - fadeRate) / 24;
    if ((fadeoutDelay <= 1) || ((call % fadeoutDelay) == 0))
      layer->fadeToBlackBy(fadeRate);
    else {
      layer->blur2d(8 + blur / 8 + fadeoutDelay * 4);
    }
    if ((aux1 < layer->size.x * layer->size.y * layer->size.z) && (sharedData.volume > 1.0f)) layer->setRGB(aux1, step);  // "repaint" last pixel after blur

    unsigned freqBand = aux0 % 16;
    uint16_t segLoc = random16(layer->size.x * layer->size.y * layer->size.z);

    if (freqMap) {  // FreqMap mode : blob location by major frequency
      int freqLocn;
      unsigned maxLen = (geqScanner) ? MAX(1, layer->size.x * layer->size.y * layer->size.z - 16) : layer->size.x * layer->size.y * layer->size.z;  // usable segment length - leave 16 pixels when embedding "GEQ scan"
      freqLocn = roundf((log10f((float)sharedData.majorPeak) - 1.78f) * float(maxLen) / (MAX_FREQ_LOG10 - 1.78f));                                  // log10 frequency range is from 1.78 to 3.71. Let's scale to layer->size.x * layer->size.y * layer->size.z. // WLEDMM proper rounding
      if (freqLocn < 1) freqLocn = 0;                                                                                                               // avoid underflow
      segLoc = (geqScanner) ? freqLocn + freqBand : freqLocn;
    } else if (geqScanner) {  // GEQ Scanner mode: blob location is defined by frequency band + random offset
      float bandWidth = float(layer->size.x * layer->size.y * layer->size.z) / 16.0f;
      int bandStart = roundf(bandWidth * freqBand);
      segLoc = bandStart + random16(max(1, int(bandWidth)));
    }
    segLoc = MAX(uint16_t(0), MIN(uint16_t(layer->size.x * layer->size.y * layer->size.z - 1), segLoc));  // fix overflows

    if (layer->size.x * layer->size.y * layer->size.z < 2) segLoc = 0;                                                       // WLEDMM just to be sure
    unsigned pixColor = (2 * sharedData.bands[freqBand] * 240) / max(1, layer->size.x * layer->size.y * layer->size.z - 1);  // WLEDMM avoid uint8 overflow, and preserve pixel parameters for redraw
    unsigned pixIntensity = min((unsigned)(2.0f * sharedData.bands[freqBand]), 255U);

    if (sharedData.volume > 1.0f) {
      layer->setRGB(segLoc, ColorFromPalette(layer->layerP->palette, pixColor));
      step = layer->getRGB(segLoc);  // remember last color
      aux1 = segLoc;                 // remember last position

      layer->blur2d(blur);
      aux0++;
      aux0 %= 16;                                                                 // make sure it doesn't cross 16
      layer->addRGB(segLoc, ColorFromPalette(layer->layerP->palette, pixColor));  // repaint center pixel after blur
    } else
      layer->blur2d(blur);  // silence - just blur it again
    call++;
  }  // loop
};  // BlurzEffect

// DistortionWaves inspired by WLED, ldirko and blazoncek, https://editor.soulmatelights.com/gallery/1089-distorsion-waves
class DistortionWavesEffect : public Node {
 public:
  static const char* name() { return "Distortion Waves"; }
  static uint8_t dim() { return _2D; }
  static const char* tags() { return "🔥🐙"; }

  uint8_t speed = 4;
  uint8_t scale = 4;

  void setup() override {
    addControl(speed, "speed", "slider", 0, 8);
    addControl(scale, "scale", "slider", 0, 8);
  }

  void loop() override {
    uint8_t w = 2;

    uint16_t a = millis() / 32;
    uint16_t a2 = a / 2;
    uint16_t a3 = a / 3;

    uint16_t cx = beatsin8(10 - speed, 0, layer->size.x - 1) * scale;
    uint16_t cy = beatsin8(12 - speed, 0, layer->size.y - 1) * scale;
    uint16_t cx1 = beatsin8(13 - speed, 0, layer->size.x - 1) * scale;
    uint16_t cy1 = beatsin8(15 - speed, 0, layer->size.y - 1) * scale;
    uint16_t cx2 = beatsin8(17 - speed, 0, layer->size.x - 1) * scale;
    uint16_t cy2 = beatsin8(14 - speed, 0, layer->size.y - 1) * scale;

    uint16_t xoffs = 0;
    Coord3D pos = {0, 0, 0};
    for (pos.x = 0; pos.x < layer->size.x; pos.x++) {
      xoffs += scale;
      uint16_t yoffs = 0;

      for (pos.y = 0; pos.y < layer->size.y; pos.y++) {
        yoffs += scale;

        uint8_t rdistort = cos8((cos8(((pos.x << 3) + a) & 255) + cos8(((pos.y << 3) - a2) & 255) + a3) & 255) >> 1;
        uint8_t gdistort = cos8((cos8(((pos.x << 3) - a2) & 255) + cos8(((pos.y << 3) + a3) & 255) + a + 32) & 255) >> 1;
        uint8_t bdistort = cos8((cos8(((pos.x << 3) + a3) & 255) + cos8(((pos.y << 3) - a) & 255) + a2 + 64) & 255) >> 1;

        uint8_t valueR = rdistort + w * (a - (((xoffs - cx) * (xoffs - cx) + (yoffs - cy) * (yoffs - cy)) >> 7));
        uint8_t valueG = gdistort + w * (a2 - (((xoffs - cx1) * (xoffs - cx1) + (yoffs - cy1) * (yoffs - cy1)) >> 7));
        uint8_t valueB = bdistort + w * (a3 - (((xoffs - cx2) * (xoffs - cx2) + (yoffs - cy2) * (yoffs - cy2)) >> 7));

        valueR = layer->layerP->gamma8(cos8(valueR));
        valueG = layer->layerP->gamma8(cos8(valueG));
        valueB = layer->layerP->gamma8(cos8(valueB));

        layer->setRGB(pos, CRGB(valueR, valueG, valueB));
      }
    }
  }
};  // DistortionWaves

  #define MAX_FREQUENCY 11025  // sample frequency / 2 (as per Nyquist criterion)

class FreqMatrixEffect : public Node {
 public:
  static const char* name() { return "Freq Matrix"; }
  static uint8_t dim() { return _1D; }  // 2D/3D-ish?
  static const char* tags() { return "🔥♪🐙"; }

  uint8_t speed = 255;
  uint8_t fx = 128;
  uint8_t lowBin = 0;   // 18;
  uint8_t highBin = 0;  // 48;
  uint8_t sensitivity10 = 30;
  bool audioSpeed = false;  // 💫

  void setup() override {
    addControl(speed, "speed", "slider");
    addControl(fx, "soundFX", "slider");
    addControl(lowBin, "lowBin", "slider");
    addControl(highBin, "highBin", "slider");
    addControl(sensitivity10, "sensitivity", "slider", 10, 100);
    addControl(audioSpeed, "audioSpeed", "checkbox");
  }

  uint8_t aux0;
  unsigned long bassMemory = 0;

  void loop() override {
    uint8_t secondHand;
    if (audioSpeed) {
      if (sharedData.bands[0] > 100) {
        if (bassMemory < 5000) bassMemory++;
      } else if (bassMemory)
        bassMemory--;  // 5000 frames cooldown

      secondHand = (micros() / (256 - speed * bassMemory / 5000) / 500 % 16);
    } else
      secondHand = (speed < 255) ? (micros() / (256 - speed) / 500 % 16) : 0;

    if ((speed > 254) || (aux0 != secondHand)) {  // WLEDMM allow run run at full speed
      aux0 = secondHand;

      // Pixel brightness (value) based on volume * sensitivity * intensity
      // uint_fast8_t sensitivity10 = ::map(sensitivity, 0, 31, 10, 100); // reduced resolution slider // WLEDMM sensitivity * 10, to avoid losing precision
      int pixVal = sharedData.volume * (float)fx * (float)sensitivity10 / 2560.0f;  // WLEDMM 2560 due to sensitivity * 10
      if (pixVal > 255) pixVal = 255;                                               // make a brightness from the last avg

      CRGB color = CRGB::Black;

      if (sharedData.majorPeak > MAX_FREQUENCY) sharedData.majorPeak = 1;
      // MajorPeak holds the freq. value which is most abundant in the last sample.
      // With our sampling rate of 10240Hz we have a usable freq range from roughtly 80Hz to 10240/2 Hz
      // we will treat everything with less than 65Hz as 0

      if ((sharedData.majorPeak > 80.0f) && (sharedData.volume > 0.25f)) {  // WLEDMM
        // Pixel color (hue) based on major frequency
        int upperLimit = 80 + 42 * highBin;
        int lowerLimit = 80 + 3 * lowBin;
        // uint8_t i =  lowerLimit!=upperLimit ? ::map(sharedData.majorPeak, lowerLimit, upperLimit, 0, 255) : sharedData.majorPeak;  // (original formula) may under/overflow - so we enforce uint8_t
        int freqMapped = lowerLimit != upperLimit ? ::map(sharedData.majorPeak, lowerLimit, upperLimit, 0, 255) : sharedData.majorPeak;  // WLEDMM preserve overflows
        uint8_t i = abs(freqMapped) & 0xFF;                                                                                              // WLEDMM we embrace overflow ;-) by "modulo 256"

        color = CHSV(i, 240, (uint8_t)pixVal);  // implicit conversion to RGB supplied by FastLED
      }

      // shift the pixels one pixel up
      layer->setRGB(0, color);
      for (int x = layer->size.x - 1; x >= 0; x--) {  // int as we count down!!!
        if (x != 0) color = layer->getRGB(x - 1);
        for (int y = 0; y < layer->size.y; y++)
          for (int z = 0; z < layer->size.z; z++) layer->setRGB(Coord3D(x, y, z), color);
      }
    }
  }
};

class GEQEffect : public Node {
 public:
  static const char* name() { return "GEQ"; }
  static uint8_t dim() { return _2D; }
  static const char* tags() { return "🔥🎨♫🐙"; }

  uint8_t fadeOut = 255;
  uint8_t ripple = 128;
  bool colorBars = false;
  bool smoothBars = true;

  unsigned long step;

  void setup() override {
    Node::setup();

    addControl(fadeOut, "fadeOut", "slider");
    addControl(ripple, "ripple", "slider");
    addControl(colorBars, "colorBars", "checkbox");
    addControl(smoothBars, "smoothBars", "checkbox");

    step = millis();
  }

  uint16_t* previousBarHeight = nullptr;  // array

  ~GEQEffect() {
    freeMB(previousBarHeight);

    Node::~Node();
  }

  void onSizeChanged(const Coord3D& prevSize) override {
    freeMB(previousBarHeight);
    previousBarHeight = allocMB<uint16_t>(layer->size.x);
    if (!previousBarHeight) {
      EXT_LOGE(ML_TAG, "allocate previousBarHeight failed");
    }
  }

  void loop() override {
    const int NUM_BANDS = NUM_GEQ_CHANNELS;  // ::map(layer->custom1, 0, 255, 1, 16);

  #ifdef SR_DEBUG
    uint8_t samplePeak = *(uint8_t*)um_data->u_data[3];
  #endif

    bool rippleTime = false;
    if (millis() - step >= (256U - ripple)) {
      step = millis();
      rippleTime = true;
    }

    int fadeoutDelay = (256 - fadeOut) / 64;                                   // 256..1 -> 4..0
    size_t beat = ::map(beat16(fadeOut), 0, UINT16_MAX, 0, fadeoutDelay - 1);  // instead of call%fadeOutDelay

    if ((fadeoutDelay <= 1) || (beat == 0)) layer->fadeToBlackBy(fadeOut);

    uint16_t lastBandHeight = 0;  // WLEDMM: for smoothing out bars

    // evenly distribute see also Funky Plank/By ewowi/From AXI
    float bandwidth = (float)layer->size.x / NUM_BANDS;
    float remaining = bandwidth;
    uint8_t band = 0;
    Coord3D pos = {0, 0, 0};
    for (pos.x = 0; pos.x < layer->size.x; pos.x++) {
      // WLEDMM if not enough remaining
      if (remaining < 1) {
        band++;
        remaining += bandwidth;
      }  // increase remaining but keep the current remaining
      remaining--;  // consume remaining

      // EXT_LOGD(ML_TAG, "x %d b %d n %d w %f %f\n", x, band, NUM_BANDS, bandwidth, remaining);
      uint8_t frBand = ((NUM_BANDS < NUM_GEQ_CHANNELS) && (NUM_BANDS > 1)) ? ::map(band, 0, NUM_BANDS, 0, NUM_GEQ_CHANNELS) : band;  // always use full range. comment out this line to get the previous behaviour.
      // frBand = constrain(frBand, 0, NUM_GEQ_CHANNELS-1); //WLEDMM can never be out of bounds (I think...)
      uint16_t colorIndex = frBand * 17;               // WLEDMM 0.255
      uint16_t bandHeight = sharedData.bands[frBand];  // WLEDMM we use the original ffResult, to preserve accuracy

      // WLEDMM begin - smooth out bars
      if ((pos.x > 0) && (pos.x < (layer->size.x - 1)) && (smoothBars)) {
        // get height of next (right side) bar
        uint8_t nextband = (remaining < 1) ? band + 1 : band;
        nextband = constrain(nextband, 0, NUM_GEQ_CHANNELS - 1);                                                                       // just to be sure
        frBand = ((NUM_BANDS < NUM_GEQ_CHANNELS) && (NUM_BANDS > 1)) ? ::map(nextband, 0, NUM_BANDS, 0, NUM_GEQ_CHANNELS) : nextband;  // always use full range. comment out this line to get the previous behaviour.
        uint16_t nextBandHeight = sharedData.bands[frBand];
        // smooth Band height
        bandHeight = (7 * bandHeight + 3 * lastBandHeight + 3 * nextBandHeight) / 12;  // yeees, its 12 not 13 (10% amplification)
        bandHeight = constrain(bandHeight, 0, 255);                                    // remove potential over/underflows
        colorIndex = ::map(pos.x, 0, layer->size.x, 0, 256);                           // WLEDMM
      }
      lastBandHeight = bandHeight;  // remember BandHeight (left side) for next iteration
      uint16_t barHeight = ::map(bandHeight, 0, 255, 0,
                                 layer->size.y);  // Now we map bandHeight to barHeight. do not subtract -1 from layer->size.y here (should be 256 (ewowiHack) but then the top row is not displayed...)
      // WLEDMM end

      if (barHeight > layer->size.y) barHeight = layer->size.y;  // WLEDMM ::map() can "overshoot" due to rounding errors

      CRGB ledColor = CRGB::Black;

      for (pos.y = 0; pos.y < barHeight; pos.y++) {
        if (colorBars)  // color_vertical / color bars toggle
          colorIndex = ::map(pos.y, 0, layer->size.y, 0, 256);

        ledColor = ColorFromPalette(layer->layerP->palette, (uint8_t)colorIndex);

        layer->setRGB(Coord3D(pos.x, layer->size.y - 1 - pos.y), ledColor);
      }

      if (previousBarHeight) {
        if (barHeight > previousBarHeight[pos.x]) previousBarHeight[pos.x] = barHeight;                                  // drive the peak up
        if ((ripple > 0) && (previousBarHeight[pos.x] > 0) && (previousBarHeight[pos.x] < layer->size.y))                // WLEDMM avoid "overshooting" into other segments
          layer->setRGB(Coord3D(pos.x, layer->size.y - previousBarHeight[pos.x]), (CRGB)CHSV(millis() / 50, 255, 255));  // take millis()/50 color for the time being

        if (rippleTime && previousBarHeight[pos.x] > 0) previousBarHeight[pos.x]--;  // delay/ripple effect
      }
    }
  }
};

class LissajousEffect : public Node {
 public:
  static const char* name() { return "Lissajous"; }
  static uint8_t dim() { return _2D; }
  static const char* tags() { return "🔥🎨🐙"; }

  uint8_t xFrequency = 64;
  uint8_t fadeRate = 128;
  uint8_t speed = 128;

  void setup() override {
    addControl(xFrequency, "xFrequency", "slider");
    addControl(fadeRate, "fadeRate", "slider");
    addControl(speed, "speed", "slider");
  }

  void loop() override {
    layer->fadeToBlackBy(fadeRate);
    uint_fast16_t phase = millis() * speed / 256;  // allow user to control rotation speed, speed between 0 and 255!
    Coord3D locn = {0, 0, 0};
    for (int i = 0; i < 256; i++) {
      // WLEDMM: stick to the original calculations of xlocn and ylocn
      locn.x = sin8(phase / 2 + (i * xFrequency) / 64);
      locn.y = cos8(phase / 2 + i * 2);
      locn.x = (layer->size.x < 2) ? 1 : (::map(2 * locn.x, 0, 511, 0, 2 * (layer->size.x - 1)) + 1) / 2;  // softhack007: "*2 +1" for proper rounding
      locn.y = (layer->size.y < 2) ? 1 : (::map(2 * locn.y, 0, 511, 0, 2 * (layer->size.y - 1)) + 1) / 2;  // "layer->size.y > 2" is needed to avoid div/0 in ::map()
      layer->setRGB(locn, ColorFromPalette(layer->layerP->palette, millis() / 100 + i, 255));
    }
  }
};

class Noise2DEffect : public Node {
 public:
  static const char* name() { return "Noise2D"; }
  static uint8_t dim() { return _2D; }
  static const char* tags() { return "🔥🎨🐙"; }

  uint8_t speed = 8;
  uint8_t scale = 64;

  void setup() override {
    addControl(speed, "speed", "slider", 0, 15);
    addControl(scale, "scale", "slider", 2, 255);
  }

  void loop() override {
    for (int y = 0; y < layer->size.y; y++) {
      for (int x = 0; x < layer->size.x; x++) {
        uint8_t pixelHue8 = inoise8(x * scale, y * scale, millis() / (16 - speed));
        layer->setRGB(Coord3D(x, y), ColorFromPalette(layer->layerP->palette, pixelHue8));
      }
    }
  }
};  // Noise2D

class NoiseMeterEffect : public Node {
 public:
  static const char* name() { return "Noise Meter"; }
  static uint8_t dim() { return _1D; }
  static const char* tags() { return "♪🐙🎨"; }

  uint8_t fadeRate = 248;
  uint8_t width = 128;

  void setup() override {
    addControl(fadeRate, "fadeRate", "slider", 200, 254);
    addControl(width, "width", "slider", 0, 255);
  }

  uint8_t aux0;
  uint8_t aux1;

  void loop() override {
    layer->fadeToBlackBy(fadeRate);

    float tmpSound2 = sharedData.volumeRaw * 2.0 * (float)width / 255.0;
    int maxLen = ::map(tmpSound2, 0, 255, 0, layer->size.x);  // map to pixels availeable in current segment              // Still a bit too sensitive.
    // if (maxLen <0) maxLen = 0;
    // if (maxLen >layer->size.x) maxLen = layer->size.x;

    for (int i = 0; i < maxLen; i++) {                                                                                             // The louder the sound, the wider the soundbar. By Andrew Tuline.
      uint8_t index = inoise8(i * sharedData.volume + aux0, aux1 + i * sharedData.volume);                                         // Get a value from the noise function. I'm using both x and y axis.
      for (int y = 0; y < layer->size.y; y++)                                                                                      // propagate to other dimensions
        for (int z = 0; z < layer->size.z; z++) layer->setRGB(Coord3D(i, y, z), ColorFromPalette(layer->layerP->palette, index));  //, 255, PALETTE_SOLID_WRAP));
    }

    aux0 += beatsin8(5, 0, 10);
    aux1 += beatsin8(4, 0, 10);
  }
};  // NoiseMeter

// each needs 19 bytes
// Spark type is used for popcorn, fireworks, and drip
struct Spark {
  float pos, posX;
  float vel, velX;
  uint16_t col;
  uint8_t colIndex;
};

  #define maxNumPopcorn 21  // max 21 on 16 segment ESP8266
  #define NUM_COLORS 3      /* number of colors per segment */

class PopCornEffect : public Node {
 public:
  static const char* name() { return "PopCorn"; }
  static uint8_t dim() { return _1D; }  // 2D-ish? check latest in WLED...
  static const char* tags() { return "♪🎨🐙"; }

  uint8_t speed = 128;
  uint8_t numPopcorn = maxNumPopcorn / 2;
  bool useaudio = false;

  void setup() override {
    addControl(speed, "speed", "slider");
    addControl(numPopcorn, "corns", "slider", 1, maxNumPopcorn);
    addControl(useaudio, "useaudio", "checkbox");
  }

  Spark popcorn[maxNumPopcorn];  // array

  void loop() override {
    // binding of loop persistent values (pointers)

    layer->fill_solid(CRGB::Black);

    float gravity = -0.0001f - (speed / 200000.0f);  // m/s/s
    gravity *= layer->size.x;

    if (numPopcorn == 0) numPopcorn = 1;

    for (int i = 0; i < numPopcorn; i++) {
      if (popcorn[i].pos >= 0.0f) {  // if kernel is active, update its position
        popcorn[i].pos += popcorn[i].vel;
        popcorn[i].vel += gravity;
      } else {                   // if kernel is inactive, randomly pop it
        bool doPopCorn = false;  // WLEDMM allows to inhibit new pops
        // WLEDMM begin
        if (useaudio) {
          if ((sharedData.volume > 1.0f)  // no pops in silence
                                          // &&((audioSync->sync.samplePeak > 0) || (audioSync->sync.volumeRaw > 128))  // try to pop at onsets (our peek detector still sucks)
              && (random8() < 4))         // stay somewhat random
            doPopCorn = true;
        } else {
          if (random8() < 2) doPopCorn = true;  // default POP!!!
        }

        if (doPopCorn) {  // POP!!!
          popcorn[i].pos = 0.01f;

          uint16_t peakHeight = 128 + random8(128);  // 0-255
          peakHeight = (peakHeight * (layer->size.x - 1)) >> 8;
          popcorn[i].vel = sqrtf(-2.0f * gravity * peakHeight);

          // if (layer->palette)
          // {
          popcorn[i].colIndex = random8();
          // } else {
          //   uint8_t  col = random8(0, NUM_COLORS);
          //   if (!SEGCOLOR(2) || !SEGCOLOR(col)) col = 0;
          //   popcorn[i].colIndex = col;
          // }
        }
      }
      if (popcorn[i].pos >= 0.0f) {  // draw now active popcorn (either active before or just popped)
        // uint32_t col = layer->color_wheel(popcorn[i].colIndex);
        // if (!layer->palette && popcorn[i].colIndex < NUM_COLORS) col = SEGCOLOR(popcorn[i].colIndex);
        uint16_t ledIndex = popcorn[i].pos;
        CRGB col = ColorFromPalette(layer->layerP->palette, popcorn[i].colIndex * (256 / maxNumPopcorn));
        if (ledIndex < layer->size.x) {
          layer->setRGB(ledIndex, col);
          for (int y = 0; y < layer->size.y; y++)
            for (int z = 0; z < layer->size.z; z++) layer->setRGB(Coord3D(ledIndex, y, z), col);
        }
      }
    }
  }
};  // PopCorn

class WaverlyEffect : public Node {
 public:
  static const char* name() { return "Waverly"; }
  static uint8_t dim() { return _2D; }
  static const char* tags() { return "🔥♪🎨🐙"; }

  uint8_t fadeRate = 128;
  uint8_t amplification = 30;
  bool noClouds = false;

  void setup() override {
    addControl(fadeRate, "fadeRate", "slider");
    addControl(amplification, "amplification", "slider");
    addControl(noClouds, "noClouds", "checkbox");

    // addControl(bool, "soundPressure", layer->effectData.write<bool>(false));
    // addControl(bool, "AGCDebug", layer->effectData.write<bool>(false));
  }

  void loop() override {
    // bool soundPressure = layer->effectData.read<bool>();
    // bool agcDebug = layer->effectData.read<bool>();

    layer->fadeToBlackBy(fadeRate);
    // netmindz: cannot find these in audio sync
    //  if (agcDebug && soundPressure) soundPressure = false;                 // only one of the two at any time
    //  if ((soundPressure) && (audioSync->sync.volumeSmth > 0.5f)) audioSync->sync.volumeSmth = audioSync->sync.soundPressure;    // show sound pressure instead of volume
    //  if (agcDebug) audioSync->sync.volumeSmth = 255.0 - audioSync->sync.agcSensitivity;                    // show AGC level instead of volume

    long t = millis() / 2;
    Coord3D pos = {0, 0, 0};  // initialize z otherwise wrong results
    for (pos.x = 0; pos.x < layer->size.x; pos.x++) {
      uint16_t thisVal = sharedData.volume * amplification * inoise8(pos.x * 45, t, t) / 4096;  // WLEDMM back to SR code
      uint16_t thisMax = min(map(thisVal, 0, 512, 0, layer->size.y), (long)layer->size.y);

      for (pos.y = 0; pos.y < thisMax; pos.y++) {
        CRGB color = ColorFromPalette(layer->layerP->palette, ::map(pos.y, 0, thisMax, 250, 0));
        if (!noClouds) layer->addRGB(pos, color);
        layer->addRGB(Coord3D((layer->size.x - 1) - pos.x, (layer->size.y - 1) - pos.y), color);
      }
    }
    layer->blur2d(16);
  }
};  // Waverly

// // By: Stepko https://editor.soulmatelights.com/gallery/1012 , Modified by: Andrew Tuline
// class BlackHoleEffect: public Node {
//   static const char * name() {return "BlackHole";}
//   static uint8_t dim() {return _2D;}
//   static const char * tags() {return "🐙";}

//   void setup() override {
//     ui->initSlider(parentVar, "fade", leds.effectData.write<uint8_t>(16), 0, 32);
//     ui->initSlider(parentVar, "outX", leds.effectData.write<uint8_t>(16), 0, 32);
//     ui->initSlider(parentVar, "outY", leds.effectData.write<uint8_t>(16), 0, 32);
//     ui->initSlider(parentVar, "inX", leds.effectData.write<uint8_t>(16), 0, 32);
//     ui->initSlider(parentVar, "inY", leds.effectData.write<uint8_t>(16), 0, 32);
//   }

//   void loop() override {
//     //Binding of controls. Keep before binding of vars and keep in same order as in setup()
//     uint8_t fade = leds.effectData.read<uint8_t>();
//     uint8_t outX = leds.effectData.read<uint8_t>();
//     uint8_t outY = leds.effectData.read<uint8_t>();
//     uint8_t inX = leds.effectData.read<uint8_t>();
//     uint8_t inY = leds.effectData.read<uint8_t>();

//     uint16_t x, y;

//     leds.fadeToBlackBy(16 + (fade)); // create fading trails
//     unsigned long t = sys->now/128;                 // timebase
//     // outer stars
//     for (size_t i = 0; i < 8; i++) {
//       x = beatsin8(outX,   0, leds.size.x - 1, 0, ((i % 2) ? 128 : 0) + t * i);
//       y = beatsin8(outY, 0, leds.size.y - 1, 0, ((i % 2) ? 192 : 64) + t * i);
//       leds.addPixelColor(x, y, CHSV(i*32, 255, 255));
//     }
//     // inner stars
//     for (size_t i = 0; i < 4; i++) {
//       x = beatsin8(inX, leds.size.x/4, leds.size.x - 1 - leds.size.x/4, 0, ((i % 2) ? 128 : 0) + t * i);
//       y = beatsin8(inY, leds.size.y/4, leds.size.y - 1 - leds.size.y/4, 0, ((i % 2) ? 192 : 64) + t * i);
//       leds.addPixelColor(x, y, CHSV(i*32, 255, 255));
//     }
//     // central white dot
//     leds.setPixelColor(leds.size.x/2, leds.size.y/2, CHSV(0, 0, 255));

//     // blur everything a bit
//     leds.blur2d(16);

//   }
// }; // BlackHole

// // dna originally by by ldirko at https://pastebin.com/pCkkkzcs. Updated by Preyy. WLED conversion by Andrew Tuline.
// class DNAEffect: public Node {
//   const char * name() override {return "DNA";}
//   uint8_t dim() override {return _2D;}
//   const char * tags() override {return "🐙💫";}

//   void setup() override {
//     Effect::setup(leds, parentVar);
//     ui->initSlider(parentVar, "speed", leds.effectData.write<uint8_t>(16), 0, 32);
//     ui->initSlider(parentVar, "blur", leds.effectData.write<uint8_t>(128));
//     ui->initSlider(parentVar, "phases", leds.effectData.write<uint8_t>(64));
//   }

//   void loop() override {
//     //Binding of controls. Keep before binding of vars and keep in same order as in setup()
//     uint8_t speed = leds.effectData.read<uint8_t>();
//     uint8_t blur = leds.effectData.read<uint8_t>();
//     uint8_t phases = leds.effectData.read<uint8_t>();

//     const int cols = leds.size.x;
//     const int rows = leds.size.y;

//     leds.fadeToBlackBy(64);

//     // WLEDMM optimized to prevent holes at height > 32
//     int lastY1 = -1;
//     int lastY2 = -1;
//     for (int i = 0; i < cols; i++) {
//             //256 is a complete phase
//       // half a phase is dna is 128
//       uint8_t phase;// = cols * i / 8;
//       //32: 4 * i
//       //16: 8 * i
//       phase = i * phases / cols;

//       // phase = i * 2 / (cols+1) * phases;

//       int posY1 = beatsin8(speed, 0, rows-1, 0, phase    );
//       int posY2 = beatsin8(speed, 0, rows-1, 0, phase + 128);
//       if ((i==0) || ((abs(lastY1 - posY1) < 2) && (abs(lastY2 - posY2) < 2))) {   // use original code when no holes
//         leds.setPixelColor(i, posY1, ColorFromPalette(leds.palette, i*5+sys->now/17, beatsin8(5, 55, 255, 0, i*10)));
//         leds.setPixelColor(i, posY2, ColorFromPalette(leds.palette, i*5+128+sys->now/17, beatsin8(5, 55, 255, 0, i*10+128)));
//       } else {                                                                    // draw line to prevent holes
//         leds.drawLine(i-1, lastY1, i, posY1, ColorFromPalette(leds.palette, i*5+sys->now/17, beatsin8(5, 55, 255, 0, i*10)));
//         leds.drawLine(i-1, lastY2, i, posY2, ColorFromPalette(leds.palette, i*5+128+sys->now/17, beatsin8(5, 55, 255, 0, i*10+128)));
//       }
//       lastY1 = posY1;
//       lastY2 = posY2;
//     }
//     leds.blur2d(blur);
//   }
// }; // DNA

// //Octopus inspired by WLED, Stepko and Sutaburosu and blazoncek
// //Idea from https://www.youtube.com/watch?v=HsA-6KIbgto&ab_channel=GreatScott%21 (https://editor.soulmatelights.com/gallery/671-octopus)
// class OctopusEffect: public Node {
//   const char * name() override {return "Octopus";}
//   uint8_t dim() override {return _2D;}
//   const char * tags() override {return "🐙";}

//   struct Map_t {
//     uint8_t angle;
//     uint8_t radius;
//   };

//   void setup() override {
//     Effect::setup(leds, parentVar); //palette
//     bool *setup = leds.effectData.write<bool>(true);
//     ui->initSlider(parentVar, "speed", leds.effectData.write<uint8_t>(16), 1, 32);
//     ui->initSlider(parentVar, "offsetX", leds.effectData.write<uint8_t>(128), 0, 255, false, [setup] (EventArguments) { switch (eventType) {
//       case onChange: {*setup = true; return true;}
//       default: return false;
//     }});
//     ui->initSlider(parentVar, "offsetY", leds.effectData.write<uint8_t>(128), 0, 255, false, [setup] (EventArguments) { switch (eventType) {
//       case onChange: {*setup = true; return true;}
//       default: return false;
//     }});
//     ui->initSlider(parentVar, "legs", leds.effectData.write<uint8_t>(4), 1, 8);

//     ui->initCheckBox(parentVar, "radialWave", leds.effectData.write<bool>(false));
//   }

//   void loop() override {
//     //Binding of controls. Keep before binding of vars and keep in same order as in setup()
//     bool   *setup = leds.effectData.readWrite<bool>();
//     uint8_t speed = leds.effectData.read<uint8_t>();
//     uint8_t offsetX = leds.effectData.read<uint8_t>();
//     uint8_t offsetY = leds.effectData.read<uint8_t>();
//     uint8_t legs = leds.effectData.read<uint8_t>();
//     bool radialWave = leds.effectData.read<bool>();

//     // Effect Variables
//     Coord3D  *prevLedSize = leds.effectData.readWrite<Coord3D>();
//     Map_t    *rMap = leds.effectData.readWrite<Map_t>(leds.size.x * leds.size.y); //array
//     uint32_t *step = leds.effectData.readWrite<uint32_t>();

//     if (leds.effectData.success()) { //octopus allocates quite a lot, so worth checking

//       const uint8_t mapp = 180 / max(leds.size.x,leds.size.y);

//       Coord3D pos = {0,0,0};

//       if (*setup || *prevLedSize != leds.size) { // Setup map if leds.size or offset changes
//         *setup = false;
//         *prevLedSize = leds.size;
//         const uint8_t C_X = leds.size.x / 2 + (offsetX - 128)*leds.size.x/255;
//         const uint8_t C_Y = leds.size.y / 2 + (offsetY - 128)*leds.size.y/255;
//         for (pos.x = 0; pos.x < leds.size.x; pos.x++) {
//           for (pos.y = 0; pos.y < leds.size.y; pos.y++) {
//             uint16_t indexV = leds.XYZUnprojected(pos);
//             if (indexV < leds.size.x * leds.size.y) { //excluding UINT16_MAX from XY if out of bounds due to projection
//               rMap[indexV].angle = 40.7436f * atan2f(pos.y - C_Y, pos.x - C_X); // avoid 128*atan2()/PI
//               rMap[indexV].radius = hypotf(pos.x - C_X, pos.y - C_Y) * mapp; //thanks Sutaburosu
//             }
//           }
//         }
//       }

//       *step = sys->now * speed / 25; //sys.now/25 = 40 per second. speed / 32: 1-4 range ? (1-8 ??)
//       if (radialWave)
//         *step = 3 * (*step) / 4; // 7/6 = 1.16 for RadialWave mode
//       else
//         *step = (*step) / 2; // 1/2 for Octopus mode

//       for (pos.x = 0; pos.x < leds.size.x; pos.x++) {
//         for (pos.y = 0; pos.y < leds.size.y; pos.y++) {
//           uint16_t indexV = leds.XYZUnprojected(pos);
//           if (indexV < leds.size.x * leds.size.y) { //excluding UINT16_MAX from XY if out of bounds due to projection
//             byte angle = rMap[indexV].angle;
//             byte radius = rMap[indexV].radius;
//             uint16_t intensity;
//             if (radialWave)
//               intensity = sin8(*step + sin8(*step - radius) + angle * legs);                               // RadialWave
//             else
//               intensity = sin8(sin8((angle * 4 - radius) / 4 + (*step)/2) + radius - (*step) + angle * legs); //octopus
//             intensity = intensity * intensity / 255; // add a bit of non-linearity for cleaner display
//             leds[pos] = ColorFromPalette(leds.palette, (*step) / 2 - radius, intensity);
//           }
//         }
//       }
//     } //if (leds.effectData.success())
//   }

// }; // Octopus

//   const uint32_t colors[] = {
//     0x000000,
//     0x100000,
//     0x300000,
//     0x600000,
//     0x800000,
//     0xA00000,
//     0xC02000,
//     0xC04000,
//     0xC06000,
//     0xC08000,
//     0x807080
//   };

// //https://github.com/toggledbits/MatrixFireFast/blob/master/MatrixFireFast/MatrixFireFast.ino
// class FireEffect: public Node {
//   const char * name() {return "Fire";}
//   uint8_t dim() {return _2D;}
//   const char * tags() {return "💫";}

//   const uint8_t NCOLORS = (sizeof(colors)/sizeof(colors[0]));

//   void glow(int x, int y, int z, uint8_t flareDecay, bool usePalette) {
//     int b = z * 10 / flareDecay + 1;
//     for ( int i=(y-b); i<(y+b); ++i ) {
//       for ( int j=(x-b); j<(x+b); ++j ) {
//         if ( i >=0 && j >= 0 && i < leds.size.y && j < leds.size.x ) {
//           int d = ( flareDecay * isqrt((x-j)*(x-j) + (y-i)*(y-i)) + 5 ) / 10;
//           uint8_t n = 0;
//           if ( z > d ) n = z - d;
//           if ( leds[leds.XY(j, leds.size.y - 1 - i)] < usePalette?ColorFromPalette(leds.palette, n*23): colors[n]) { // can only get brighter
//             leds[leds.XY(j, leds.size.y - 1 - i)] = usePalette?ColorFromPalette(leds.palette, n*23): colors[n]; //23*11 -> within palette range
//           }
//         }
//       }
//     }
//   }

//   //utility function?
//   uint32_t isqrt(uint32_t n) {
//     if ( n < 2 ) return n;
//     uint32_t smallCandidate = isqrt(n >> 2) << 1;
//     uint32_t largeCandidate = smallCandidate + 1;
//     return (largeCandidate*largeCandidate > n) ? smallCandidate : largeCandidate;
//   }

//   void setup() {
//     Effect::setup(leds, parentVar); //palette

//     ui->initCheckBox(parentVar, "usePalette",    leds.effectData.write<bool>(false));
//     ui->initSlider(parentVar, "flareRows", leds.effectData.write<uint8_t>(2), 0, 5);    /* number of rows (from bottom) allowed to flare */
//     ui->initSlider(parentVar, "maxFlare", leds.effectData.write<uint8_t>(8), 0, 18);     /* max number of simultaneous flares */
//     ui->initSlider(parentVar, "flareChance", leds.effectData.write<uint8_t>(50), 0, 100); /* chance (%) of a new flare (if there's room) */
//     ui->initSlider(parentVar, "flareDecay", leds.effectData.write<uint8_t>(14), 0, 28);  /* decay rate of flare radiation; 14 is good */
//   }

//   void loop() {

//     bool usePalette = leds.effectData.read<bool>();
//     uint8_t flareRows = leds.effectData.read<uint8_t>();
//     uint8_t maxFlare = leds.effectData.read<uint8_t>();
//     uint8_t flareChance = leds.effectData.read<uint8_t>();
//     uint8_t flareDecay = leds.effectData.read<uint8_t>();

//     // Effect Variables
//     uint8_t *nflare = leds.effectData.readWrite<uint8_t>();
//     uint32_t *flare = leds.effectData.readWrite<uint32_t>(18);

//     // First, move all existing heat points up the display and fade
//     for (int y=leds.size.y-1; y>0; --y ) {
//       for (int x=0; x<leds.size.x; ++x ) {
//         CRGB n = CRGB::Black;
//         if ( leds[leds.XY(x, leds.size.y - y)] != CRGB::Black) {
//           n = leds[leds.XY(x, leds.size.y - y)] - 0; //-0 to force conversion to CRGB
//           if (n.red > 10) n.red -= 10; else n.red = 0;
//           if (n.green > 10) n.green -= 10; else n.green = 0;
//           if (n.blue > 10) n.blue -= 10; else n.blue = 0;
//         }
//         leds[leds.XY(x, leds.size.y - 1 - y)] = n;
//       }
//     }

//     // Heat the bottom row
//     for (int x=0; x<leds.size.x; ++x ) {
//       CRGB i = leds[leds.XY(x, leds.size.y - 1 - 0)] - 0; //-0 to force conversion to CRGB
//       if ( i != CRGB::Black ) {
//         leds[leds.XY(x, leds.size.y - 1 - 0)] = usePalette?ColorFromPalette(leds.palette, random8()): colors[random(NCOLORS-6, NCOLORS-2)];
//       }
//     }

//     // flare
//     for (int i=0; i<*nflare; ++i ) {
//       int x = flare[i] & 0xff;
//       int y = (flare[i] >> 8) & 0xff;
//       int z = (flare[i] >> 16) & 0xff;

//       glow( x, y, z, flareDecay, leds, usePalette);

//       if ( z > 1 ) {
//         flare[i] = (flare[i] & 0xffff) | ((z-1)<<16);
//       } else {
//         // This flare is out
//         for ( int j=i+1; j<*nflare; ++j ) {
//           flare[j-1] = flare[j];
//         }
//         --(*nflare);
//       }
//     }

//     //newflare();
//     if ( *nflare < maxFlare && random(1,101) <= flareChance ) {
//       int x = random(0, leds.size.x);
//       int y = random(0, flareRows);
//       int z = NCOLORS - 1;
//       flare[(*nflare)++] = (z<<16) | (y<<8) | (x&0xff);

//       glow( x, y, z, flareDecay, leds, usePalette);
//     }
//   }

// }; // Fire Effect

// //Frizzles inspired by WLED, Stepko, Andrew Tuline, https://editor.soulmatelights.com/gallery/640-color-frizzles
// class FrizzlesEffect: public Node {
//   const char * name() override {return "Frizzles";}
//   uint8_t dim() override {return _2D;}
//   const char * tags() override {return "🐙";}

//   void setup() override {
//     Effect::setup(leds, parentVar);
//     ui->initSlider(parentVar, "BPM", leds.effectData.write<uint8_t>(60));
//     ui->initSlider(parentVar, "intensity", leds.effectData.write<uint8_t>(128));
//     ui->initSlider(parentVar, "blur", leds.effectData.write<uint8_t>(128));
//   }

//   void loop() override {
//     //Binding of controls. Keep before binding of vars and keep in same order as in setup()
//     uint8_t bpm = leds.effectData.read<uint8_t>();
//     uint8_t intensity = leds.effectData.read<uint8_t>();
//     uint8_t blur = leds.effectData.read<uint8_t>();

//     leds.fadeToBlackBy(16);

//     for (int i = 8; i > 0; i--) {
//       Coord3D pos = {0,0,0};
//       pos.x = beatsin8(bpm/8 + i, 0, leds.size.x - 1);
//       pos.y = beatsin8(intensity/8 - i, 0, leds.size.y - 1);
//       CRGB color = ColorFromPalette(leds.palette, beatsin8(12, 0, 255));
//       leds[pos] = color;
//     }
//     leds.blur2d(blur);
//   }
// }; // Frizzles

// /*
//  * Exploding fireworks effect
//  * adapted from: http://www.anirama.com/1000leds/1d-fireworks/
//  * adapted for 2D WLED by blazoncek (Blaz Kristan (AKA blazoncek))
//  * simplified for StarLight by ewowi in Nov 24
//  */
// class FireworksEffect: public Node {
//   const char * name() override {return "Fireworks";}
//   uint8_t dim() override {return _2D;}
//   const char * tags() override {return "🐙";}

//   void setup() override {
//     leds.fadeToBlackBy(16);
//     Effect::setup(leds, parentVar);
//     ui->initSlider(parentVar, "gravity", leds.effectData.write<uint8_t>(128));
//     ui->initSlider(parentVar, "firingSide", leds.effectData.write<uint8_t>(128));
//     ui->initSlider(parentVar, "numSparks", leds.effectData.write<uint8_t>(128));
//     // PROGMEM = "Fireworks 1D@Gravity,Firing side;!,!;!;12;pal=11,ix=128";
//   }

//   void loop() override {

//     //Binding of controls. Keep before binding of vars and keep in same order as in setup()
//     uint8_t gravityC = leds.effectData.read<uint8_t>();
//     uint8_t firingSide = leds.effectData.read<uint8_t>();
//     uint8_t numSparks = leds.effectData.read<uint8_t>();

//     float *dying_gravity = leds.effectData.readWrite<float>();
//     uint16_t *aux0Flare = leds.effectData.readWrite<uint16_t>();
//     Spark* sparks = leds.effectData.readWrite<Spark>(255);
//     Spark* flare = sparks; //first spark is flare data

//     const uint16_t cols = leds.size.x;
//     const uint16_t rows = leds.size.y;

//     //allocate segment data

//     // if (dataSize != aux1) { //reset to flare if sparks were reallocated (it may be good idea to reset segment if bounds change)
//     //   *dying_gravity = 0.0f;
//     //   (*aux0Flare) = 0;
//     //   aux1 = dataSize;
//     // }

//     leds.fadeToBlackBy(252); //fade_out(252);

//     float gravity = -0.0004f - (gravityC/800000.0f); // m/s/s
//     gravity *= rows;

//     if ((*aux0Flare) < 2) { //FLARE
//       if ((*aux0Flare) == 0) { //init flare
//         flare->pos = 0;
//         flare->posX = random16(2,cols-3);
//         uint16_t peakHeight = 75 + random8(180); //0-255
//         peakHeight = (peakHeight * (rows -1)) >> 8;
//         flare->vel = sqrtf(-2.0f * gravity * peakHeight);
//         flare->velX = (random8(9)-4)/32.f;
//         flare->col = 255; //brightness
//         (*aux0Flare) = 1;
//       }

//       // launch
//       if (flare->vel > 12 * gravity) {
//         // flare
//         leds.setPixelColor(int(flare->posX), rows - uint16_t(flare->pos) - 1, CRGB(flare->col, flare->col, flare->col));
//         flare->pos  += flare->vel;
//         flare->posX += flare->velX;
//         flare->pos  = constrain(flare->pos, 0, rows-1);
//         flare->posX = constrain(flare->posX, 0, cols-1);
//         flare->vel  += gravity;
//         flare->col  -= 2;
//       } else {
//         (*aux0Flare) = 2;  // ready to explode
//       }
//     } else if ((*aux0Flare) < 4) {
//       /*
//       * Explode!
//       *
//       * Explosion happens where the flare ended.
//       * Size is proportional to the height.
//       */
//       uint8_t nSparks = flare->pos + random8(4);
//       // nSparks = std::max(nSparks, 4U);  // This is not a standard constrain; numSparks is not guaranteed to be at least 4
//       nSparks = std::min(nSparks, numSparks);

//       // initialize sparks
//       if ((*aux0Flare) == 2) {
//         for (int i = 1; i < nSparks; i++) {
//           sparks[i].pos  = flare->pos;
//           sparks[i].posX = flare->posX;
//           sparks[i].vel  = (float(random16(20001)) / 10000.0f) - 0.9f; // from -0.9 to 1.1
//           // sparks[i].vel *= rows<32 ? 0.5f : 1; // reduce velocity for smaller strips
//           sparks[i].velX  = (float(random16(20001)) / 10000.0f) - 0.9f; // from -0.9 to 1.1
//           // sparks[i].velX = (float(random16(10001)) / 10000.0f) - 0.5f; // from -0.5 to 0.5
//           sparks[i].col  = 345;//abs(sparks[i].vel * 750.0); // set colors before scaling velocity to keep them bright
//           //sparks[i].col = constrain(sparks[i].col, 0, 345);
//           sparks[i].colIndex = random8();
//           sparks[i].vel  *= flare->pos/rows; // proportional to height
//           sparks[i].velX *= flare->posX/cols; // proportional to width
//           sparks[i].vel  *= -gravity *50;
//         }
//         //sparks[1].col = 345; // this will be our known spark
//         *dying_gravity = gravity/2;
//         (*aux0Flare) = 3;
//       }

//       if (sparks[1].col > 4) {//&& sparks[1].pos > 0) { // as long as our known spark is lit, work with all the sparks
//         for (int i = 1; i < nSparks; i++) {
//           sparks[i].pos  += sparks[i].vel;
//           sparks[i].posX += sparks[i].velX;
//           sparks[i].vel  += *dying_gravity;
//           sparks[i].velX += *dying_gravity;
//           if (sparks[i].col > 3) sparks[i].col -= 4;

//           if (sparks[i].pos > 0 && sparks[i].pos < rows) {
//             if (!(sparks[i].posX >= 0 && sparks[i].posX < cols)) continue;
//             uint16_t prog = sparks[i].col;
//             CRGB spColor = ColorFromPalette(leds.palette, sparks[i].colIndex);
//             CRGB c = CRGB::Black; //HeatColor(sparks[i].col);
//             if (prog > 300) { //fade from white to spark color
//               c = CRGB(blend(spColor, CRGB::White, (prog - 300)*5));
//             } else if (prog > 45) { //fade from spark color to black
//               c = CRGB(blend(CRGB::Black, spColor, prog - 45));
//               uint8_t cooling = (300 - prog) >> 5;
//               c.g = qsub8(c.g, cooling);
//               c.b = qsub8(c.b, cooling * 2);
//             }
//             leds.setPixelColor(sparks[i].posX, rows - sparks[i].pos - 1, c);
//           }
//         }
//         leds.blur2d(16);
//         *dying_gravity *= .8f; // as sparks burn out they fall slower
//       } else {
//         (*aux0Flare) = 6 + random8(10); //wait for this many frames
//       }
//     } else {
//       (*aux0Flare)--;
//       if ((*aux0Flare) < 4) {
//         (*aux0Flare) = 0; //back to flare
//       }
//     }
//   }

// }; //FireworksEffect

// class FunkyPlankEffect: public Node {
//   const char * name() override {return "Funky Plank";}
//   uint8_t dim() override {return _2D;}
//   const char * tags() override {return "♫🐙💫";}

//   void setup() override {
//     leds.fill_solid(CRGB::Black);
//     ui->initSlider(parentVar, "speed", leds.effectData.write<uint8_t>(255));
//     ui->initSlider(parentVar, "bands", leds.effectData.write<uint8_t>(NUM_GEQ_CHANNELS), 1, NUM_GEQ_CHANNELS);
//   }

//   void loop() override {
//     //Binding of controls. Keep before binding of vars and keep in same order as in setup()
//     uint8_t speed = leds.effectData.read<uint8_t>();
//     uint8_t num_bands = leds.effectData.read<uint8_t>();

//     //binding of loop persistent values (pointers) tbd: aux0,1,step etc can be renamed to meaningful names
//     uint8_t *aux0 = leds.effectData.readWrite<uint8_t>();

//     uint8_t secondHand = (speed < 255) ? (micros()/(256-speed)/500 % 16) : 0;
//     if ((speed > 254) || (*aux0 != secondHand)) {   // WLEDMM allow run run at full speed
//       *aux0 = secondHand;

//       //evenly distribute see also GEQ/By ewowi/From AXI
//       float bandwidth = (float)leds.size.x / num_bands;
//       float remaining = bandwidth;
//       uint8_t band = 0;
//       for (int posx=0; posx < leds.size.x; posx++) {
//         if (remaining < 1) {band++; remaining += bandwidth;} //increase remaining but keep the current remaining
//         remaining--; //consume remaining

//         int hue = sharedData.bands[map(band, 0, num_bands-1, 0, NUM_GEQ_CHANNELS-1)];
//         int v = ::map(hue, 0, 255, 10, 255);
//         leds.setPixelColor(posx, 0, CHSV(hue, 255, v));
//       }

//       // drip down:
//       for (int i = (leds.size.y - 1); i > 0; i--) {
//         for (int j = (leds.size.x - 1); j >= 0; j--) {
//           leds.setPixelColor(j, i, leds.getPixelColor(j, i-1));
//         }
//       }
//     }
//   }
// }; //FunkyPlank

// class VUMeterEffect: public Node {
//   const char * name() override {return "VU Meter";}
//   uint8_t dim() override {return _2D;}
//   const char * tags() override {return "♫💫📺";}

//   void drawNeedle(float angle, Coord3D topLeft, Coord3D size, CRGB color) {
//       int x0 = topLeft.x + size.x / 2; // Center of the needle
//       int y0 = topLeft.y + size.y - 1; // Bottom of the needle

//       leds.drawCircle(topLeft.x + size.x / 2, topLeft.y + size.y / 2, size.x/2, ColorFromPalette(leds.palette, 35, 128), false);

//       // Calculate needle end position
//       int x1 = x0 - round(size.y * 0.7 * cos((angle + 30) * PI / 180));
//       int y1 = y0 - round(size.y * 0.7 * sin((angle + 30) * PI / 180));

//       // Draw the needle
//       leds.drawLine(x0, y0, x1, y1, color, true);
//   }

//   void setup() override {
//     Effect::setup(leds, parentVar); //palette
//     leds.fill_solid(CRGB::Black);
//     ui->initSlider(parentVar, "speed", leds.effectData.write<uint8_t>(255));
//     ui->initSlider(parentVar, "bands", leds.effectData.write<uint8_t>(NUM_GEQ_CHANNELS), 1, NUM_GEQ_CHANNELS);
//   }

//   void loop() override {
//     //Binding of controls. Keep before binding of vars and keep in same order as in setup()
//     uint8_t speed = leds.effectData.read<uint8_t>();
//     uint8_t num_bands = leds.effectData.read<uint8_t>();
//     leds.fadeToBlackBy(200);

//     uint8_t nHorizontal = 4;
//     uint8_t nVertical = 2;

//     uint8_t band = 0;
//     for (int h = 0; h < nHorizontal; h++) {
//       for (int v = 0; v < nVertical; v++) {
//         drawNeedle(leds, (float)sharedData.bands[2*(band++)] / 2.0, {leds.size.x * h / nHorizontal, leds.size.y * v / nVertical, 0}, {leds.size.x / nHorizontal, leds.size.y / nVertical, 0},
//                 ColorFromPalette(leds.palette, 255 / (nHorizontal * nVertical) * band));
//       } //sharedData.bands[band++] / 200
//     }
//     // ppf(" v:%f, f:%f", sharedData.volume, (float) sharedData.bands[5]);
//   }
// }; //VUMeter

// class PixelMapEffect: public Node {
//   const char * name() override {return "PixelMap 🧊";}
//   uint8_t dim() override {return _3D;}
//   const char * tags() override {return "💫";}

//   void setup() override {
//     ui->initSlider(parentVar, "x", leds.effectData.write<uint8_t>(0), 0, leds.size.x - 1);
//     ui->initSlider(parentVar, "y", leds.effectData.write<uint8_t>(0), 0, leds.size.y - 1);
//     ui->initSlider(parentVar, "z", leds.effectData.write<uint8_t>(0), 0, leds.size.z - 1);
//   }

//   void loop() override {
//     //Binding of controls. Keep before binding of vars and keep in same order as in setup()
//     uint8_t x = leds.effectData.read<uint8_t>();
//     uint8_t y = leds.effectData.read<uint8_t>();
//     uint8_t z = leds.effectData.read<uint8_t>();

//     leds.fill_solid(CRGB::Black);

//     Coord3D pos = {x, y, z};
//     leds[pos] = CHSV( sys->now/50 + random8(64), 255, 255);// ColorFromPalette(leds.palette,call, bri);
//   }
// }; // PixelMap

// class MarioTestEffect: public Node {
//   const char * name() override {return "MarioTest";}
//   uint8_t       dim() override {return _2D;}
//   const char * tags() override {return "💫";}

//   void setup() override {
//     ui->initCheckBox(parentVar, "background", leds.effectData.write<bool>(false));
//     ui->initSlider(parentVar, "offsetX", leds.effectData.write<uint8_t>(leds.size.x/2 - 8), 0, leds.size.x - 16);
//     ui->initSlider(parentVar, "offsetY", leds.effectData.write<uint8_t>(leds.size.y/2 - 8), 0, leds.size.y - 16);
//   }

//   void loop() override {
//     bool background = leds.effectData.read<bool>();
//     uint8_t offsetX = leds.effectData.read<uint8_t>();
//     uint8_t offsetY = leds.effectData.read<uint8_t>();

//     const uint8_t mario[16][16] = {
//       {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0},
//       {0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
//       {0, 0, 0, 0, 2, 2, 2, 3, 3, 4, 3, 0, 0, 0, 0, 0},
//       {0, 0, 0, 2, 3, 2, 3, 3, 3, 4, 3, 3, 3, 0, 0, 0},
//       {0, 0, 0, 2, 3, 2, 2, 3, 3, 3, 4, 3, 3, 3, 0, 0},
//       {0, 0, 0, 0, 2, 3, 3, 3, 3, 4, 4, 4, 4, 0, 0, 0},
//       {0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0},
//       {0, 0, 0, 0, 5, 5, 1, 5, 5, 1, 0, 0, 0, 0, 0, 0},
//       {0, 0, 0, 5, 5, 5, 1, 5, 5, 1, 5, 5, 5, 0, 0, 0},
//       {0, 0, 5, 5, 5, 5, 1, 5, 5, 1, 5, 5, 5, 5, 0, 0},
//       {0, 0, 3, 3, 5, 5, 1, 1, 1, 1, 5, 5, 3, 3, 0, 0},
//       {0, 0, 3, 3, 3, 1, 6, 1, 1, 6, 1, 3, 3, 3, 0, 0},
//       {0, 0, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 0, 0},
//       {0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0},
//       {0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0},
//       {0, 0, 2, 2, 2, 2, 0, 0, 0, 0, 2, 2, 2, 2, 0, 0}
//     };

//     CRGB colors[7] = {CRGB::DimGrey, CRGB::Red, CRGB::Brown, CRGB::Tan, CRGB::Black, CRGB::Blue, CRGB::Yellow};

//     if (background) leds.fill_solid(CRGB::DimGrey);
//     else leds.fill_solid(CRGB::Black);
//     //draw 16x16 mario
//     for (int x = 0; x < 16; x++) for (int y = 0; y < 16; y++) {
//       leds[Coord3D(x + offsetX, y + offsetY)] = colors[mario[y][x]];
//     }
//   }
// }; // MarioTest

// class RainbowWithGlitterEffect: public Node {
//   const char * name() override {return "Rainbow with glitter";}
//   uint8_t dim() override {return _1D;}
//   const char * tags() override {return "⚡";} //⚡ means FastLED origin

//   void setup() override {
//     //no palette control is created
//     ui->initCheckBox(parentVar, "glitter", leds.effectData.write<bool>(false));
//   }

//   void loop() override {
//     bool glitter = leds.effectData.read<bool>();

//     // built-in FastLED rainbow, plus some random sparkly glitter
//     // FastLED's built-in rainbow generator
//     leds.fill_rainbow(sys->now/50, 7);

//     if (glitter)
//       addGlitter(leds, 80);
//   }

//   void addGlitter(fract8 chanceOfGlitter)
//   {
//     if( random8() < chanceOfGlitter) {
//       leds[ random16(leds.size.x) ] += CRGB::White;
//     }
//   }
// };

// // Best of both worlds from Palette and Spot effects. By Aircoookie
// class FlowEffect: public Node {
//   const char * name() override {return "Flow";}
//   uint8_t      dim()  override {return _1D;}
//   const char * tags() override {return "🐙";} //🐙 means wled origin

//   void setup() override {
//     Effect::setup(leds, parentVar);
//     ui->initSlider(parentVar, "speed", leds.effectData.write<uint8_t>(128));
//     ui->initSlider(parentVar, "zones", leds.effectData.write<uint8_t>(128));
//   }

//   void loop() override {
//     // UI Variables
//     uint8_t speed   = leds.effectData.read<uint8_t>();
//     uint8_t zonesUI = leds.effectData.read<uint8_t>();

//     uint16_t counter = 0;
//     if (speed != 0) {
//       counter = sys->now * ((speed >> 2) +1);
//       counter = counter >> 8;
//     }

//     uint16_t maxZones = leds.size.x / 6; //only looks good if each zone has at least 6 LEDs
//     uint16_t zones    = (zonesUI * maxZones) >> 8;
//     if (zones & 0x01) zones++; //zones must be even
//     if (zones < 2)    zones = 2;
//     uint16_t zoneLen = leds.size.x / zones;
//     uint16_t offset  = (leds.size.x - zones * zoneLen) >> 1;

//     leds.fill_solid(ColorFromPalette(leds.palette, -counter));

//     for (int z = 0; z < zones; z++) {
//       uint16_t pos = offset + z * zoneLen;
//       for (int i = 0; i < zoneLen; i++) {
//         uint8_t  colorIndex = (i * 255 / zoneLen) - counter;
//         uint16_t led = (z & 0x01) ? i : (zoneLen -1) -i;
//         leds[pos + led] = ColorFromPalette(leds.palette, colorIndex);
//       }
//     }
//   }
// };

// class RingEffect: public Node {
//   protected:

//   void setRing(int ring, CRGB colour) { //so britisch ;-)
//     leds[ring] = colour;
//   }

// };

// class RingRandomFlowEffect: public RingEffect {
//   const char * name() override {return "RingRandomFlow";}
//   uint8_t dim() override {return _1D;}
//   const char * tags() override {return "💫";}

//   void setup() override {} //so no palette control is created

//   void loop() override {
//     //binding of loop persistent values (pointers)
//     uint8_t *hue = leds.effectData.readWrite<uint8_t>(leds.size.x); //array

//     hue[0] = random(0, 255);
//     for (int r = 0; r < leds.size.x; r++) {
//       setRing(leds, r, CHSV(hue[r], 255, 255));
//     }
//     for (int r = (leds.size.x - 1); r >= 1; r--) {
//       hue[r] = hue[(r - 1)]; // set this ruing based on the inner
//     }
//     // FastLED.delay(SPEED);
//   }
// };

// void mode_fireworks(uint16_t *aux0, uint16_t *aux1, uint8_t speed, uint8_t intensity, bool useAudio = false) {
//   // fade_out(0);
//   leds.fadeToBlackBy(10);
//   // if (call == 0) {
//   //   *aux0 = UINT16_MAX;
//   //   *aux1 = UINT16_MAX;
//   // }
//   bool valid1 = (*aux0 < leds.size.x);
//   bool valid2 = (*aux1 < leds.size.x);
//   CRGB sv1 = 0, sv2 = 0;
//   if (valid1) sv1 = leds.getPixelColor(*aux0);
//   if (valid2) sv2 = leds.getPixelColor(*aux1);

//   // WLEDSR
//   uint8_t blurAmount   = 255 - speed;
//   uint8_t my_intensity = 129 - intensity;
//   bool addPixels = true;                        // false -> inhibit new pixels in silence
//   int soundColor = -1;                          // -1 = random color; 0..255 = use as palette index

//   // if (useAudio) {
//   //   if (FFT_MajorPeak < 100)    { blurAmount = 254;} // big blobs
//   //   else {
//   //     if (FFT_MajorPeak > 3200) { blurAmount = 1;}   // small blobs
//   //     else {                                         // blur + color depends on major frequency
//   //       float musicIndex = logf(FFT_MajorPeak);            // log scaling of peak freq
//   //       blurAmount = mapff(musicIndex, 4.60, 8.08, 253, 1);// map to blur range (low freq = more blur)
//   //       blurAmount = constrain(blurAmount, 1, 253);        // remove possible "overshot" results
//   //       soundColor = mapff(musicIndex, 4.6, 8.08, 0, 255); // pick color from frequency
//   //   } }
//   //   if (sampleAgc <= 1.0) {      // silence -> no new pixels, just blur
//   //     valid1 = valid2 = false;   // do not copy last pixels
//   //     addPixels = false;
//   //     blurAmount = 128;
//   //   }
//   //   my_intensity = 129 - (speed >> 1); // dirty hack: use "speed" slider value intensity (no idea how to _disable_ the first slider, but show the second one)
//   //   if (samplePeak == 1) my_intensity -= my_intensity / 4;    // inclease intensity at peaks
//   //   if (samplePeak > 1) my_intensity = my_intensity / 2;      // double intensity at main peaks
//   // }
//   // // WLEDSR end

//   leds.blur1d(blurAmount);
//   if (valid1) leds.setPixelColor(*aux0, sv1);
//   if (valid2) leds.setPixelColor(*aux1, sv2);

//   if (addPixels) {                                                                             // WLEDSR
//     for(uint16_t i=0; i<max(1, leds.size.x/20); i++) {
//       if(random8(my_intensity) == 0) {
//         uint16_t index = random(leds.size.x);
//         if (soundColor < 0)
//           leds.setPixelColor(index, ColorFromPalette(leds.palette, random8()));
//         else
//           leds.setPixelColor(index, ColorFromPalette(leds.palette, soundColor + random8(24))); // WLEDSR
//         *aux1 = *aux0;
//         *aux0 = index;
//       }
//     }
//   }
//   // return FRAMETIME;
// }

// class RainEffect: public Node {
//   const char * name() override {return "Rain";}
//   uint8_t dim() override {return _1D;}
//   const char * tags() override {return "🐙";}

//   void setup() override {
//     Effect::setup(leds, parentVar);
//     ui->initSlider(parentVar, "speed", leds.effectData.write<uint8_t>(128), 1, 255);
//     ui->initSlider(parentVar, "intensity", leds.effectData.write<uint8_t>(64), 1, 128);
//   }

//   void loop() override {
//     //Binding of controls. Keep before binding of vars and keep in same order as in setup()
//     uint8_t speed = leds.effectData.read<uint8_t>();
//     uint8_t intensity = leds.effectData.read<uint8_t>();

//     //binding of loop persistent values (pointers) tbd: aux0,1,step etc can be renamed to meaningful names
//     uint16_t *aux0 = leds.effectData.readWrite<uint16_t>();
//     uint16_t *aux1 = leds.effectData.readWrite<uint16_t>();
//     uint16_t *step = leds.effectData.readWrite<uint16_t>();

//     // if(call == 0) {
//       // leds.fill(BLACK);
//     // }
//     *step += 1000 / 40;// FRAMETIME;
//     if (*step > (5U + (50U*(255U - speed))/leds.size.x)) { //SPEED_FORMULA_L) {
//       *step = 1;
//       // if (strip.isMatrix) {
//       //   //uint32_t ctemp[leds.size.x];
//       //   //for (int i = 0; i<leds.size.x; i++) ctemp[i] = leds.getPixelColor(i, leds.size.y-1);
//       //   leds.move(6, 1, true);  // move all pixels down
//       //   //for (int i = 0; i<leds.size.x; i++) leds.setPixelColor(i, 0, ctemp[i]); // wrap around
//       //   *aux0 = (*aux0 % leds.size.x) + (*aux0 / leds.size.x + 1) * leds.size.x;
//       //   *aux1 = (*aux1 % leds.size.x) + (*aux1 / leds.size.x + 1) * leds.size.x;
//       // } else
//       {
//         //shift all leds left
//         CRGB ctemp = leds.getPixelColor(0);
//         for (int i = 0; i < leds.size.x - 1; i++) {
//           leds.setPixelColor(i, leds.getPixelColor(i+1));
//         }
//         leds.setPixelColor(leds.size.x -1, ctemp); // wrap around
//         *aux0++;  // increase spark index
//         *aux1++;
//       }
//       if (*aux0 == 0) *aux0 = UINT16_MAX; // reset previous spark position
//       if (*aux1 == 0) *aux0 = UINT16_MAX; // reset previous spark position
//       if (*aux0 >= leds.size.x*leds.size.y) *aux0 = 0;     // ignore
//       if (*aux1 >= leds.size.x*leds.size.y) *aux1 = 0;
//     }
//     mode_fireworks(leds, aux0, aux1, speed, intensity);
//   }
// }; // RainEffect

// //each needs 19 bytes
// //Spark type is used for popcorn, fireworks, and drip
// struct Spark {
//   float pos, posX;
//   float vel, velX;
//   uint16_t col;
//   uint8_t colIndex;
// };

// #define maxNumDrops 6
// class DripEffect: public Node {
//   const char * name() override {return "Drip";}
//   uint8_t dim() override {return _1D;}
//   const char * tags() override {return "🐙💫";}

//   void setup() override {
//     Effect::setup(leds, parentVar);
//     ui->initSlider(parentVar, "gravity", leds.effectData.write<uint8_t>(128), 1, 255);
//     ui->initSlider(parentVar, "drips", leds.effectData.write<uint8_t>(4), 1, 6);
//     ui->initSlider(parentVar, "swell", leds.effectData.write<uint8_t>(4), 1, 6);
//     ui->initCheckBox(parentVar, "invert", leds.effectData.write<bool>(false));
//   }

//   void loop() override {
//     //Binding of controls. Keep before binding of vars and keep in same order as in setup()
//     uint8_t grav = leds.effectData.read<uint8_t>();
//     uint8_t drips = leds.effectData.read<uint8_t>();
//     uint8_t swell = leds.effectData.read<uint8_t>();
//     bool invert = leds.effectData.read<bool>();

//     //binding of loop persistent values (pointers)
//     Spark* drops = leds.effectData.readWrite<Spark>(maxNumDrops);

//     // leds.fadeToBlackBy(90);
//     leds.fill_solid(CRGB::Black);

//     float gravity = -0.0005f - (grav/25000.0f); //increased gravity (50000 to 25000)
//     gravity *= max(1, leds.size.x-1);
//     int sourcedrop = 12;

//     for (int j=0;j<drips;j++) {
//       if (drops[j].colIndex == 0) { //init
//         drops[j].pos = leds.size.x-1;    // start at end
//         drops[j].vel = 0;           // speed
//         drops[j].col = sourcedrop;  // brightness
//         drops[j].colIndex = 1;      // drop state (0 init, 1 forming, 2 falling, 5 bouncing)
//         drops[j].velX = (uint32_t)ColorFromPalette(leds.palette, random8()); // random color
//       }
//       CRGB dropColor = drops[j].velX;

//       leds.setPixelColor(invert?0:leds.size.x-1, blend(CRGB::Black, dropColor, sourcedrop));// water source
//       if (drops[j].colIndex==1) {
//         if (drops[j].col>255) drops[j].col=255;
//         leds.setPixelColor(invert?leds.size.x-1-drops[j].pos:drops[j].pos, blend(CRGB::Black, dropColor, drops[j].col));

//         drops[j].col += swell; // swelling

//         if (random16() <= drops[j].col * swell * swell / 10) {               // random drop
//           drops[j].colIndex=2;               //fall
//           drops[j].col=255;
//         }
//       }
//       if (drops[j].colIndex > 1) {           // falling
//         if (drops[j].pos > 0) {              // fall until end of segment
//           drops[j].pos += drops[j].vel;
//           if (drops[j].pos < 0) drops[j].pos = 0;
//           drops[j].vel += gravity;           // gravity is negative

//           for (int i=1;i<7-drops[j].colIndex;i++) { // some minor math so we don't expand bouncing droplets
//             uint16_t pos = constrain(uint16_t(drops[j].pos) +i, 0, leds.size.x-1); //this is BAD, returns a pos >= leds.size.x occasionally
//             leds.setPixelColor(invert?leds.size.x-1-pos:pos, blend(CRGB::Black, dropColor, drops[j].col/i)); //spread pixel with fade while falling
//           }

//           if (drops[j].colIndex > 2) {       // during bounce, some water is on the floor
//             leds.setPixelColor(invert?leds.size.x-1:0, blend(dropColor, CRGB::Black, drops[j].col));
//           }
//         } else {                             // we hit bottom
//           if (drops[j].colIndex > 2) {       // already hit once, so back to forming
//             drops[j].colIndex = 0;
//             // drops[j].col = sourcedrop;

//           } else {

//             if (drops[j].colIndex==2) {      // init bounce
//               drops[j].vel = -drops[j].vel/4;// reverse velocity with damping
//               drops[j].pos += drops[j].vel;
//             }
//             drops[j].col = sourcedrop*2;
//             drops[j].colIndex = 5;           // bouncing
//           }
//         }
//       }
//     }
//   }
// }; // DripEffect

// class HeartBeatEffect: public Node {
//   const char * name() override {return "HeartBeat";}
//   uint8_t dim() override {return _1D;}
//   const char * tags() override {return "🐙💫♥";}

//   void setup() override {
//     Effect::setup(leds, parentVar);
//     ui->initSlider(parentVar, "speed", leds.effectData.write<uint8_t>(15), 0, 31);
//     ui->initSlider(parentVar, "intensity", leds.effectData.write<uint8_t>(128));
//   }

//   void loop() override {
//     //Binding of controls. Keep before binding of vars and keep in same order as in setup()
//     uint8_t speed = leds.effectData.read<uint8_t>();
//     uint8_t intensity = leds.effectData.read<uint8_t>();

//     //binding of loop persistent values (pointers) tbd: aux0,1,step etc can be renamed to meaningful names
//     bool *isSecond = leds.effectData.readWrite<bool>();
//     uint16_t *bri_lower = leds.effectData.readWrite<uint16_t>();
//     unsigned long *step = leds.effectData.readWrite<unsigned long>();

//     uint8_t bpm = 40 + (speed);
//     uint32_t msPerBeat = (60000L / bpm);
//     uint32_t secondBeat = (msPerBeat / 3);
//     unsigned long beatTimer = sys->now - *step;

//     *bri_lower = *bri_lower * 2042 / (2048 + intensity);

//     if ((beatTimer > secondBeat) && !*isSecond) { // time for the second beat?
//       *bri_lower = UINT16_MAX; //3/4 bri
//       *isSecond = true;
//     }

//     if (beatTimer > msPerBeat) { // time to reset the beat timer?
//       *bri_lower = UINT16_MAX; //full bri
//       *isSecond = false;
//       *step = sys->now;
//     }

//     for (int i = 0; i < leds.size.x; i++) {
//       leds.setPixelColor(i, ColorFromPalette(leds.palette, ::map(i, 0, leds.size.x, 0, 255), 255 - (*bri_lower >> 8)));
//     }
//   }
// }; // HeartBeatEffect

// class AudioRingsEffect: public RingEffect {
//   const char * name() override {return "AudioRings";}
//   uint8_t dim() override {return _1D;}
//   const char * tags() override {return "♫💫";}

//   void setup() override {
//     Effect::setup(leds, parentVar);
//     ui->initCheckBox(parentVar, "inWards", leds.effectData.write<bool>(true));
//     ui->initSlider(parentVar, "rings", leds.effectData.write<uint8_t>(7), 1, 50);
//   }

//   void loop() override {
//     //Binding of controls. Keep before binding of vars and keep in same order as in setup()
//     bool inWards = leds.effectData.read<bool>();
//     uint8_t nrOfRings = leds.effectData.read<uint8_t>();

//     for (int i = 0; i < nrOfRings; i++) {

//       uint8_t band = ::map(i, 0, nrOfRings-1, 0, NUM_GEQ_CHANNELS-1);

//       byte val;
//       if (inWards) {
//         val = sharedData.bands[band];
//       }
//       else {
//         val = sharedData.bands[NUM_GEQ_CHANNELS-1 - band];
//       }

//       // Visualize leds to the beat
//       CRGB color = ColorFromPalette(leds.palette, val, val);
// //      CRGB color = ColorFromPalette(currentPalette, val, 255, currentBlending);
// //      color.nscale8_video(val);
//       setRing(leds, i, color);
// //        setRingFromFtt((i * 2), i);
//     }

//     setRingFromFtt(leds, 2, 7); // set outer ring to bass
//     setRingFromFtt(leds, 0, 8); // set outer ring to bass

//   }
//   void setRingFromFtt(int index, int ring) {
//     byte val = sharedData.bands[index];
//     // Visualize leds to the beat
//     CRGB color = ColorFromPalette(leds.palette, val);
//     color.nscale8_video(val);
//     setRing(leds, ring, color);
//   }
// };

// class DJLightEffect: public Node {
//   const char * name() override {return "DJLight";}
//   uint8_t dim() override {return _1D;}
//   const char * tags() override {return "♫🐙";}

//   void setup() override {
//     leds.fill_solid(CRGB::Black);
//     ui->initSlider(parentVar, "speed", leds.effectData.write<uint8_t>(255));
//     ui->initCheckBox(parentVar, "candyFactory", leds.effectData.write<bool>(true));
//     ui->initSlider(parentVar, "fade", leds.effectData.write<uint8_t>(4), 0, 10);
//   }

//   void loop() override {
//     //Binding of controls. Keep before binding of vars and keep in same order as in setup()
//     uint8_t speed = leds.effectData.read<uint8_t>();
//     bool candyFactory = leds.effectData.read<bool>();
//     uint8_t fade = leds.effectData.read<uint8_t>();

//     //binding of loop persistent values (pointers) tbd: aux0,1,step etc can be renamed to meaningful names
//     uint8_t *aux0 = leds.effectData.readWrite<uint8_t>();

//     const int mid = leds.size.x / 2;

//     uint8_t secondHand = (speed < 255) ? (micros()/(256-speed)/500 % 16) : 0;
//     if((speed > 254) || (*aux0 != secondHand)) {   // WLEDMM allow run run at full speed
//       *aux0 = secondHand;

//       CRGB color = CRGB(0,0,0);
//       // color = CRGB(sharedData.bands[NUM_GEQ_CHANNELS-1]/2, sharedData.bands[5]/2, sharedData.bands[0]/2);   // formula from 0.13.x (10Khz): R = 3880-5120, G=240-340, B=60-100
//       if (!candyFactory) {
//         color = CRGB(sharedData.bands[12]/2, sharedData.bands[3]/2, sharedData.bands[1]/2);    // formula for 0.14.x  (22Khz): R = 3015-3704, G=216-301, B=86-129
//       } else {
//         // candy factory: an attempt to get more colors
//         color = CRGB(sharedData.bands[11]/2 + sharedData.bands[12]/4 + sharedData.bands[14]/4, // red  : 2412-3704 + 4479-7106
//                     sharedData.bands[4]/2 + sharedData.bands[3]/4,                     // green: 216-430
//                     sharedData.bands[0]/4 + sharedData.bands[1]/4 + sharedData.bands[2]/4);   // blue:  46-216
//         if ((color.getLuma() < 96) && (volumeSmth >= 1.5f)) {             // enhance "almost dark" pixels with yellow, based on not-yet-used channels
//           unsigned yello_g = (sharedData.bands[5] + sharedData.bands[6] + sharedData.bands[7]) / 3;
//           unsigned yello_r = (sharedData.bands[7] + sharedData.bands[8] + sharedData.bands[9] + sharedData.bands[10]) / 4;
//           color.green += (uint8_t) yello_g / 2;
//           color.red += (uint8_t) yello_r / 2;
//         }
//       }

//       if (volumeSmth < 1.0f) color = CRGB(0,0,0); // silence = black

//       // make colors less "pastel", by turning up color saturation in HSV space
//       if (color.getLuma() > 32) {                                      // don't change "dark" pixels
//         CHSV hsvColor = rgb2hsv_approximate(color);
//         hsvColor.v = constrain(hsvColor.v, 48, 204);  // 48 < brightness < 204
//         if (candyFactory)
//           hsvColor.s = max(hsvColor.s, (uint8_t)204);                  // candy factory mode: strongly turn up color saturation (> 192)
//         else
//           hsvColor.s = max(hsvColor.s, (uint8_t)108);                  // normal mode: turn up color saturation to avoid pastels
//         color = hsvColor;
//       }
//       //if (color.getLuma() > 12) color.maximizeBrightness();          // for testing

//       //leds.setPixelColor(mid, color.fadeToBlackBy(map(sharedData.bands[4], 0, 255, 255, 4)));     // 0.13.x  fade -> 180hz-260hz
//       uint8_t fadeVal = ::map(sharedData.bands[3], 0, 255, 255, 4);                                      // 0.14.x  fade -> 216hz-301hz
//       if (candyFactory) fadeVal = constrain(fadeVal, 0, 176);  // "candy factory" mode - avoid complete fade-out
//       leds.setPixelColor(mid, color.fadeToBlackBy(fadeVal));

//       for (int i = leds.size.x - 1; i > mid; i--)   leds.setPixelColor(i, leds.getPixelColor(i-1)); // move to the left
//       for (int i = 0; i < mid; i++)            leds.setPixelColor(i, leds.getPixelColor(i+1)); // move to the right

//       leds.fadeToBlackBy(fade);

//     }
//   }
// }; //DJLight

// /*
//  * Spotlights moving back and forth that cast dancing shadows.
//  * Shine this through tree branches/leaves or other close-up objects that cast
//  * interesting shadows onto a ceiling or tarp.
//  *
//  * By Steve Pomeroy @xxv
//  */
// uint16_t mode_dancing_shadows(void)
// {
//   if (SEGLEN == 1) return mode_static();
//   uint8_t numSpotlights = ::map(SEGMENT.intensity, 0, 255, 2, SPOT_MAX_COUNT);  // 49 on 32 segment ESP32, 17 on 16 segment ESP8266
//   bool initialize = aux0 != numSpotlights;
//   aux0 = numSpotlights;

//   uint16_t dataSize = sizeof(spotlight) * numSpotlights;
//   if (!SEGENV.allocateData(dataSize)) return mode_static(); //allocation failed
//   Spotlight* spotlights = reinterpret_cast<Spotlight*>(SEGENV.data);
//   if (call == 0) SEGENV.setUpLeds();   // WLEDMM use lossless getRGB()

//   SEGMENT.fill(BLACK);

//   unsigned long time = strip.now;
//   bool respawn = false;

//   for (size_t i = 0; i < numSpotlights; i++) {
//     if (!initialize) {
//       // advance the position of the spotlight
//       int16_t delta = (float)(time - spotlights[i].lastUpdateTime) *
//                   (spotlights[i].speed * ((1.0f + speed)/100.0f));

//       if (abs(delta) >= 1) {
//         spotlights[i].position += delta;
//         spotlights[i].lastUpdateTime = time;
//       }

//       respawn = (spotlights[i].speed > 0.0 && spotlights[i].position > (SEGLEN + 2))
//              || (spotlights[i].speed < 0.0 && spotlights[i].position < -(spotlights[i].width + 2));
//     }

//     if (initialize || respawn) {
//       spotlights[i].colorIdx = random8();
//       spotlights[i].width = random8(1, 10);

//       spotlights[i].speed = 1.0f/random8(4, 50);

//       if (initialize) {
//         spotlights[i].position = random16(SEGLEN);
//         spotlights[i].speed *= random8(2) > 0 ? 1.0 : -1.0;
//       } else {
//         if (random8(2)) {
//           spotlights[i].position = SEGLEN + spotlights[i].width;
//           spotlights[i].speed *= -1.0f;
//         }else {
//           spotlights[i].position = -spotlights[i].width;
//         }
//       }

//       spotlights[i].lastUpdateTime = time;
//       spotlights[i].type = random8(SPOT_TYPES_COUNT);
//     }

//     uint32_t color = SEGMENT. ColorFromPalette(layer->layerP->palette, spotlights[i].colorIdx);
//     int start = spotlights[i].position;

//     if (spotlights[i].width <= 1) {
//       if (start >= 0 && start < SEGLEN) {
//         SEGMENT.blendColor(start, color, 128);
//       }
//     } else {
//       switch (spotlights[i].type) {
//         case SPOT_TYPE_SOLID:
//           for (size_t j = 0; j < spotlights[i].width; j++) {
//             if ((start + j) >= 0 && (start + j) < SEGLEN) {
//               SEGMENT.blendColor(start + j, color, 128);
//             }
//           }
//         break;

//         case SPOT_TYPE_GRADIENT:
//           for (size_t j = 0; j < spotlights[i].width; j++) {
//             if ((start + j) >= 0 && (start + j) < SEGLEN) {
//               SEGMENT.blendColor(start + j, color, cubicwave8(map(j, 0, spotlights[i].width - 1, 0, 255)));
//             }
//           }
//         break;

//         case SPOT_TYPE_2X_GRADIENT:
//           for (size_t j = 0; j < spotlights[i].width; j++) {
//             if ((start + j) >= 0 && (start + j) < SEGLEN) {
//               SEGMENT.blendColor(start + j, color, cubicwave8(2 * ::map(j, 0, spotlights[i].width - 1, 0, 255)));
//             }
//           }
//         break;

//         case SPOT_TYPE_2X_DOT:
//           for (size_t j = 0; j < spotlights[i].width; j += 2) {
//             if ((start + j) >= 0 && (start + j) < SEGLEN) {
//               SEGMENT.blendColor(start + j, color, 128);
//             }
//           }
//         break;

//         case SPOT_TYPE_3X_DOT:
//           for (size_t j = 0; j < spotlights[i].width; j += 3) {
//             if ((start + j) >= 0 && (start + j) < SEGLEN) {
//               SEGMENT.blendColor(start + j, color, 128);
//             }
//           }
//         break;

//         case SPOT_TYPE_4X_DOT:
//           for (size_t j = 0; j < spotlights[i].width; j += 4) {
//             if ((start + j) >= 0 && (start + j) < SEGLEN) {
//               SEGMENT.blendColor(start + j, color, 128);
//             }
//           }
//         break;
//       }
//     }
//   }

//   return FRAMETIME;
// }
// static const char _data_FX_MODE_DANCING_SHADOWS[] PROGMEM = "Dancing Shadows@!,# of shadows;!;!";

// //  TwinkleFOX by Mark Kriegsman: https://gist.github.com/kriegsman/756ea6dcae8e30845b5a
// //
// //  TwinkleFOX: Twinkling 'holiday' lights that fade in and out.
// //  Colors are chosen from a palette. Read more about this effect using the link above!

// // If COOL_LIKE_INCANDESCENT is set to 1, colors will
// // fade out slighted 'reddened', similar to how
// // incandescent bulbs change color as they get dim down.
// #define COOL_LIKE_INCANDESCENT 1

// CRGB twinklefox_one_twinkle(uint32_t ms, uint8_t salt, bool cat)
// {
//   // Overall twinkle speed (changed)
//   uint16_t ticks = ms / aux0;
//   uint8_t fastcycle8 = ticks;
//   uint16_t slowcycle16 = (ticks >> 8) + salt;
//   slowcycle16 += sin8_t(slowcycle16);
//   slowcycle16 = (slowcycle16 * 2053) + 1384;
//   uint8_t slowcycle8 = (slowcycle16 & 0xFF) + (slowcycle16 >> 8);

//   // Overall twinkle density.
//   // 0 (NONE lit) to 8 (ALL lit at once).
//   // Default is 5.
//   uint8_t twinkleDensity = (SEGMENT.intensity >> 5) +1;

//   uint8_t bright = 0;
//   if (((slowcycle8 & 0x0E)/2) < twinkleDensity) {
//     uint8_t ph = fastcycle8;
//     // This is like 'triwave8', which produces a
//     // symmetrical up-and-down triangle sawtooth waveform, except that this
//     // function produces a triangle wave with a faster attack and a slower decay
//     if (cat) //twinklecat, variant where the leds instantly turn on
//     {
//       bright = 255 - ph;
//     } else { //vanilla twinklefox
//       if (ph < 86) {
//       bright = ph * 3;
//       } else {
//         ph -= 86;
//         bright = 255 - (ph + (ph/2));
//       }
//     }
//   }

//   uint8_t hue = slowcycle8 - salt;
//   CRGB c;
//   if (bright > 0) {
//     c = ColorFromPalette(SEGPALETTE, hue, bright, NOBLEND);
//     if(COOL_LIKE_INCANDESCENT == 1) {
//       // This code takes a pixel, and if its in the 'fading down'
//       // part of the cycle, it adjusts the color a little bit like the
//       // way that incandescent bulbs fade toward 'red' as they dim.
//       if (fastcycle8 >= 128)
//       {
//         uint8_t cooling = (fastcycle8 - 128) >> 4;
//         c.g = qsub8(c.g, cooling);
//         c.b = qsub8(c.b, cooling * 2);
//       }
//     }
//   } else {
//     c = CRGB::Black;
//   }
//   return c;
// }

// //  This function loops over each pixel, calculates the
// //  adjusted 'clock' that this pixel should use, and calls
// //  "CalculateOneTwinkle" on each pixel.  It then displays
// //  either the twinkle color of the background color,
// //  whichever is brighter.
// uint16_t twinklefox_base(bool cat)
// {
//   // "PRNG16" is the pseudorandom number generator
//   // It MUST be reset to the same starting value each time
//   // this function is called, so that the sequence of 'random'
//   // numbers that it generates is (paradoxically) stable.
//   uint16_t PRNG16 = 11337;

//   // Calculate speed
//   if (speed > 100) aux0 = 3 + ((255 - speed) >> 3);
//   else aux0 = 22 + ((100 - speed) >> 1);

//   // Set up the background color, "bg".
//   CRGB bg = CRGB(SEGCOLOR(1));
//   uint8_t bglight = bg.getAverageLight();
//   if (bglight > 64) {
//     bg.nscale8_video(16); // very bright, so scale to 1/16th
//   } else if (bglight > 16) {
//     bg.nscale8_video(64); // not that bright, so scale to 1/4th
//   } else {
//     bg.nscale8_video(86); // dim, scale to 1/3rd.
//   }

//   uint8_t backgroundBrightness = bg.getAverageLight();

//   for (int i = 0; i < SEGLEN; i++) {

//     PRNG16 = (uint16_t)(PRNG16 * 2053) + 1384; // next 'random' number
//     uint16_t myclockoffset16= PRNG16; // use that number as clock offset
//     PRNG16 = (uint16_t)(PRNG16 * 2053) + 1384; // next 'random' number
//     // use that number as clock speed adjustment factor (in 8ths, from 8/8ths to 23/8ths)
//     uint8_t myspeedmultiplierQ5_3 =  ((((PRNG16 & 0xFF)>>4) + (PRNG16 & 0x0F)) & 0x0F) + 0x08;
//     uint32_t myclock30 = (uint32_t)((strip.now * myspeedmultiplierQ5_3) >> 3) + myclockoffset16;
//     uint8_t  myunique8 = PRNG16 >> 8; // get 'salt' value for this pixel

//     // We now have the adjusted 'clock' for this pixel, now we call
//     // the function that computes what color the pixel should be based
//     // on the "brightness = f( time )" idea.
//     CRGB c = twinklefox_one_twinkle(myclock30, myunique8, cat);

//     uint8_t cbright = c.getAverageLight();
//     int16_t deltabright = cbright - backgroundBrightness;
//     if (deltabright >= 32 || (!bg)) {
//       // If the new pixel is significantly brighter than the background color,
//       // use the new color.
//       layer->setRGB(i, c.red, c.green, c.blue);
//     } else if (deltabright > 0) {
//       // If the new pixel is just slightly brighter than the background color,
//       // mix a blend of the new color and the background color
//       layer->setRGB(i, color_blend(RGBW32(bg.r,bg.g,bg.b,0), RGBW32(c.r,c.g,c.b,0), deltabright * 8));
//     } else {
//       // if the new pixel is not at all brighter than the background color,
//       // just use the background color.
//       layer->setRGB(i, bg.r, bg.g, bg.b);
//     }
//   }
//   return FRAMETIME;
// }

// uint16_t mode_twinklefox()
// {
//   return twinklefox_base(false);
// }
// static const char _data_FX_MODE_TWINKLEFOX[] PROGMEM = "Twinklefox@!,Twinkle rate;!,!;!";

// uint16_t mode_twinklecat()
// {
//   return twinklefox_base(true);
// }
// static const char _data_FX_MODE_TWINKLECAT[] PROGMEM = "Twinklecat@!,Twinkle rate;!,!;!";

#endif