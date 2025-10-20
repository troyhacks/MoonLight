/**
    @title     MoonLight
    @file      Effects.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/moonlight/overview/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact us for more information.
**/


/**
 * Guidelines:
 * 
 * 1) Don't use String type class variables but char[x] as can crash when node is destructed (node in PSRAM, string in heap)
 * 2) no static variables in node classes as we can run multiple instances of the same node which should not share data -> class variables.
 */

#if FT_MOONLIGHT

class SolidEffect: public Node {
  public:

  static const char * name() {return "Solid 🔥💡";}
  static uint8_t dim() {return _1D;}
  static const char * tags() {return "";}

  uint8_t red = 182;
  uint8_t green = 15;
  uint8_t blue = 98;
  uint8_t white = 0;
  uint8_t brightness = 255;

  void setup() override {
    addControl(red, "red", "range");
    addControl(green, "green", "range");
    addControl(blue, "blue", "range");
    addControl(white, "white", "range");
    addControl(brightness, "brightness", "range");
  }

  void loop() override {
      layerV->fill_solid(CRGB(red * brightness/255, green * brightness/255, blue * brightness/255));
      if (layerV->layerP->lights.header.offsetWhite != UINT8_MAX && white > 0)
        for (int index; index < layerV->nrOfLights; index++) layerV->setWhite(index, white * brightness/255);
  }
};

//by limpkin
class FixedRectangleEffect: public Node {
  public:

  static const char * name() {return "Fixed Rectangle 🔥";}
  static uint8_t dim() {return _2D;}
  static const char * tags() {return "";}

  uint8_t red = 182;
  uint8_t green = 15;
  uint8_t blue = 98;
  uint8_t white = 0;
  uint8_t width = 1;
  uint8_t x = 0;
  uint8_t height = 1;
  uint8_t y = 0;
  uint8_t depth = 1;
  uint8_t z = 0;
  bool alternateWhite = false; // to be used for frontlight

  void setup() override {
    addControl(red, "red", "range");
    addControl(green, "green", "range");
    addControl(blue, "blue", "range");
    addControl(white, "white", "range");
    addControl(x, "X position", "range", 0);
    addControl(y, "Y position", "range", 0);
    addControl(z, "Z position", "range", 0);
    addControl(width, "Rectangle width", "range", 1);
    addControl(height, "Rectangle height", "range", 1);
    addControl(depth, "Rectangle depth", "range", 1);
    addControl(alternateWhite, "alternateWhite", "checkbox");
  }

  bool alternate = true; 

  void loop() override {
    alternate = false;
    layerV->fadeToBlackBy(10); //cleanup old leds if changing
    Coord3D pos = {0,0,0};
    for (pos.z = z; pos.z < MIN(z+depth, layerV->size.z); pos.z++) {
      for (pos.y = y; pos.y < MIN(y+height, layerV->size.y); pos.y++) {
        for (pos.x = x; pos.x < MIN(x+width, layerV->size.x); pos.x++) {
          if (alternateWhite && alternate)
            layerV->setRGB(pos, CRGB::White);
          else
            layerV->setRGB(pos, CRGB(red, green, blue));
          if (white > 0)
            layerV->setWhite(pos, white);
          if (height < width)
            alternate = !alternate;
        }
        if (height > width)
          alternate = !alternate;
      }
    }
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

  static const char * name() {return "Bouncing Balls 🔥🎨💡";}
  static uint8_t dim() {return _1D;}
  static const char * tags() {return "";}

  uint8_t grav = 128;
  uint8_t numBalls = 8;

  void setup() override {
    addControl(grav, "grav", "range");
    addControl(numBalls, "numBalls", "range", 1, maxNumBalls);
  }

  //binding of loop persistent values (pointers)
  Ball (*balls)[maxNumBalls] = nullptr ;//[maxColumns][maxNumBalls];

  ~BouncingBallsEffect() {
    freeMB(balls);

    Node::~Node();
  }

  void onSizeChanged(Coord3D prevSize) override {
    freeMB(balls);
    balls = allocMB<Ball[maxNumBalls]>(layerV->size.y);

    if (!balls) {
      MB_LOGE(ML_TAG, "allocate balls failed");
    }
  }

  void loop() override {
    if (!balls) return;

    layerV->fadeToBlackBy(100);

    // non-chosen color is a random color
    const float gravity = -9.81f; // standard value of gravity
    // const bool hasCol2 = SEGCOLOR(2);
    const unsigned long time = millis();

    //not necessary as effectControls is cleared at setup()
    // if (call == 0) {
    //   for (size_t i = 0; i < maxNumBalls; i++) balls[i].lastBounceTime = time;
    // }

    for (int y =0; y < layerV->size.y; y++) {
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
        // if (layerV->layerP->palette) {
        //   color = layerV->color_wheel(i*(256/MAX(numBalls, 8)));
        // } 
        // else if (hasCol2) {
        //   color = SEGCOLOR(i % NUM_COLORS);
        // }

        uint8_t pos = roundf(balls[y][i].height * (layerV->size.x - 1));

        CRGB color = ColorFromPalette(layerV->layerP->palette, i*(256/max(numBalls, (uint8_t)8))); //error: no matching function for call to 'max(uint8_t&, int)'

        layerV->setRGB(Coord3D(pos, y), color);
        // if (layerV->size.x<32) layerV->setRGB(indexToVStrip(pos, stripNr), color); // encode virtual strip into index
        // else           layerV->setRGB(balls[i].height + (stripNr+1)*10.0f, color);
      } //balls      layerV->fill_solid(CRGB::White);
    }
  }
};

//DistortionWaves inspired by WLED, ldirko and blazoncek, https://editor.soulmatelights.com/gallery/1089-distorsion-waves
class DistortionWavesEffect: public Node {
public:
  static const char * name() {return "Distortion Waves 🔥💡";}
  static uint8_t dim() {return _2D;}
  static const char * tags() {return "";}

  uint8_t speed = 4;
  uint8_t scale = 4; 

  void setup() override {
    addControl(speed, "speed", "range", 0, 8);
    addControl(scale, "scale", "range", 0, 8);
  }

  void loop() override {
    //Binding of controls. Keep before binding of vars and keep in same order as in setup()

    uint8_t  w = 2;

    uint16_t a  = millis()/32;
    uint16_t a2 = a/2;
    uint16_t a3 = a/3;

    uint16_t cx =  beatsin8(10-speed,0,layerV->size.x-1)*scale;
    uint16_t cy =  beatsin8(12-speed,0,layerV->size.y-1)*scale;
    uint16_t cx1 = beatsin8(13-speed,0,layerV->size.x-1)*scale;
    uint16_t cy1 = beatsin8(15-speed,0,layerV->size.y-1)*scale;
    uint16_t cx2 = beatsin8(17-speed,0,layerV->size.x-1)*scale;
    uint16_t cy2 = beatsin8(14-speed,0,layerV->size.y-1)*scale;
    
    uint16_t xoffs = 0;
    Coord3D pos = {0,0,0};
    for (pos.x = 0; pos.x < layerV->size.x; pos.x++) {
      xoffs += scale;
      uint16_t yoffs = 0;

      for (pos.y = 0; pos.y < layerV->size.y; pos.y++) {
        yoffs += scale;

        uint8_t  rdistort = cos8((cos8(((pos.x<<3)+a )&255)+cos8(((pos.y<<3)-a2)&255)+a3   )&255)>>1; 
        uint8_t  gdistort = cos8((cos8(((pos.x<<3)-a2)&255)+cos8(((pos.y<<3)+a3)&255)+a+32 )&255)>>1; 
        uint8_t  bdistort = cos8((cos8(((pos.x<<3)+a3)&255)+cos8(((pos.y<<3)-a) &255)+a2+64)&255)>>1; 

        uint8_t  valueR = rdistort+ w*  (a- ( ((xoffs - cx)  * (xoffs - cx)  + (yoffs - cy)  * (yoffs - cy))>>7  ));
        uint8_t  valueG = gdistort+ w*  (a2-( ((xoffs - cx1) * (xoffs - cx1) + (yoffs - cy1) * (yoffs - cy1))>>7 ));
        uint8_t  valueB = bdistort+ w*  (a3-( ((xoffs - cx2) * (xoffs - cx2) + (yoffs - cy2) * (yoffs - cy2))>>7 ));

        valueR = layerV->layerP->gamma8(cos8(valueR));
        valueG = layerV->layerP->gamma8(cos8(valueG));
        valueB = layerV->layerP->gamma8(cos8(valueB));

        layerV->setRGB(pos, CRGB(valueR, valueG, valueB));
      }
    }
  }
}; // DistortionWaves

#define MAX_FREQUENCY   11025          // sample frequency / 2 (as per Nyquist criterion)

class FreqMatrixEffect: public Node {
public:

  static const char * name() {return "Freq Matrix 🔥♪💡";}
  static uint8_t dim() {return _1D;}
  static const char * tags() {return "";}

  uint8_t speed = 255;
  uint8_t fx = 128;
  uint8_t lowBin = 0;//18;
  uint8_t highBin = 0;//48;
  uint8_t sensitivity10 = 30;
  bool audioSpeed = false; //💫

  void setup() override {
    addControl(speed, "speed", "range");
    addControl(fx, "soundFX", "range");
    addControl(lowBin, "lowBin", "range");
    addControl(highBin, "highBin", "range");
    addControl(sensitivity10, "sensitivity", "range", 10, 100);
    addControl(audioSpeed, "audioSpeed", "checkbox");
  }

  uint8_t aux0;
  unsigned long bassMemory = 0;

  void loop() override {
    
    uint8_t secondHand;
    if (audioSpeed) {
      if (sharedData.bands[0] > 100) {
        if (bassMemory < 5000) bassMemory++; 
      }
      else 
        if (bassMemory) bassMemory--; //5000 frames cooldown

      secondHand = (micros()/(256-speed* bassMemory / 5000)/500 % 16);
    } else
      secondHand = (speed < 255) ? (micros()/(256-speed)/500 % 16) : 0;

    if ((speed > 254) || (aux0 != secondHand)) {   // WLEDMM allow run run at full speed
      aux0 = secondHand;

      // Pixel brightness (value) based on volume * sensitivity * intensity
      // uint_fast8_t sensitivity10 = ::map(sensitivity, 0, 31, 10, 100); // reduced resolution slider // WLEDMM sensitivity * 10, to avoid losing precision
      int pixVal = sharedData.volume * (float)fx * (float)sensitivity10 / 2560.0f; // WLEDMM 2560 due to sensitivity * 10
      if (pixVal > 255) pixVal = 255;  // make a brightness from the last avg

      CRGB color = CRGB::Black;

      if (sharedData.majorPeak > MAX_FREQUENCY) sharedData.majorPeak = 1;
      // MajorPeak holds the freq. value which is most abundant in the last sample.
      // With our sampling rate of 10240Hz we have a usable freq range from roughtly 80Hz to 10240/2 Hz
      // we will treat everything with less than 65Hz as 0

      if ((sharedData.majorPeak > 80.0f) && (sharedData.volume > 0.25f)) { // WLEDMM
        // Pixel color (hue) based on major frequency
        int upperLimit = 80 + 42 * highBin;
        int lowerLimit = 80 + 3 * lowBin;
        //uint8_t i =  lowerLimit!=upperLimit ? ::map(sharedData.majorPeak, lowerLimit, upperLimit, 0, 255) : sharedData.majorPeak;  // (original formula) may under/overflow - so we enforce uint8_t
        int freqMapped =  lowerLimit!=upperLimit ? ::map(sharedData.majorPeak, lowerLimit, upperLimit, 0, 255) : sharedData.majorPeak;  // WLEDMM preserve overflows
        uint8_t i = abs(freqMapped) & 0xFF;  // WLEDMM we embrace overflow ;-) by "modulo 256"

        color = CHSV(i, 240, (uint8_t)pixVal); // implicit conversion to RGB supplied by FastLED
      }

      // shift the pixels one pixel up
      layerV->setRGB(0, color);
      for (int x = layerV->size.x - 1; x >= 0; x--) { //int as we count down!!!
        if (x!=0) color = layerV->getRGB(x-1);
        for (int y = 0; y < layerV->size.y; y++)
          for (int z = 0; z < layerV->size.z; z++)
            layerV->setRGB(Coord3D(x,y,z), color);
      }
    }
  }
};

class GEQEffect: public Node {
public:

  static const char * name() {return "GEQ 🔥🎨♫💡";}
  static uint8_t dim() {return _1D;}
  static const char * tags() {return "";}

  uint8_t fadeOut = 255;
  uint8_t ripple = 128;
  bool colorBars = false;
  bool smoothBars = true;

  unsigned long step;

  void setup() override {
    Node::setup();

    addControl(fadeOut, "fadeOut", "range");
    addControl(ripple, "ripple", "range");
    addControl(colorBars, "colorBars", "checkbox");
    addControl(smoothBars, "smoothBars", "checkbox");

    step = millis();
  }

  uint16_t *previousBarHeight = nullptr; //array

  ~GEQEffect() {
    freeMB(previousBarHeight);

    Node::~Node();
  }

  void onSizeChanged(Coord3D prevSize) override {
      freeMB(previousBarHeight);
      previousBarHeight = allocMB<uint16_t>(layerV->size.x);
      if (!previousBarHeight) {
        MB_LOGE(ML_TAG, "allocate previousBarHeight failed");
      }
  }

  void loop() override {

    const int NUM_BANDS = NUM_GEQ_CHANNELS ; // ::map(layerV->custom1, 0, 255, 1, 16);

    #ifdef SR_DEBUG
    uint8_t samplePeak = *(uint8_t*)um_data->u_data[3];
    #endif

    bool rippleTime = false;
    if (millis() - step >= (256U - ripple)) {
      step = millis();
      rippleTime = true;
    }

    int fadeoutDelay = (256 - fadeOut) / 64; //256..1 -> 4..0
    size_t beat = ::map(beat16( fadeOut), 0, UINT16_MAX, 0, fadeoutDelay-1 ); // instead of call%fadeOutDelay

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

      // MB_LOGD(ML_TAG, "x %d b %d n %d w %f %f\n", x, band, NUM_BANDS, bandwidth, remaining);
      uint8_t frBand = ((NUM_BANDS < NUM_GEQ_CHANNELS) && (NUM_BANDS > 1)) ? ::map(band, 0, NUM_BANDS, 0, NUM_GEQ_CHANNELS):band; // always use full range. comment out this line to get the previous behaviour.
      // frBand = constrain(frBand, 0, NUM_GEQ_CHANNELS-1); //WLEDMM can never be out of bounds (I think...)
      uint16_t colorIndex = frBand * 17; //WLEDMM 0.255
      uint16_t bandHeight = sharedData.bands[frBand];  // WLEDMM we use the original ffResult, to preserve accuracy

      // WLEDMM begin - smooth out bars
      if ((pos.x > 0) && (pos.x < (layerV->size.x-1)) && (smoothBars)) {
        // get height of next (right side) bar
        uint8_t nextband = (remaining < 1)? band +1: band;
        nextband = constrain(nextband, 0, NUM_GEQ_CHANNELS-1);  // just to be sure
        frBand = ((NUM_BANDS < NUM_GEQ_CHANNELS) && (NUM_BANDS > 1)) ? ::map(nextband, 0, NUM_BANDS, 0, NUM_GEQ_CHANNELS):nextband; // always use full range. comment out this line to get the previous behaviour.
        uint16_t nextBandHeight = sharedData.bands[frBand];
        // smooth Band height
        bandHeight = (7*bandHeight + 3*lastBandHeight + 3*nextBandHeight) / 12;   // yeees, its 12 not 13 (10% amplification)
        bandHeight = constrain(bandHeight, 0, 255);   // remove potential over/underflows
        colorIndex = ::map(pos.x, 0, layerV->size.x, 0, 256); //WLEDMM
      }
      lastBandHeight = bandHeight; // remember BandHeight (left side) for next iteration
      uint16_t barHeight = ::map(bandHeight, 0, 255, 0, layerV->size.y); // Now we map bandHeight to barHeight. do not subtract -1 from layerV->size.y here (should be 256 (ewowiHack) but then the top row is not displayed...)
      // WLEDMM end

      if (barHeight > layerV->size.y) barHeight = layerV->size.y;                      // WLEDMM ::map() can "overshoot" due to rounding errors

      CRGB ledColor = CRGB::Black;

      for (pos.y=0; pos.y < barHeight; pos.y++) {
        if (colorBars) //color_vertical / color bars toggle
          colorIndex = ::map(pos.y, 0, layerV->size.y, 0, 256);

        ledColor = ColorFromPalette(layerV->layerP->palette, (uint8_t)colorIndex);

        layerV->setRGB(Coord3D(pos.x, layerV->size.y - 1 - pos.y), ledColor);
      }

      if (previousBarHeight) {
        if (barHeight > previousBarHeight[pos.x]) previousBarHeight[pos.x] = barHeight; //drive the peak up
        if ((ripple > 0) && (previousBarHeight[pos.x] > 0) && (previousBarHeight[pos.x] < layerV->size.y))  // WLEDMM avoid "overshooting" into other segments
          layerV->setRGB(Coord3D(pos.x, layerV->size.y - previousBarHeight[pos.x]), (CRGB)CHSV( millis()/50, 255, 255)); // take millis()/50 color for the time being

        if (rippleTime && previousBarHeight[pos.x]>0) previousBarHeight[pos.x]--;    //delay/ripple effect
      }
    }
  }
};

// Author: @TroyHacks
// @license GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
class GEQ3DEffect: public Node {
  public:
  static const char * name() {return "GEQ 3D 🔥♫🐺";}
  static uint8_t dim() {return _2D;}
  static const char * tags() {return "";}

  uint8_t speed = 2;
  uint8_t frontFill = 228;
  uint8_t horizon = 0;
  uint8_t depth = 176;
  uint8_t numBands = 16;
  bool borders = true;
  bool softHack = true;

  void setup() override {
    layerV->fadeToBlackBy(16);

    addControl(speed, "speed", "range", 1, 10);
    addControl(frontFill, "frontFill", "range");
    addControl(horizon, "horizon", "range", 0, layerV->size.x-1); //layerV->size.x-1 is not always set here
    addControl(depth, "depth", "range");
    addControl(numBands, "numBands", "range", 2, 16);
    addControl(borders, "borders", "checkbox");
    addControl(softHack, "softHack", "checkbox");

    // "GEQ 3D ☾@Speed,Front Fill,Horizon,Depth,Num Bands,Borders,Soft,;!,,Peaks;!;2f;sx=255,ix=228,c1=255,c2=255,c3=15,pal=11";
  }

  uint16_t projector;
  int8_t projector_dir;
  uint32_t counter;

  void loop() override {
    //Binding of controls. Keep before binding of vars and keep in same order as in setup()

    if (numBands == 0) return; //init Effect

    const int cols = layerV->size.x;
    const int rows = layerV->size.y;

    if (counter++ % (11-speed) == 0) projector += projector_dir;
    if (projector >= cols) projector_dir = -1;
    if (projector <= 0) projector_dir = 1;

    layerV->fill_solid(CRGB::Black);

    CRGB ledColorTemp;
    const int NUM_BANDS = numBands;
    uint_fast8_t split  = ::map(projector,0,cols,0,(NUM_BANDS - 1));

    uint8_t heights[NUM_GEQ_CHANNELS] = { 0 };
    const uint8_t maxHeight = roundf(float(rows) * ((rows<18) ? 0.75f : 0.85f));           // slightly reduce bar height on small panels 
    for (int i=0; i<NUM_BANDS; i++) {
      unsigned band = i;
      if (NUM_BANDS < NUM_GEQ_CHANNELS) band = ::map(band, 0, NUM_BANDS, 0, NUM_GEQ_CHANNELS); // always use full range.
      heights[i] = map8(sharedData.bands[band],0,maxHeight); // cache sharedData.bands[] as data might be updated in parallel by the audioreactive core
    }


    for (int i=0; i<=split; i++) { // paint right vertical faces and top - LEFT to RIGHT
      uint16_t colorIndex = ::map(cols/NUM_BANDS*i, 0, cols, 0, 256);
      CRGB ledColor = ColorFromPalette(layerV->layerP->palette, colorIndex);
      int linex = i*(cols/NUM_BANDS);

      if (heights[i] > 1) {
        ledColorTemp = blend(ledColor, CRGB::Black, 255-32);
        int pPos = max(0, linex+(cols/NUM_BANDS)-1);
        for (int y = (i<NUM_BANDS-1) ? heights[i+1] : 0; y <= heights[i]; y++) { // don't bother drawing what we'll hide anyway
          if (rows-y > 0) layerV->drawLine(pPos,rows-y-1,projector,horizon,ledColorTemp,false,depth); // right side perspective
        } 

        ledColorTemp = blend(ledColor, CRGB::Black, 255-128);
        if (heights[i] < rows-horizon && (projector <=linex || projector >= pPos)) { // draw if above horizon AND not directly under projector (special case later)
          if (rows-heights[i] > 1) {  // sanity check - avoid negative Y
            for (uint_fast8_t x=linex; x<=pPos;x++) { 
              bool doSoft = softHack && ((x==linex) || (x==pPos)); // only first and last line need AA
              layerV->drawLine(x,rows-heights[i]-2,projector,horizon,ledColorTemp,doSoft,depth); // top perspective
            }
          }
        }
      }
    }


    for (int i=(NUM_BANDS - 1); i>split; i--) { // paint left vertical faces and top - RIGHT to LEFT
      uint16_t colorIndex = ::map(cols/NUM_BANDS*i, 0, cols-1, 0, 255);
      CRGB ledColor = ColorFromPalette(layerV->layerP->palette, colorIndex);
      int linex = i*(cols/NUM_BANDS);
      int pPos = max(0, linex+(cols/NUM_BANDS)-1);

      if (heights[i] > 1) {
        ledColorTemp = blend(ledColor, CRGB::Black, 255-32);
        for (uint_fast8_t y = (i>0) ? heights[i-1] : 0; y <= heights[i]; y++) { // don't bother drawing what we'll hide anyway
          if (rows-y > 0) layerV->drawLine(linex,rows-y-1,projector,horizon,ledColorTemp,false,depth); // left side perspective
        }

        ledColorTemp = blend(ledColor, CRGB::Black, 255-128);
        if (heights[i] < rows-horizon && (projector <=linex || projector >= pPos)) { // draw if above horizon AND not directly under projector (special case later)
          if (rows-heights[i] > 1) {  // sanity check - avoid negative Y
            for (uint_fast8_t x=linex; x<=pPos;x++) {
              bool doSoft = softHack && ((x==linex) || (x==pPos)); // only first and last line need AA
              layerV->drawLine(x,rows-heights[i]-2,projector,horizon,ledColorTemp,doSoft,depth); // top perspective
            }
          }
        }
      }
    }


    for (int i=0; i<NUM_BANDS; i++) {
      uint16_t colorIndex = ::map(cols/NUM_BANDS*i, 0, cols-1, 0, 255);
      CRGB ledColor = ColorFromPalette(layerV->layerP->palette, colorIndex);
      int linex = i*(cols/NUM_BANDS);
      int pPos  = linex+(cols/NUM_BANDS)-1;
      int pPos1 = linex+(cols/NUM_BANDS);

      if (projector >=linex && projector <= pPos) { // special case when top perspective is directly under the projector
        if ((heights[i] > 1) && (heights[i] < rows-horizon) && (rows-heights[i] > 1)) {
          ledColorTemp = blend(ledColor, CRGB::Black, 255-128);
          for (uint_fast8_t x=linex; x<=pPos;x++) {
            bool doSoft = softHack && ((x==linex) || (x==pPos)); // only first and last line need AA
            layerV->drawLine(x,rows-heights[i]-2,projector,horizon,ledColorTemp,doSoft,depth); // top perspective
          }
        }
      }

      if ((heights[i] > 1) && (rows-heights[i] > 0)) {
        ledColorTemp = blend(ledColor, CRGB::Black, 255-frontFill);
        for (uint_fast8_t x=linex; x<pPos1;x++) { 
          layerV->drawLine(x,rows-1,x,rows-heights[i]-1,ledColorTemp); // front fill
        }

        if (!borders && heights[i] > rows-horizon) {
          if (frontFill == 0) ledColorTemp = blend(ledColor, CRGB::Black, 255-32); // match side fill if we're in blackout mode
          layerV->drawLine(linex,rows-heights[i]-1,linex+(cols/NUM_BANDS)-1,rows-heights[i]-1,ledColorTemp); // top line to simulate hidden top fill
        }

        if ((borders) && (rows-heights[i] > 1)) {
          layerV->drawLine(linex,                   rows-1,linex,rows-heights[i]-1,ledColor); // left side line
          layerV->drawLine(linex+(cols/NUM_BANDS)-1,rows-1,linex+(cols/NUM_BANDS)-1,rows-heights[i]-1,ledColor); // right side line
          layerV->drawLine(linex,                   rows-heights[i]-2,linex+(cols/NUM_BANDS)-1,rows-heights[i]-2,ledColor); // top line
          layerV->drawLine(linex,                   rows-1,linex+(cols/NUM_BANDS)-1,rows-1,ledColor); // bottom line
        }
      }
    }
  }

}; //GEQ3DEffect

class LinesEffect: public Node {
public:

  static const char * name() {return "Lines 🔥";}
  static uint8_t dim() {return _1D;}
  static const char * tags() {return "";}

  uint8_t bpm = 120;

  void setup() override {
    addControl(bpm, "bpm", "range");
  }

  void loop() override {
    int   frameNr;

    layerV->fadeToBlackBy(255);

    Coord3D pos = {0,0,0};
    pos.x = ::map(beat16( bpm), 0, UINT16_MAX, 0, layerV->size.x ); //instead of call%width

    for (pos.y = 0; pos.y < layerV->size.y; pos.y++) {
      int colorNr = (frameNr / layerV->size.y) % 3;
      layerV->setRGB(pos, colorNr == 0?CRGB::Red:colorNr == 1?CRGB::Green:CRGB::Blue);
    }

    pos = {0,0,0};
    pos.y = ::map(beat16( bpm), 0, UINT16_MAX, 0, layerV->size.y ); //instead of call%height
    for (pos.x = 0; pos.x <  layerV->size.x; pos.x++) {
      int colorNr = (frameNr / layerV->size.x) % 3;
      layerV->setRGB(pos, colorNr == 0?CRGB::Red:colorNr == 1?CRGB::Green:CRGB::Blue);
    }
    (frameNr)++;
  }
};

class LissajousEffect: public Node {
public:

  static const char * name() {return "Lissajous 🔥🎨💡";}
  static uint8_t dim() {return _1D;}
  static const char * tags() {return "";}

  uint8_t xFrequency = 64;
  uint8_t fadeRate = 128;
  uint8_t speed = 128;

  void setup() override {
    addControl(xFrequency, "xFrequency", "range");
    addControl(fadeRate, "fadeRate", "range");
    addControl(speed, "speed", "range");
  }

  void loop() override {
    layerV->fadeToBlackBy(fadeRate);
    uint_fast16_t phase = millis() * speed / 256;  // allow user to control rotation speed, speed between 0 and 255!
    Coord3D locn = {0,0,0};
    for (int i=0; i < 256; i ++) {
        //WLEDMM: stick to the original calculations of xlocn and ylocn
        locn.x = sin8(phase/2 + (i*xFrequency)/64);
        locn.y = cos8(phase/2 + i*2);
        locn.x = (layerV->size.x < 2) ? 1 : (::map(2*locn.x, 0,511, 0,2*(layerV->size.x-1)) +1) /2;    // softhack007: "*2 +1" for proper rounding
        locn.y = (layerV->size.y < 2) ? 1 : (::map(2*locn.y, 0,511, 0,2*(layerV->size.y-1)) +1) /2;    // "layerV->size.y > 2" is needed to avoid div/0 in ::map()
        layerV->setRGB(locn, ColorFromPalette(layerV->layerP->palette, millis()/100+i, 255));
    }
  }
};

class Noise2DEffect: public Node {
  public:
  static const char * name() {return "Noise2D 🔥🎨💡";}
  static uint8_t dim() {return _2D;}
  static const char * tags() {return "";}
  
  uint8_t speed = 8;
  uint8_t scale = 64;

  void setup() override {
    addControl(speed, "speed", "range", 0, 15);
    addControl(scale, "scale", "range", 2, 255);
  }

  void loop() override {
    //Binding of controls. Keep before binding of vars and keep in same order as in setup()

    for (int y = 0; y < layerV->size.y; y++) {
      for (int x = 0; x < layerV->size.x; x++) {
        uint8_t pixelHue8 = inoise8(x * scale, y * scale, millis() / (16 - speed));
        layerV->setRGB(Coord3D(x, y), ColorFromPalette(layerV->layerP->palette, pixelHue8));
      }
    }
  }
}; //Noise2D

class NoiseMeterEffect: public Node {
  public:
  static const char * name() {return "Noise Meter ♪💡🎨";}
  static uint8_t dim() {return _1D;}
  static const char * tags() {return "";}
  
  uint8_t fadeRate = 248;
  uint8_t width = 128;

  void setup() override {
    addControl(fadeRate, "fadeRate", "range", 200, 254);
    addControl(width, "width", "range", 0, 255);
  }

  uint8_t aux0;
  uint8_t aux1;

  void loop() override {
    layerV->fadeToBlackBy(fadeRate);

    float tmpSound2 = sharedData.volumeRaw * 2.0 * (float)width / 255.0;
    int maxLen = ::map(tmpSound2, 0, 255, 0, layerV->size.x); // map to pixels availeable in current segment              // Still a bit too sensitive.
    // if (maxLen <0) maxLen = 0;
    // if (maxLen >layerV->size.x) maxLen = layerV->size.x;

    for (int i=0; i<maxLen; i++) {                                    // The louder the sound, the wider the soundbar. By Andrew Tuline.
      uint8_t index = inoise8(i * sharedData.volume + aux0, aux1 + i * sharedData.volume);  // Get a value from the noise function. I'm using both x and y axis.
      for (int y = 0; y < layerV->size.y; y++) //propagate to other dimensions
        for (int z = 0; z < layerV->size.z; z++)
          layerV->setRGB(Coord3D(i, y, z), ColorFromPalette(layerV->layerP->palette, index));//, 255, PALETTE_SOLID_WRAP));
    }

    aux0+=beatsin8(5,0,10);
    aux1+=beatsin8(4,0,10);
  }
}; //NoiseMeter

// Author: @TroyHacks, from WLED MoonModules
// @license GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
class PaintBrushEffect: public Node {
public:

  static const char * name() {return "Paintbrush 🔥🎨♫🧊🐺";}
  static uint8_t dim() {return _1D;}
  static const char * tags() {return "";}

  uint8_t oscillatorOffset = 6 * 160 / 255;
  uint8_t numLines = 255;
  uint8_t fadeRate = 40;
  uint8_t minLength = 0;
  bool color_chaos = false;
  bool soft = true;
  bool phase_chaos = false;
  
  void setup() override {
    addControl(oscillatorOffset, "oscillatorOffset", "range", 0, 16);
    addControl(numLines, "numLines", "range", 2, 255);
    addControl(fadeRate, "fadeRate", "range", 0, 128);
    addControl(minLength, "minLength", "range");
    addControl(color_chaos, "color_chaos", "checkbox");
    addControl(soft, "soft", "checkbox");
    addControl(phase_chaos, "phase_chaos", "checkbox");
  }

  uint16_t aux0Hue;
  uint16_t aux1Chaos;

  void loop() override {
    const uint16_t cols = layerV->size.x;
    const uint16_t rows = layerV->size.y;
    const uint16_t depth = layerV->size.z;

    // uint8_t numLines = map8(nrOfLines,1,64);
    
    (aux0Hue)++;  // hue
    layerV->fadeToBlackBy(fadeRate);

    aux1Chaos = phase_chaos?random8():0;

    for (size_t i = 0; i < numLines; i++) {
      uint8_t bin = ::map(i,0,numLines,0,NUM_GEQ_CHANNELS-1);
      
      uint8_t x1 = beatsin8(oscillatorOffset*1 + sharedData.bands[0]/NUM_GEQ_CHANNELS, 0, (cols-1), sharedData.bands[bin], aux1Chaos);
      uint8_t x2 = beatsin8(oscillatorOffset*2 + sharedData.bands[0]/NUM_GEQ_CHANNELS, 0, (cols-1), sharedData.bands[bin], aux1Chaos);
      uint8_t y1 = beatsin8(oscillatorOffset*3 + sharedData.bands[0]/NUM_GEQ_CHANNELS, 0, (rows-1), sharedData.bands[bin], aux1Chaos);
      uint8_t y2 = beatsin8(oscillatorOffset*4 + sharedData.bands[0]/NUM_GEQ_CHANNELS, 0, (rows-1), sharedData.bands[bin], aux1Chaos);
      uint8_t z1;
      uint8_t z2;
      int length;
      if (depth > 1) {
        z1 = beatsin8(oscillatorOffset*5 + sharedData.bands[0]/NUM_GEQ_CHANNELS, 0, (depth-1), sharedData.bands[bin], aux1Chaos);
        z2 = beatsin8(oscillatorOffset*6 + sharedData.bands[0]/NUM_GEQ_CHANNELS, 0, (depth-1), sharedData.bands[bin], aux1Chaos);

        length = sqrt((x2-x1) * (x2-x1) + (y2-y1) * (y2-y1) + (z2-z1) * (z2-z1));
      } else 
        length = sqrt((x2-x1) * (x2-x1) + (y2-y1) * (y2-y1));

      length = map8(sharedData.bands[bin],0,length);

      if (length > max(1, (int)minLength)) {
        CRGB color;
        if (color_chaos)
          color = ColorFromPalette(layerV->layerP->palette, i * 255 / numLines + ((aux0Hue)&0xFF), 255);
        else
          color = ColorFromPalette(layerV->layerP->palette, ::map(i, 0, numLines, 0, 255), 255);
        if (depth > 1)
          layerV->drawLine3D(x1, y1, z1, x2, y2, z2, color, soft, length); // no soft implemented in 3D yet
        else
          layerV->drawLine(x1, y1, x2, y2, color, soft, length);
      }
    }
  }
};

//each needs 19 bytes
//Spark type is used for popcorn, fireworks, and drip
struct Spark {
  float pos, posX;
  float vel, velX;
  uint16_t col;
  uint8_t colIndex;
};

#define maxNumPopcorn 21 // max 21 on 16 segment ESP8266
#define NUM_COLORS       3 /* number of colors per segment */

class PopCornEffect: public Node {
  public:
  static const char * name() {return "PopCorn ♪🎨💡";}
  static uint8_t dim() {return _1D;}
  static const char * tags() {return "";}
  
  uint8_t speed = 128;
  uint8_t numPopcorn = maxNumPopcorn/2;
  bool useaudio = false;

  void setup() override {
    addControl(speed, "speed", "range");
    addControl(numPopcorn, "corns", "range", 1, maxNumPopcorn);
    addControl(useaudio, "useaudio", "checkbox");
  }

  Spark popcorn[maxNumPopcorn]; //array

  void loop() override {

    //binding of loop persistent values (pointers)

    layerV->fill_solid(CRGB::Black);

    float gravity = -0.0001f - (speed/200000.0f); // m/s/s
    gravity *= layerV->size.x;

    if (numPopcorn == 0) numPopcorn = 1;

    for (int i = 0; i < numPopcorn; i++) {
      if (popcorn[i].pos >= 0.0f) { // if kernel is active, update its position
        popcorn[i].pos += popcorn[i].vel;
        popcorn[i].vel += gravity;
      } else { // if kernel is inactive, randomly pop it
        bool doPopCorn = false;  // WLEDMM allows to inhibit new pops
        // WLEDMM begin
        if (useaudio) {
          if (  (sharedData.volume > 1.0f)                      // no pops in silence
              // &&((audioSync->sync.samplePeak > 0) || (audioSync->sync.volumeRaw > 128))  // try to pop at onsets (our peek detector still sucks)
              &&(random8() < 4) )                        // stay somewhat random
            doPopCorn = true;
        } else {         
          if (random8() < 2) doPopCorn = true; // default POP!!!
        }

        if (doPopCorn) { // POP!!!
          popcorn[i].pos = 0.01f;

          uint16_t peakHeight = 128 + random8(128); //0-255
          peakHeight = (peakHeight * (layerV->size.x -1)) >> 8;
          popcorn[i].vel = sqrtf(-2.0f * gravity * peakHeight);

          // if (layerV->palette)
          // {
            popcorn[i].colIndex = random8();
          // } else {
          //   uint8_t  col = random8(0, NUM_COLORS);
          //   if (!SEGCOLOR(2) || !SEGCOLOR(col)) col = 0;
          //   popcorn[i].colIndex = col;
          // }
        }
      }
      if (popcorn[i].pos >= 0.0f) { // draw now active popcorn (either active before or just popped)
        // uint32_t col = layerV->color_wheel(popcorn[i].colIndex);
        // if (!layerV->palette && popcorn[i].colIndex < NUM_COLORS) col = SEGCOLOR(popcorn[i].colIndex);
        uint16_t ledIndex = popcorn[i].pos;
        CRGB col = ColorFromPalette(layerV->layerP->palette, popcorn[i].colIndex*(256/maxNumPopcorn));
        if (ledIndex < layerV->size.x) {
          layerV->setRGB(ledIndex, col);
          for (int y = 0; y < layerV->size.y; y++)
            for (int z = 0; z < layerV->size.z; z++)
              layerV->setRGB(Coord3D(ledIndex,y,z), col);
        }
      }
    }
  }
}; //PopCorn

class RainbowEffect: public Node {
public:

  static const char * name() {return "Rainbow 🔥";}
  static uint8_t dim() {return _1D;}
  static const char * tags() {return "";}

  uint8_t deltaHue = 7;
  uint8_t speed = 8; //default 8*32 = 256 / 256 = 1 = hue++

  void setup() {
    addControl(speed, "speed", "range", 0, 32);
    addControl(deltaHue, "deltaHue", "range", 0, 32);
  }

  uint16_t hue = 0;

  void loop() override {
    layerV->fill_rainbow((hue+=speed*32) >> 8, deltaHue); //hue back to uint8_t
  }
};

class RandomEffect: public Node {
  public:

  static const char * name() {return "Random 🔥";}
  static uint8_t dim() {return _1D;}
  static const char * tags() {return "";}

  uint8_t speed=128;
  void setup() {
    addControl(speed, "speed", "range");
  }
  void loop() override {
      layerV->fadeToBlackBy(70);
      layerV->setRGB(random16(layerV->nrOfLights), CRGB(255, random8(), 0));
  }
};

class RipplesEffect: public Node {
  public:

  static const char * name() {return "Ripples 🔥🧊";}
  static uint8_t dim() {return _3D;}
  static const char * tags() {return "";}
  
  uint8_t speed = 50;
  uint8_t interval = 128;

  void setup() override {
    addControl(speed, "speed", "range");
    addControl(interval, "interval", "range");
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

        layerV->setRGB(pos, (CRGB)CHSV( millis()/50 + random8(64), 200, 255));
      }
    }
  }
};

#if USE_M5UNIFIED
    #include <M5Unified.h>
#endif

class ScrollingTextEffect: public Node {
public:
  static const char * name() {return "Scrolling Text 🔥";}
  static uint8_t dim() {return _1D;}
  static const char * tags() {return "";}

  char textIn[32];
  Char<32> text;// = Char<32>("MoonLight");
  uint8_t speed = 0;
  uint8_t font = 0;
  uint8_t preset = 0;
  Coord3D ctest;

  void setup() override {
    JsonObject property;
    JsonArray values;
    property = addControl(font, "font", "select");
    values = property["values"].to<JsonArray>();
    values.add("4x6");
    values.add("5x8");
    values.add("5x12");
    values.add("6x8");
    values.add("7x9");

    property = addControl(preset, "preset", "select");
    values = property["values"].to<JsonArray>();
    values.add("Auto");
    values.add("Text");
    values.add("IP");
    values.add("FPS");
    values.add("Time");
    values.add("Up");
    values.add("Status 🛜");
    values.add("Clients 🛜");
    values.add("Connected 🛜");
    values.add("Free memory");

    addControl(textIn, "text", "text", 1, sizeof(textIn)); //size needed to protect char array!

    addControl(speed, "speed", "range");
  }

  void loop() override {

    layerV->fadeToBlackBy();

    uint8_t choice;
    if (preset > 0) //not auto
      choice = preset;
    else {
      if (strlen(textIn) == 0) 
        choice = (millis()/1000 % 8)+2;
      else
        choice = (millis()/1000 % 9)+1;
    } 
    
    switch (choice) {
      case 1: text.format("%s", textIn); break;
      case 2: text.format(".%d", WiFi.localIP()[3]); break;
      case 3: text.format("%ds", sharedData.fps); break;
      case 4: text.formatTime(time(nullptr), "%H%M"); break;
      case 5: 
        #define MILLIS_PER_MINUTE (60*1000)
        #define MILLIS_PER_HOUR (MILLIS_PER_MINUTE * 60)
        #define MILLIS_PER_DAY (MILLIS_PER_HOUR * 24)
        if (millis() < MILLIS_PER_MINUTE) //within 1 minute
          text.format("%ds", millis()/1000); //seconds
        else if (millis() < MILLIS_PER_MINUTE*10) //within 10 min
          text.format("%dm%d", millis()/MILLIS_PER_MINUTE, (millis()/1000)%60); //minutes and seconds 
        else if (millis() < MILLIS_PER_HOUR) //within 1 hour
          text.format("%dm", millis()/MILLIS_PER_MINUTE);  //minutes
        else if (millis() < MILLIS_PER_HOUR*10) //within 10 hours
          text.format("%dh%d", millis()/MILLIS_PER_HOUR, (millis()/MILLIS_PER_MINUTE)%60); //hours and minutes
        else if (millis() < MILLIS_PER_DAY) //within 1 day
          text.format("%dh", millis()/MILLIS_PER_HOUR); //hours
        else if (millis() < MILLIS_PER_DAY*10) //within 10 days
          text.format("%dd%d", millis()/(MILLIS_PER_DAY), (millis()/MILLIS_PER_HOUR)%24); //days and hours
        else // more than 10 days
          text.format("%dh", millis()/(MILLIS_PER_DAY));  //days
        break;
      case 6: text.format("%s", sharedData.connectionStatus==0?"Off":sharedData.connectionStatus==1?"AP-":sharedData.connectionStatus==2?"AP+":sharedData.connectionStatus==3?"Sta-":sharedData.connectionStatus==4?"Sta+":"mqqt"); break;
      case 7: text.format("%dC", sharedData.clientListSize); break;
      case 8: text.format("%dCC", sharedData.connectedClients); break;
      case 9: text.format("%dKB", ESP.getFreeHeap()/1024); break;
    }
    layerV->setRGB(Coord3D(choice-1), CRGB::Blue); 

    // EVERY_N_SECONDS(1)
    //   Serial.printf(" %d:%s", choice-1, text.c_str());

    // if (text && strnlen(text.c_str(), 2) > 0) {
      layerV->drawText(text.c_str(), 0, 1, font, CRGB::Red, - (millis()/25*speed/256)); //instead of call
    // }

    #if USE_M5UNIFIEDDisplay
      M5.Display.fillRect(0, 0, 100, 15, BLACK);
      M5.Display.setTextColor(WHITE);
      M5.Display.setTextSize(2);
      M5.Display.drawString(text.c_str(), 0, 0);
    #endif
  }
}; //ScrollingText

//AI generated
class SinusEffect: public Node {
  public:

  static const char * name() {return "Sinus 🔥";}
  static uint8_t dim() {return _1D;}
  static const char * tags() {return "";}

  uint8_t speed = 5;

  void setup() override {
    addControl(speed, "speed", "range");
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
        layerV->setRGB(i, (CRGB)CHSV(hue, 255, brightness));
    }

    // Increment the phase to animate the wave
    phase += speed;
  }
};

class SphereMoveEffect: public Node {
  public:

  static const char * name() {return "Sphere Move 🔥🧊";}
  static uint8_t dim() {return _3D;}
  static const char * tags() {return "";}

  uint8_t speed = 50;
  
  void setup() override {
    addControl(speed, "speed", "range", 0, 99);
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
                  layerV->setRGB(pos, (CRGB)CHSV( millis()/50 + random8(64), 200, 255));
                }
            }
        }
    }
  }
}; // SphereMoveEffect

//by @Brandon502
class StarFieldEffect: public Node {  // Inspired by Daniel Shiffman's Coding Train https://www.youtube.com/watch?v=17WoOqgXsRM
  public:
  static const char * name() {return "StarField 🔥🎨";}
  static uint8_t     dim() {return _2D;}
  static const char * tags() {return "";}

  struct Star {
    int x, y, z;
    uint8_t colorIndex;
  };

  static float fmap(const float x, const float in_min, const float in_max, const float out_min, const float out_max) {
    return (out_max - out_min) * (x - in_min) / (in_max - in_min) + out_min;
  }

  uint8_t speed = 20;
  uint8_t numStars = 16;
  uint8_t blur = 128;
  bool usePalette = false;

  void setup() override {
    addControl(speed, "speed", "range", 0, 30);
    addControl(numStars, "numStars", "range", 1, 255);
    addControl(blur, "blur", "range", 0, 255);
    addControl(usePalette, "usePalette", "checkbox");

    //set up all stars
    for (int i = 0; i < 255; i++) {
      stars[i].x = random(-layerV->size.x, layerV->size.x);
      stars[i].y = random(-layerV->size.y, layerV->size.y);
      stars[i].z = random(layerV->size.x);
      stars[i].colorIndex = random8();
    }
  }

  unsigned long step;
  Star stars[255];

  void loop() override {

    if (!speed || millis() - step < 1000 / speed) return; // Not enough time passed

    layerV->fadeToBlackBy(blur);

    for (int i = 0; i < numStars; i++) {
      //update star
      // MB_LOGD(ML_TAG, "Star %d Pos: %d, %d, %d -> ", i, stars[i].x, stars[i].y, stars[i].z);
      float sx = layerV->size.x/2.0 + fmap(float(stars[i].x) / stars[i].z, 0, 1, 0, layerV->size.x/2.0);
      float sy = layerV->size.y/2.0 + fmap(float(stars[i].y) / stars[i].z, 0, 1, 0, layerV->size.y/2.0);

      // MB_LOGD(ML_TAG, " %f, %f\n", sx, sy);

      Coord3D pos = Coord3D(sx, sy);
      if (!pos.isOutofBounds(layerV->size)) {
        if (usePalette) layerV->setRGB(Coord3D(sx, sy), ColorFromPalette(layerV->layerP->palette, stars[i].colorIndex, ::map(stars[i].z, 0, layerV->size.x, 255, 150)));
        else {
          uint8_t color = ::map(stars[i].colorIndex, 0, 255, 120, 255);
          int brightness = ::map(stars[i].z, 0, layerV->size.x, 7, 10);
          color *= brightness/10.0;
          layerV->setRGB(Coord3D(sx, sy), CRGB(color, color, color));
        }
      }
      stars[i].z -= 1;
      if (stars[i].z <= 0 || pos.isOutofBounds(layerV->size)) {
        stars[i].x = random(-layerV->size.x, layerV->size.x);
        stars[i].y = random(-layerV->size.y, layerV->size.y);
        stars[i].z = layerV->size.x;
        stars[i].colorIndex = random8();
      }
    }

    step = millis();
  }
}; //StarFieldEffect

 // BY MONSOONO / @Flavourdynamics 
class PraxisEffect: public Node {
public:
  static const char * name() {return "Praxis 🔥🎨";}
  static uint8_t dim() {return _2D;}
  static const char * tags() {return "";}

  uint8_t macroMutatorFreq = 3;
  uint8_t macroMutatorMin = 250;
  uint8_t macroMutatorMax = 255;
  uint8_t microMutatorFreq = 4;
  uint8_t microMutatorMin = 200;
  uint8_t microMutatorMax = 255;

  void setup() override {
    addControl(macroMutatorFreq, "macroMutatorFreq", "range", 0, 15);
    addControl(macroMutatorMin, "macroMutatorMin", "range", 0, 255);
    addControl(macroMutatorMax, "macroMutatorMax", "range", 0, 255);
    addControl(microMutatorFreq, "microMutatorFreq", "range", 0, 15);
    addControl(microMutatorMin, "microMutatorMin", "range", 0, 255);
    addControl(microMutatorMax, "microMutatorMax", "range", 0, 255);
    //ui->initSlider(parentVar, "hueSpeed", layerV->effectData.write<uint8_t>(20), 1, 100); // (14), 1, 255)
    //ui->initSlider(parentVar, "saturation", layerV->effectData.write<uint8_t>(255), 0, 255);
  }

  void loop() override {
    //uint8_t huespeed = layerV->effectData.read<uint8_t>();
    //uint8_t saturation = layerV->effectData.read<uint8_t>(); I will revisit this when I have a display

    uint16_t macro_mutator = beatsin16(macroMutatorFreq, macroMutatorMin << 8, macroMutatorMax << 8); // beatsin16(14, 65350, 65530);
    uint16_t micro_mutator = beatsin16(microMutatorFreq, microMutatorMin, microMutatorMax); // beatsin16(2, 550, 900);
    //uint16_t macro_mutator = beatsin8(macroMutatorFreq, macroMutatorMin, macroMutatorMax); // beatsin16(14, 65350, 65530);
    //uint16_t micro_mutator = beatsin8(microMutatorFreq, microMutatorMin, microMutatorMax); // beatsin16(2, 550, 900);
    
    Coord3D pos = {0,0,0};
    uint8_t huebase = millis() / 40; // 1 + ~huespeed

    for (pos.x = 0; pos.x < layerV->size.x; pos.x++){
      for(pos.y = 0; pos.y < layerV->size.y; pos.y++){
        //uint8_t hue = huebase + (-(pos.x+pos.y)*macro_mutator*10) + ((pos.x+pos.x*pos.y*(macro_mutator*256))/(micro_mutator+1));
        uint8_t hue = huebase + ((pos.x+pos.y*macro_mutator*pos.x)/(micro_mutator+1));
        // uint8_t hue = huebase + ((pos.x+pos.y)*(250-macro_mutator)/5) + ((pos.x+pos.y*macro_mutator*pos.x)/(micro_mutator+1)); Original
        CRGB colour = ColorFromPalette(layerV->layerP->palette, hue, 255);
        layerV->setRGB(pos, colour);// blend(layerV->getRGB(pos), colour, 155);
      }
    }
  }
}; // Praxis


class WaverlyEffect: public Node {
  public:
  static const char * name() {return "Waverly 🔥♪🎨💡";}
  static uint8_t dim() {return _2D;}
  static const char * tags() {return "";}
  
  uint8_t fadeRate = 128;
  uint8_t amplification = 30;
  bool noClouds = false;

  void setup() override {
    addControl(fadeRate, "fadeRate", "range");
    addControl(amplification, "amplification", "range");
    addControl(noClouds, "noClouds", "checkbox");

    // addControl(bool, "soundPressure", layerV->effectData.write<bool>(false));
    // addControl(bool, "AGCDebug", layerV->effectData.write<bool>(false));
  }

  void loop() override {
    //Binding of controls. Keep before binding of vars and keep in same order as in setup()
    // bool soundPressure = layerV->effectData.read<bool>();
    // bool agcDebug = layerV->effectData.read<bool>();

    layerV->fadeToBlackBy(fadeRate);
    //netmindz: cannot find these in audio sync
    // if (agcDebug && soundPressure) soundPressure = false;                 // only one of the two at any time
    // if ((soundPressure) && (audioSync->sync.volumeSmth > 0.5f)) audioSync->sync.volumeSmth = audioSync->sync.soundPressure;    // show sound pressure instead of volume
    // if (agcDebug) audioSync->sync.volumeSmth = 255.0 - audioSync->sync.agcSensitivity;                    // show AGC level instead of volume

    long t = millis() / 2; 
    Coord3D pos = {0,0,0}; //initialize z otherwise wrong results
    for (pos.x = 0; pos.x < layerV->size.x; pos.x++) {
      uint16_t thisVal = sharedData.volume * amplification * inoise8(pos.x * 45 , t , t) / 4096;      // WLEDMM back to SR code
      uint16_t thisMax = min(map(thisVal, 0, 512, 0, layerV->size.y), (long)layerV->size.y);

      for (pos.y = 0; pos.y < thisMax; pos.y++) {
        CRGB color = ColorFromPalette(layerV->layerP->palette, ::map(pos.y, 0, thisMax, 250, 0));
        if (!noClouds)
          layerV->addRGB(pos, color);
        layerV->addRGB(Coord3D((layerV->size.x - 1) - pos.x, (layerV->size.y - 1) - pos.y), color);
      }
    }
    layerV->blur2d(16);
  }
}; //Waverly

class WaveEffect: public Node {
public:

  static const char * name() {return "Wave 🔥";}
  static uint8_t dim() {return _1D;}
  static const char * tags() {return "";}

  uint8_t bpm = 60;
  uint8_t fade = 20;
  uint8_t type = 0;

  void setup() override {
    addControl(bpm, "bpm", "range");
    addControl(fade, "fade", "range");
    JsonObject property = addControl(type, "type", "select"); 
    JsonArray values = property["values"].to<JsonArray>();
    values.add("Saw");
    values.add("Triangle");
    values.add("Sinus");
    values.add("Square");
    values.add("Sin3"); //with @pathightree
    values.add("Noise");
  }

  void loop() override {
    layerV->fadeToBlackBy(fade); //should only fade rgb ...

    CRGB color = CHSV( millis()/50, 255, 255);

    int prevPos = layerV->size.x/2; //somewhere in the middle

    for (int y = 0; y<layerV->size.y; y++) {
      int pos = 0;

      uint8_t b8 = beat8(bpm, y*100);
      uint8_t bs8 = beatsin8( bpm, 0, 255, y * 100);
      //delay over y-axis..timebase ...
      switch (type) {
        case 0: pos = b8 * layerV->size.x / 256; break;
        case 1: pos = triangle8(bpm, y*100) * layerV->size.x / 256; break;
        case 2: pos = bs8 * layerV->size.x / 256; break;
        case 3: pos = b8 > 128? 0 : layerV->size.x-1; break;
        case 4: pos = (bs8 + beatsin8( bpm*0.65, 0, 255, y * 200) + beatsin8( bpm*1.43, 0, 255, y * 300)) * layerV->size.x / 256 / 3; break;
        case 5: pos = inoise8(millis()*bpm/256 + y*1000) * layerV->size.x / 256; break; //bpm not really bpm, more speed
        default: pos = 0;
      }

      //connect saw and square
      if ((type == 0 || type == 3) && abs(prevPos - pos) > layerV->size.x / 2) {
        for (int x=0; x<layerV->size.x; x++)
          layerV->setRGB(Coord3D(x, y), color);
      }

      layerV->setRGB(Coord3D(pos, y), color); //= CRGB(255, random8(), 0);
      prevPos = pos;
    }
  }
};

class Troy1ColorEffect: public Node {
  public:

  static const char * name() {return "Troy1 Color 🚨♫🐺";}

  bool audioReactive = true;

  void setup() override {
    addControl(audioReactive, "audioReactive", "checkbox");
  }

  void loop() override {

    for (int x=0; x<layerV->size.x; x++) { // loop over lights defined in layout
      if (audioReactive) {
        layerV->setRGB(x, CRGB(sharedData.bands[NUM_GEQ_CHANNELS-1],sharedData.bands[7],sharedData.bands[0]));
        layerV->setBrightness(x, (sharedData.bands[0]>200)?0:layerV->layerP->lights.header.brightness);
      } else {
        layerV->setRGB(x, CHSV( beatsin8(10), 255, 255));
        // layerV->setBrightness(x, layerV->layerP->lights.header.brightness); // done automatically
      }
    }
  }
};

class Troy1MoveEffect: public Node {
  public:

  static const char * name() {return "Troy1 Move 🗼♫🐺";}

  //set default values here
  uint8_t bpm = 30;
  uint8_t pan = 175;
  uint8_t tilt = 90;
  uint8_t zoom = 20;
  uint8_t range = 20;
  uint8_t colorwheel = 0;
  uint8_t colorwheelbrightness = 255; // 0-255, 0 = off, 255 = full brightness
  time_t cooldown = millis();
  
  bool autoMove = true;
  bool audioReactive = true;
  bool invert = true;
  int closestColorIndex = -1;

  std::vector<CRGB> colorwheelpalette = {
      CRGB(255, 255, 255), // White
      CRGB(255, 0, 0),     // Red
      CRGB(0, 255, 0),     // Green
      CRGB(0, 0, 255),     // Blue
      CRGB(255, 255, 0),   // Yellow
      CRGB(0, 255, 255),   // Cyan
      CRGB(255, 165, 0),   // Orange
      CRGB(128, 0, 128)    // Purple
  };

  void setup() override {
    addControl(bpm, "bpm", "range");
    addControl(pan, "pan", "range");
    addControl(tilt, "tilt", "range");
    addControl(zoom, "zoom", "range");
    addControl(colorwheel, "colorwheel", "range", 0, 7); // 0-7 for 8 colors in the colorwheel
    addControl(colorwheelbrightness, "colorwheelbrightness", "range"); // 0-7 for 8 colors in the colorwheel
    addControl(autoMove, "autoMove", "checkbox");
    addControl(range, "range", "range");
    addControl(audioReactive, "audioReactive", "checkbox");
    addControl(invert, "invert", "checkbox");
  }

  // Function to compute Euclidean distance between two colors
  double colorDistance(const CRGB& c1, const CRGB& c2) {
      return std::sqrt(std::pow(c1.r - c2.r, 2) + std::pow(c1.g - c2.g, 2) + std::pow(c1.b - c2.b, 2));
  }

  // Function to find the index of the closest color
  int findClosestColorWheelIndex(const CRGB& inputColor, const std::vector<CRGB>& palette) {
      int closestIndex = 0;
      double minDistance = colorDistance(inputColor, palette[0]);

      for (size_t i = 1; i < palette.size(); ++i) {
          double distance = colorDistance(inputColor, palette[i]);
          if (distance < minDistance) {
              minDistance = distance;
              closestIndex = static_cast<int>(i);
          }
      }

      return closestIndex;
  }

  void loop() override {

    for (int x=0; x < layerV->size.x; x++) { // loop over lights defined in layout
      if (audioReactive) {
        if (sharedData.bands[2] > 200 && cooldown + 3000 < millis()) { //cooldown for 3 seconds
          cooldown = millis();
          colorwheel = random8(8)*5; //random colorwheel index and convert to 0-35 range
        } 
        layerV->setGobo(x, colorwheel);
        layerV->setBrightness2(x, (sharedData.bands[0]>200)?sharedData.bands[0]:0); // Use the first band for brightness
        layerV->setZoom(x, (sharedData.bands[0]>200)?0:128);
        uint8_t mypan = beatsin8(bpm, pan-range, pan+range, 0, sharedData.bands[0]/2);
        uint8_t mytilt = beatsin8(bpm, tilt-range, tilt+range, 0, sharedData.bands[0]/2);
        if (invert && x%2==0) {
          mypan = 255 - mypan; // invert pan
          mytilt = 255 - mytilt; // invert tilt
        }
        layerV->setPan(x, mypan); 
        layerV->setTilt(x, mytilt);
      } else {
        layerV->setGobo(x,colorwheel);
        layerV->setBrightness2(x, colorwheelbrightness); // layerV->layerP->lights.header.brightness);
        layerV->setPan(x, autoMove?beatsin8(bpm, pan-range, pan + range, 0,  (invert && x%2==0)?128:0): pan); //if automove, pan the light over a range
        layerV->setTilt(x, autoMove?beatsin8(bpm, tilt - range, tilt + range, 0,  (invert && x%2==0)?128:0): tilt);
        // layerV->setBrightness(x, layerV->layerP->lights.header.brightness); // done automatically
      }
    }
  }
};

class Troy2ColorEffect: public Node {
  public:

  static const char * name() {return "Troy2 Color 🚨♫🐺";}

  uint8_t cutin = 200;
  
  bool audioReactive = true;

  void setup() override {
    addControl(cutin, "cutin", "range");
    addControl(audioReactive, "audioReactive", "checkbox");
  }

  void loop() override {
    for (int x=0; x < layerV->size.x; x++) { // loop over lights defined in layout
      if (audioReactive) {
        layerV->setRGB(x, CRGB(sharedData.bands[NUM_GEQ_CHANNELS-1]>cutin?sharedData.bands[NUM_GEQ_CHANNELS-1]:0,sharedData.bands[7]>cutin?sharedData.bands[7]:0,sharedData.bands[0]));
        layerV->setRGB1(x, CRGB(sharedData.bands[NUM_GEQ_CHANNELS-1],sharedData.bands[7]>cutin?sharedData.bands[7]:0,sharedData.bands[0]>cutin?sharedData.bands[0]:0));
        layerV->setRGB2(x, CRGB(sharedData.bands[NUM_GEQ_CHANNELS-1]>cutin?sharedData.bands[NUM_GEQ_CHANNELS-1]:0,sharedData.bands[7],sharedData.bands[0]>cutin?sharedData.bands[0]:0));
        layerV->setRGB3(x, CRGB(sharedData.bands[NUM_GEQ_CHANNELS-1]>cutin?::map(sharedData.bands[NUM_GEQ_CHANNELS-1],cutin-1,255,0,255):0,sharedData.bands[7]>cutin?::map(sharedData.bands[7],cutin-1,255,0,255):0,sharedData.bands[0]>cutin?::map(sharedData.bands[0],cutin-1,255,0,255):0));
        // layerV->setZoom(x, (sharedData.bands[0]>cutin)?255:0);
        if (sharedData.bands[0]+sharedData.bands[7]+sharedData.bands[NUM_GEQ_CHANNELS-1] > 1) {
          layerV->setBrightness(x, 255);
        } else {
          layerV->setBrightness(x, 0);
        }
      } else {
        layerV->setRGB(x, CHSV( beatsin8(10), 255, 255));
        // layerV->setBrightness(x, layerV->layerP->lights.header.brightness); // done automatically
      }
    }
  }
};

class Troy2MoveEffect: public Node {
  public:

  static const char * name() {return "Troy2 Move 🗼♫🐺";}

  uint8_t bpm = 30;
  uint8_t pan = 175;
  uint8_t tilt = 90;
  uint8_t zoom = 20;
  uint8_t range = 20;
  uint8_t cutin = 200;
  time_t cooldown = millis();
  
  bool autoMove = true;
  bool audioReactive = true;
  bool invert = true;

  void setup() override {
    addControl(bpm, "bpm", "range");
    addControl(pan, "pan", "range");
    addControl(tilt, "tilt", "range");
    addControl(zoom, "zoom", "range");
    addControl(cutin, "cutin", "range");
    addControl(autoMove, "autoMove", "checkbox");
    addControl(range, "range", "range");
    addControl(audioReactive, "audioReactive", "checkbox");
    addControl(invert, "invert", "checkbox");
  }

  // Function to compute Euclidean distance between two colors
  double colorDistance(const CRGB& c1, const CRGB& c2) {
      return std::sqrt(std::pow(c1.r - c2.r, 2) + std::pow(c1.g - c2.g, 2) + std::pow(c1.b - c2.b, 2));
  }

  // Function to find the index of the closest color
  int findClosestColorWheelIndex(const CRGB& inputColor, const std::vector<CRGB>& palette) {
      int closestIndex = 0;
      double minDistance = colorDistance(inputColor, palette[0]);
      for (size_t i = 1; i < palette.size(); ++i) {
          double distance = colorDistance(inputColor, palette[i]);
          if (distance < minDistance) {
              minDistance = distance;
              closestIndex = static_cast<int>(i);
          }
      }
      return closestIndex;
  }

  void loop() override {
    bool coolDownSet = false;
    for (int x=0; x < layerV->size.x; x++) { // loop over lights defined in layout
      if (audioReactive) {
        if (sharedData.bands[0] > cutin) {
          layerV->setZoom(x, 255);
          coolDownSet = true;
        } else if (cooldown + 5000 < millis()) {
          layerV->setZoom(x, 0);
          coolDownSet = true;
        }
        // layerV->setZoom(x, (sharedData.bands[0]>cutin)?255:0);
        uint8_t mypan = beatsin8(bpm, pan-range, pan+range, 0, sharedData.bands[0]/2);
        uint8_t mytilt = beatsin8(bpm, tilt-range, tilt+range, 0, sharedData.bands[0]/2);
        if (invert && x%2==0) {
          mypan = 255 - mypan; // invert pan
          mytilt = 255 - mytilt; // invert tilt
        }
        if (sharedData.bands[0]+sharedData.bands[7]+sharedData.bands[NUM_GEQ_CHANNELS-1] > 1) {
          layerV->setPan(x, mypan); 
          layerV->setTilt(x, mytilt);
        } else {
        }
      } else {
        layerV->setPan(x, autoMove?beatsin8(bpm, pan-range, pan + range, 0,  (invert && x%2==0)?128:0): pan); //if automove, pan the light over a range
        layerV->setTilt(x, autoMove?beatsin8(bpm, tilt - range, tilt + range, 0,  (invert && x%2==0)?128:0): tilt);
        // layerV->setBrightness(x, layerV->layerP->lights.header.brightness); // done automatically
      }
    }
    if (coolDownSet) cooldown = millis();
  }
};

class FreqColorsEffect: public Node {
  public:

  static const char * name() {return "Freq Colors 🚨♫";}

  uint8_t bpm = 30;
  bool audioReactive = true;

  void setup() override {
    addControl(bpm, "bpm", "range");
    addControl(audioReactive, "audioReactive", "checkbox");
  }

  void loop() override {

    layerV->fadeToBlackBy(50);

    for (int x=0; x < layerV->size.x; x++) { // loop over lights defined in layout

      if (audioReactive) {
        uint8_t nrOfLights = layerV->size.x * 3;
        uint8_t  delta = 256 / nrOfLights;

        //set the 3 LED groups for each moving head light
        layerV->setRGB({x,0,0}, CHSV( (x) * delta, 255, sharedData.bands[(x * 3) % 16]));
        layerV->setRGB1({x,0,0}, CHSV( (x + 3) * delta, 255, sharedData.bands[(x * 3 + 1) % 16]));
        layerV->setRGB2({x,0,0}, CHSV( (x + 6) * delta, 255, sharedData.bands[(x * 3 + 2) % 16]));
      } else {
        if (x == beatsin8(bpm, 0, layerV->size.x - 1)) { //sinelon over moving heads
          layerV->setRGB({x,0,0}, CHSV( beatsin8(10), 255, 255)); //colorwheel 10 times per minute
          layerV->setRGB1({x,0,0}, CHSV( beatsin8(10), 255, 255)); //colorwheel 10 times per minute
          layerV->setRGB2({x,0,0}, CHSV( beatsin8(10), 255, 255)); //colorwheel 10 times per minute
        }
      }

      // layerV->setBrightness({x,0,0}, layerV->layerP->lights.header.brightness); // done automatically
    }
  }
};

class WowiMoveEffect: public Node {
  public:

  static const char * name() {return "Wowi Move 🗼♫";}

  uint8_t bpm = 30;
  uint8_t pan = 175;
  uint8_t tilt = 90;
  uint8_t zoom = 20;
  uint8_t range = 20;;
  bool autoMove = true;
  bool invert = true;

  void setup() override {
    addControl(bpm, "bpm", "range");
    addControl(pan, "pan", "range");
    addControl(tilt, "tilt", "range");
    addControl(zoom, "zoom", "range");
    addControl(autoMove, "autoMove", "checkbox");
    addControl(range, "range", "range");
    addControl(invert, "invert", "checkbox");
  }

  void loop() override {

    for (int x=0; x < layerV->size.x; x++) { // loop over lights defined in layout
      layerV->setPan({x,0,0}, autoMove?beatsin8(bpm, pan-range, pan + range, 0,  (invert && x%2==0)?128:0): pan); //if automove, pan the light over a range
      layerV->setTilt({x,0,0}, autoMove?beatsin8(bpm, tilt - range, tilt + range, 0,  (invert && x%2==0)?128:0): tilt);
      layerV->setZoom({x,0,0}, zoom);
      // layerV->setBrightness({x,0,0}, layerV->layerP->lights.header.brightness); // done automatically
    }
  }
};

class AmbientMoveEffect: public Node {
  public:

  static const char * name() {return "Ambient Move 🗼♫";}

  uint8_t increaser = 255;
  uint8_t decreaser = 8;
  uint8_t tiltMin = 128;
  uint8_t tiltMax = 255;
  uint8_t panMin = 45;
  uint8_t panMax = 135;
  uint8_t panBPM = 5;
  bool invert = true;

  void setup() override {
    addControl(increaser, "increaser", "range");
    addControl(decreaser, "decreaser", "range");
    addControl(tiltMin, "tiltMin", "range");
    addControl(tiltMax, "tiltMax", "range");
    addControl(panMin, "panMin", "range");
    addControl(panMax, "panMax", "range");
    addControl(panBPM, "panBPM", "range");
    addControl(invert, "invert", "checkbox");

    memset(bandSpeed, 0, NUM_GEQ_CHANNELS);
  }

  uint16_t bandSpeed[NUM_GEQ_CHANNELS];

  void loop() override {

    for (int x = 0; x<layerV->size.x; x++) { //x-axis (column)

      uint8_t band = ::map(x, 0, layerV->size.x-1, 2, NUM_GEQ_CHANNELS-3); //the frequency band applicable for the column, skip the lowest and the highest
      uint8_t volume = sharedData.bands[band]; // the volume for the frequency band

      bandSpeed[band] = constrain(bandSpeed[band] + (volume * increaser) - decreaser * 10, 0, UINT16_MAX); // each band has a speed, increased by the volume and also decreased by decreaser. The decreaser should cause a delay

      uint8_t tilt = ::map(bandSpeed[band], 0, UINT16_MAX, tiltMin, tiltMax); //the higher the band speed, the higher the tilt

      uint8_t pan = ::map(beatsin8((bandSpeed[band] > UINT16_MAX/4)?panBPM:0, 0, 255, 0, (invert && x%2==0)?128:0), 0, 255, panMin, panMax); //expect a bit of volume before panning
      uint8_t tilt2 = ::map(beatsin8((bandSpeed[band] > UINT16_MAX/4)?panBPM:0, 0, 255, 0, 64), 0, 255, panMin, panMax); //this is beatcos8, so pan and tilt draw a circle

      layerV->setTilt(x, (tilt+tilt2)/2);
      layerV->setPan(x, pan);
    }
  }
}; //AmbientMoveEffect


class FreqSawsEffect: public Node {
public:
  static const char * name() {return "Frequency Saws 🔥♫🎨🪚";}
  uint8_t fade = 4;
  uint8_t increaser = 255;
  uint8_t decreaser = 255;
  uint8_t bpmMax = 115;
  bool invert = false;
  bool keepOn = false;
  uint8_t method = 2;

  void setup() override {
    addControl(fade, "fade", "range");
    addControl(increaser, "increaser", "range");
    addControl(decreaser, "decreaser", "range");
    addControl(bpmMax, "bpmMax", "range");
    addControl(invert, "invert", "checkbox");
    addControl(keepOn, "keepOn", "checkbox");
    JsonObject property = addControl(method, "method", "select");
    JsonArray values = property["values"].to<JsonArray>();
    values.add("Chaos");
    values.add("Chaos fix");
    values.add("BandPhases");
  
    memset(bandSpeed, 0, NUM_GEQ_CHANNELS);
    memset(bandPhase, 0, NUM_GEQ_CHANNELS);
    memset(lastBpm, 0, NUM_GEQ_CHANNELS);
    memset(phaseOffset, 0, NUM_GEQ_CHANNELS);
  
    lastTime = millis();
  }

  uint16_t bandSpeed[NUM_GEQ_CHANNELS];
  uint16_t bandPhase[NUM_GEQ_CHANNELS]; // Track phase position for each band
  uint8_t lastBpm[NUM_GEQ_CHANNELS]; // For beat8 continuity tracking
  uint8_t phaseOffset[NUM_GEQ_CHANNELS]; // Phase offset for beat8 sync
  unsigned long lastTime; // For time-based phase calculation

  void loop() override {
    layerV->fadeToBlackBy(fade);
    // Update timing for frame-rate independent phase
    unsigned long currentTime = millis();
    uint32_t deltaMs = currentTime - lastTime;
    lastTime = currentTime;

    for (int x = 0; x<layerV->size.x; x++) { //x-axis (column)
      uint8_t band = ::map(x, 0, layerV->size.x, 0, NUM_GEQ_CHANNELS); //the frequency band applicable for the column, skip the lowest and the highest
      uint8_t volume = sharedData.bands[band]; // the volume for the frequency band
      // Target speed based on current volume
      uint16_t targetSpeed = (volume * increaser * 257);

      if (volume > 0) {
        bandSpeed[band] = max(bandSpeed[band], targetSpeed);
      } else {
        // Calculate decay amount based on time to reach zero
        if (decreaser > 0 && bandSpeed[band] > 0) {
          uint32_t decayAmount = MAX((bandSpeed[band] * deltaMs) / (decreaser * 10UL), 1); // *10 to scale decreaser range 0-255 to 0-2550ms
          if (decayAmount >= bandSpeed[band]) {
            bandSpeed[band] = 0;
          } else {
            bandSpeed[band] -= decayAmount;
          }
        }
      }

      if (bandSpeed[band] > 1 || keepOn) { // for some reason bandSpeed[band] doesn't reach 0 ... WIP
        uint8_t bpm = ::map(bandSpeed[band], 0, UINT16_MAX, 0, bpmMax); //the higher the band speed, the higher the beats per minute.
        uint8_t y;
        if (method == 0) { //chaos
          y = ::map(beat8(bpm), 0, 255, 0, layerV->size.y-1); // saw wave, running over the y-axis, speed is determined by bpm
        } else if (method == 1) { //chaos corrected
          // Maintain phase continuity when BPM changes
          if (bpm != lastBpm[band]) {
            uint8_t currentPos = beat8(lastBpm[band]) + phaseOffset[band];
            uint8_t newPos = beat8(bpm);
            phaseOffset[band] = currentPos - newPos;
            lastBpm[band] = bpm;
          }
          y = ::map(beat8(bpm) + phaseOffset[band], 0, 255, 0, layerV->size.y-1); // saw wave, running over the y-axis, speed is determined by bpm
        } else if (method == 2) { //bandphases
          // Time-based phase increment - 1 complete cycle per second at 60 BPM
          uint32_t phaseIncrement = (bpm * deltaMs * 65536UL) / (60UL * 1000UL);
          phaseIncrement /= 2; // Make it 8x faster - adjust this multiplier as needed
          bandPhase[band] += phaseIncrement;
          y = ::map(bandPhase[band] >> 8, 0, 255, 0, layerV->size.y-1); // saw wave, running over the y-axis, speed is determined by bpm
        }
        //y-axis shows a saw wave which runs faster if the bandSpeed for the x-column is higher, if rings are used for the y-axis, it will show as turning wheels
        layerV->setRGB(Coord3D(x, (invert && x%2==0)?layerV->size.y - 1 - y:y), ColorFromPalette(layerV->layerP->palette, ::map(x, 0, layerV->size.x-1, 0, 255)));
      }
    }
  }
}; //FreqSawsEffect






// Written by Ewoud Wijma in 2022, inspired by https://natureofcode.com/book/chapter-7-cellular-automata/ and https://github.com/DougHaber/nlife-color ,
// Modified By: Brandon Butler / @Brandon502 / wildcats08 in 2024
// todo: ewowi check with wildcats08: can background color be removed as it is now easy to add solid as background color (blending ...?)
class GameOfLifeEffect: public Node {
  public:
  static const char * name() {return "Game Of Life 🔥💫🎨🧊";}
  static uint8_t dim() {return _3D;} //supports 3D but also 2D (1D as well?)
  static const char * tags() {return "";}

  void placePentomino(uint8_t  *futureCells, bool colorByAge) {
    uint8_t  pattern[5][2] = {{1, 0}, {0, 1}, {1, 1}, {2, 1}, {2, 2}}; // R-pentomino
    if (!random8(5)) pattern[0][1] = 3; // 1/5 chance to use glider
    CRGB color = ColorFromPalette(layerV->layerP->palette, random8());
    for (int attempts = 0; attempts < 100; attempts++) {
      int x = random8(1, layerV->size.x - 3);
      int y = random8(1, layerV->size.y - 5);
      int z = random8(2) * (layerV->size.z - 1);
      bool canPlace = true;
      for (int i = 0; i < 5; i++) {
        int nx = x + pattern[i][0];
        int ny = y + pattern[i][1];
        if (getBitValue(futureCells, layerV->XYZUnModified(Coord3D(nx, ny, z)))) {canPlace = false; break;}
      }
      if (canPlace || attempts == 99) {
        for (int i = 0; i < 5; i++) {
          int nx = x + pattern[i][0];
          int ny = y + pattern[i][1];
          setBitValue(futureCells, layerV->XYZUnModified(Coord3D(nx, ny, z)), true);
          layerV->setRGB(Coord3D(nx, ny, z), colorByAge ? CRGB::Green : color);
        }
        return;
      }
    }
  }

  Coord3D bgC                  = {0,0,0};
  uint8_t ruleset              = 1;
  char    customRuleString[20] = "B/S";
  uint8_t speed                = 20;
  uint8_t lifeChance           = 32;
  uint8_t mutation             = 2;
  bool    wrap                 = true;
  bool    disablePause         = false;
  bool    colorByAge           = false;
  bool    infinite             = false;
  uint8_t blur                 = 128;

  void setBirthAndSurvive() {
      String ruleString = "";
      if      (ruleset == 0) ruleString = customRuleString; //Custom
      else if (ruleset == 1) ruleString = "B3/S23";         //Conway's Game of Life
      else if (ruleset == 2) ruleString = "B36/S23";        //HighLife
      else if (ruleset == 3) ruleString = "B0123478/S34678";//InverseLife
      else if (ruleset == 4) ruleString = "B3/S12345";      //Maze
      else if (ruleset == 5) ruleString = "B3/S1234";       //Mazecentric
      else if (ruleset == 6) ruleString = "B367/S23";       //DrighLife

      memset(birthNumbers,   0, sizeof(bool) * 9);
      memset(surviveNumbers, 0, sizeof(bool) * 9);

      //Rule String Parsing
      int slashIndex = ruleString.indexOf('/');
      for (int i = 0; i < ruleString.length(); i++) {
        int num = ruleString.charAt(i) - '0';
        if (num >= 0 && num < 9) {
          if (i < slashIndex) birthNumbers[num] = true;
          else surviveNumbers[num] = true;
        }
      }
  }

  void setup() override {
    addControl(bgC, "backgroundColor", "coord3D", 0, 255);
    JsonObject property = addControl(ruleset, "ruleset", "select"); 
    JsonArray values = property["values"].to<JsonArray>();
    values.add("Custom B/S");
    values.add("Conway's Game of Life B3/S23");
    values.add("HighLife B36/S23");
    values.add("InverseLife B0123478/S34678");
    values.add("Maze B3/S12345");
    values.add("Mazecentric B3/S1234");
    values.add("DrighLife B367/S23");

    addControl(customRuleString, "customRuleString", "text", 0, 32);

    addControl(speed, "GameSpeed (FPS)", "range", 0, 100);
    addControl(lifeChance, "startingLifeDensity", "range", 10, 90);
    addControl(mutation, "mutationChance", "range", 0, 100);
    addControl(wrap, "wrap", "checkbox");
    addControl(disablePause, "disablePause", "checkbox");
    addControl(colorByAge, "colorByAge", "checkbox");
    addControl(infinite, "infinite", "checkbox");
    addControl(blur, "blur", "range", 0, 255);
  }

  void onUpdate(String &oldValue, JsonObject control) override {
    Node::onUpdate(oldValue, control);

    if (control["name"] == "ruleset" || control["name"] == "customRuleString") {
      setBirthAndSurvive();
    }
  }

  unsigned long step;
  uint16_t gliderLength;
  uint16_t cubeGliderLength;
  uint16_t oscillatorCRC;
  uint16_t spaceshipCRC;
  uint16_t cubeGliderCRC;
  bool     soloGlider;
  uint16_t generation;
  bool     birthNumbers[9];
  bool     surviveNumbers[9];
  CRGB     prevPalette;
  uint8_t  *cells = nullptr;
  uint8_t  *futureCells = nullptr;
  uint8_t  *cellColors = nullptr;

  void startNewGameOfLife() {
    // MB_LOGD(ML_TAG, "startNewGameOfLife");
    prevPalette  = ColorFromPalette(layerV->layerP->palette, 0);
    generation = 1;
    disablePause ? step = millis() : step = millis() + 1500;

    // Setup Grid
    memset(cells, 0, dataSize);
    memset(cellColors, 0, layerV->size.x * layerV->size.y * layerV->size.z);

    for (int x = 0; x < layerV->size.x; x++) for (int y = 0; y < layerV->size.y; y++) for (int z = 0; z < layerV->size.z; z++){
      if (layerV->layerDimension == _3D && !layerV->isMapped(layerV->XYZUnModified(Coord3D(x,y,z)))) continue;
      if (random8(100) < lifeChance) {
        int index = layerV->XYZUnModified(Coord3D(x,y,z));
        setBitValue(cells, index, true);
        cellColors[index] = random8(1, 255);
        layerV->setRGB(Coord3D(x,y,z), colorByAge ? CRGB::Green : ColorFromPalette(layerV->layerP->palette, cellColors[index]));
        // layerV->setRGB(Coord3D(x,y,z), bgColor); // Color set in redraw loop
      }
    }
    memcpy(futureCells, cells, dataSize);

    soloGlider = false;
    // Change CRCs
    uint16_t crc = crc16((const unsigned char*)cells, dataSize);
    oscillatorCRC = crc, spaceshipCRC = crc, cubeGliderCRC = crc;
    gliderLength  = lcm(layerV->size.y, layerV->size.x) * 4;
    cubeGliderLength = gliderLength * 6; // Change later for rectangular cuboid
  }

  int dataSize = 0;

  ~GameOfLifeEffect() {
    freeMB(cells);
    freeMB(futureCells);
    freeMB(cellColors);

    //destructor
    Node::~Node();
  }

  void onSizeChanged(Coord3D prevSize) override {
    dataSize = ((layerV->size.x * layerV->size.y * layerV->size.z + 7) / 8);

    freeMB(cells);
    freeMB(futureCells);
    freeMB(cellColors);

    cells = allocMB<uint8_t>(dataSize);
    futureCells = allocMB<uint8_t>(dataSize);
    cellColors = allocMB<uint8_t>(layerV->size.x * layerV->size.y * layerV->size.z);

    if (!cells || !futureCells || !cellColors) {
        MB_LOGE(ML_TAG, "allocation of cells || !futureCells || !cellColors failed");
    }

    startNewGameOfLife();
  }

  void loop() override {

    if (!cells || !futureCells || !cellColors) return;

    if (generation == 0 && step < millis()) {
      // MB_LOGD(ML_TAG, "gen / step");
      startNewGameOfLife();
      return; // show the start
    }

    CRGB bgColor = CRGB(bgC.x, bgC.y, bgC.z);
    CRGB color   = ColorFromPalette(layerV->layerP->palette, random8()); // Used if all parents died

    int fadedBackground = 0;
    if (blur > 220 && !colorByAge) { // Keep faded background if blur > 220
      fadedBackground = bgColor.r + bgColor.g + bgColor.b + 20 + (blur-220);
      blur -= (blur-220);
    }
    bool blurDead = step > millis() && !fadedBackground;
    // Redraw Loop
    if (generation <= 1 || blurDead) { // Readd overlay support when implemented
      for (int x = 0; x < layerV->size.x; x++) for (int y = 0; y < layerV->size.y; y++) for (int z = 0; z < layerV->size.z; z++){
        uint16_t cIndex = layerV->XYZUnModified(Coord3D(x,y,z)); // Current cell index (bit grid lookup)
        Coord3D cLocPos = Coord3D(x,y,z);
        uint16_t cLoc   = layerV->XYZ(cLocPos);            // Current cell location (led index)
        if (!layerV->isMapped(cIndex)) continue;
        bool alive = getBitValue(cells, cIndex);
        bool recolor = (alive && generation == 1 && cellColors[cIndex] == 0 && !random(16)); // Palette change or Initial Color
        // Redraw alive if palette changed, spawn initial colors randomly, age alive cells while paused
        if (alive && recolor) {
          cellColors[cIndex] = random8(1, 255);
          layerV->setRGB(cLoc, colorByAge ? CRGB::Green : ColorFromPalette(layerV->layerP->palette, cellColors[cIndex]));
        }
        else if (alive && colorByAge && !generation) layerV->blendColor(cLoc, CRGB::Red, 248); // Age alive cells while paused
        else if (alive && cellColors[cIndex] != 0) layerV->setRGB(cLoc, colorByAge ? CRGB::Green : ColorFromPalette(layerV->layerP->palette, cellColors[cIndex]));
        // Redraw dead if palette changed, blur paused game, fade on newgame
        // if      (!alive && (paletteChanged || disablePause)) layerV->setRGB(cLoc, bgColor);   // Remove blended dead cells
        else if (!alive && blurDead)         layerV->blendColor(cLoc, bgColor, blur);           // Blend dead cells while paused
        else if (!alive && generation == 1) layerV->blendColor(cLoc, bgColor, 248);            // Fade dead on new game
      }
    }

    // if (!speed || step > millis() || millis() - step < 1000 / speed) return; // Check if enough time has passed for updating
    if (!speed || step > millis() || (speed != 100 && millis() - step < 1000 / speed)) return; // Uncapped speed when slider maxed

    //Update Game of Life
    int aliveCount = 0, deadCount = 0; // Detect solo gliders and dead grids
    const int zAxis  = (layerV->layerDimension == _3D) ? 1 : 0; // Avoids looping through z axis neighbors if 2D
    bool disableWrap = !wrap || soloGlider || generation % 1500 == 0 || zAxis;
    //Loop through all cells. Count neighbors, apply rules, setPixel
    for (int x = 0; x < layerV->size.x; x++) for (int y = 0; y < layerV->size.y; y++) for (int z = 0; z < layerV->size.z; z++){
      Coord3D  cPos      = Coord3D(x, y, z);
      uint16_t cIndex    = layerV->XYZUnModified(cPos);
      bool     cellValue = getBitValue(cells, cIndex);
      if (cellValue) aliveCount++; else deadCount++;
      if (zAxis && !layerV->isMapped(cIndex)) continue; // Skip if not physical led on 3D fixtures
      uint8_t  neighbors = 0, colorCount = 0;
      uint8_t  nColors[9];

      for (int i = -1; i <= 1; i++) for (int j = -1; j <= 1; j++) for (int k = -zAxis; k <= zAxis; k++) {
        if (i==0 && j==0 && k==0) continue; // Ignore itself
        Coord3D nPos = Coord3D(x+i, y+j, z+k);
        if (nPos.isOutofBounds(layerV->size)) {
          // Wrap is disabled when unchecked, for 3D fixtures, every 1500 generations, and solo gliders
          if (disableWrap) continue;
          nPos = (nPos + layerV->size) % layerV->size; // Wrap around 3D
        }
        uint16_t nIndex = layerV->XYZUnModified(nPos);
        // Count neighbors and store up to 9 neighbor colors
        if (getBitValue(cells, nIndex)) {
          neighbors++;
          if (cellValue || colorByAge) continue; // Skip if cell is alive (color already set) or color by age (colors are not used)
          if (cellColors[nIndex] == 0) continue; // Skip if neighbor color is 0 (dead cell)
          nColors[colorCount % 9] = cellColors[nIndex];
          colorCount++;
        }
      }
      // Rules of Life
      if (cellValue && !surviveNumbers[neighbors]) {
        // Loneliness or Overpopulation
        setBitValue(futureCells, cIndex, false);
        layerV->blendColor(cPos, bgColor, blur);
      }
      else if (!cellValue && birthNumbers[neighbors]){
        // Reproduction
        setBitValue(futureCells, cIndex, true);
        uint8_t  colorIndex = nColors[random8(colorCount)];
        if (random8(100) < mutation) colorIndex = random8();
        cellColors[cIndex] = colorIndex;
        layerV->setRGB(cPos, colorByAge ? CRGB::Green : ColorFromPalette(layerV->layerP->palette, colorIndex));
      }
      else {
        // Blending, fade dead cells further causing blurring effect to moving cells
        if (!cellValue) {
          if (fadedBackground) {
              CRGB val = layerV->getRGB(cPos);
              if (fadedBackground < val.r + val.g + val.b) layerV->blendColor(cPos, bgColor, blur);
          }
          else layerV->blendColor(cPos, bgColor, blur);
        }
        else { // alive
          if (colorByAge) layerV->blendColor(cPos, CRGB::Red, 248);
          else layerV->setRGB(cPos, ColorFromPalette(layerV->layerP->palette, cellColors[cIndex]));
        }
      }
    }

    if (aliveCount == 5) soloGlider = true; else soloGlider = false;
    memcpy(cells, futureCells, dataSize);
    uint16_t crc = crc16((const unsigned char*)cells, dataSize);

    bool repetition = false;
    if (!aliveCount || crc == oscillatorCRC || crc == spaceshipCRC || crc == cubeGliderCRC) repetition = true;
    if ((repetition && infinite) || (infinite && !random8(50)) || (infinite && float(aliveCount)/(aliveCount + deadCount) < 0.05)) {
      placePentomino(futureCells, colorByAge); // Place R-pentomino/Glider if infinite mode is enabled
      memcpy(cells, futureCells, dataSize);
      repetition = false;
    }
    if (repetition) {
      generation = 0;
      disablePause ? step = millis() : step = millis() + 1000;
      return;
    }
    // Update CRC values
    if (generation % 16 == 0) oscillatorCRC = crc;
    if (gliderLength     && generation % gliderLength     == 0) spaceshipCRC = crc;
    if (cubeGliderLength && generation % cubeGliderLength == 0) cubeGliderCRC = crc;
    (generation)++;
    step = millis();
  }
}; //GameOfLife

// original version from SR 0.13, with some enhancements by @softhack007
// Blurz. By Andrew Tuline.
// Hint: Looks best with segment brightness set to max (use global brightness to reduce brightness)
// even with 1D effect we have to take logic for 2D segments for allocation as fill_solid() fills whole segment
#define MAX_FREQ_LOG10  4.04238f       // log10(MAX_FREQUENCY)
class BlurzEffect: public Node {
  public:
  static const char * name() {return "Blurz 🔥🎵🎨☾";}
  static uint8_t dim() {return _3D;}
  static const char * tags() {return "";}
  
  // static const char _data_FX_MODE_BLURZ[] PROGMEM = "Blurz Plus ☾@Fade rate,Blur,,,,FreqMap ☾,GEQ Scanner ☾,;!,Color mix;!;01f;sx=48,ix=127,m12=7,si=0"; // Pinwheel, Beatsin

  uint16_t aux0 = 0;
  uint16_t  aux1 = 65535; // last pixel postion. 65535 = none
  CRGB  step = {0,0,0};  // last pixel color
  uint32_t  call = 0;  // last pixel color

  uint8_t fadeRate = 48;
  uint8_t blur = 127;
  bool freqMap;
  bool geqScanner;

  void setup() override {
    addControl(fadeRate, "fadeRate", "range", 1, 255);
    addControl(blur, "blur", "range", 1, 255);
    addControl(freqMap, "freqMap", "checkbox");
    addControl(geqScanner, "geqScanner", "checkbox");

    layerV->fadeToBlackBy(255);
  }

  void loop() override {
    random16_add_entropy(esp_random() & 0xFFFF); // improves randonmess

    int fadeoutDelay = (256 - fadeRate) / 24;
    if ((fadeoutDelay <= 1 ) || ((call % fadeoutDelay) == 0))
        layerV->fadeToBlackBy(fadeRate);
    else {
      layerV->blur2d(8 + blur/8 + fadeoutDelay*4);
    }
    if ((aux1 < layerV->size.x * layerV->size.y * layerV->size.z) && (sharedData.volume > 1.0f)) layerV->setRGB(aux1, step); // "repaint" last pixel after blur

    unsigned freqBand = aux0 % 16;
    uint16_t segLoc = random16(layerV->size.x * layerV->size.y * layerV->size.z);

    if (freqMap) {                                                                                   // FreqMap mode : blob location by major frequency
      int freqLocn;
      unsigned maxLen = (geqScanner) ? MAX(1, layerV->size.x * layerV->size.y * layerV->size.z-16): layerV->size.x * layerV->size.y * layerV->size.z;                                       // usable segment length - leave 16 pixels when embedding "GEQ scan"
      freqLocn = roundf((log10f((float)sharedData.majorPeak) - 1.78f) * float(maxLen)/(MAX_FREQ_LOG10 - 1.78f));  // log10 frequency range is from 1.78 to 3.71. Let's scale to layerV->size.x * layerV->size.y * layerV->size.z. // WLEDMM proper rounding
      if (freqLocn < 1) freqLocn = 0; // avoid underflow
      segLoc =  (geqScanner) ? freqLocn + freqBand : freqLocn;
    } else if (geqScanner) {                                                                            // GEQ Scanner mode: blob location is defined by frequency band + random offset
      float bandWidth = float(layerV->size.x * layerV->size.y * layerV->size.z)  / 16.0f;
      int bandStart = roundf(bandWidth * freqBand);
      segLoc = bandStart + random16(max(1, int(bandWidth)));
    }
    segLoc = MAX(uint16_t(0), MIN(uint16_t(layerV->size.x * layerV->size.y * layerV->size.z-1), segLoc));  // fix overflows

    if (layerV->size.x * layerV->size.y * layerV->size.z < 2) segLoc = 0; // WLEDMM just to be sure
    unsigned pixColor = (2*sharedData.bands[freqBand]*240)/max(1, layerV->size.x * layerV->size.y * layerV->size.z-1);                  // WLEDMM avoid uint8 overflow, and preserve pixel parameters for redraw
    unsigned pixIntensity = min((unsigned)(2.0f*sharedData.bands[freqBand]), 255U);

    if (sharedData.volume > 1.0f) {
      layerV->setRGB(segLoc, ColorFromPalette(layerV->layerP->palette, pixColor));
      step = layerV->getRGB(segLoc);  // remember last color
      aux1 = segLoc;                         // remember last position

      layerV->blur2d(blur);
      aux0 ++;
      aux0 %= 16; // make sure it doesn't cross 16
      layerV->addRGB(segLoc,  ColorFromPalette(layerV->layerP->palette, pixColor)); // repaint center pixel after blur
    } else layerV->blur2d(blur);  // silence - just blur it again
    call++;
  } //loop
}; // BlurzEffect


//by WildCats08 / @Brandon502
class RubiksCubeEffect: public Node {
  public:
  static const char * name() {return "Rubik's Cube 🔥💫🧊";}
  static uint8_t     dim() {return _3D;}
  static const char * tags() {return "💫";}

  struct Cube {
      uint8_t SIZE;
      static const uint8_t MAX_SIZE = 8;
      using Face = std::array<std::array<uint8_t, MAX_SIZE>, MAX_SIZE>;
      Face front;
      Face back;
      Face left;
      Face right;
      Face top;
      Face bottom;

      Cube() {
        init(SIZE);
      }
      
      void init(uint8_t cubeSize) {
        SIZE = cubeSize;
        for (int i = 0; i < MAX_SIZE; i++) for (int j = 0; j < MAX_SIZE; j++) {
          front[i][j]  = 0;
          back[i][j]   = 1;  
          left[i][j]   = 2;  
          right[i][j]  = 3; 
          top[i][j]    = 4;   
          bottom[i][j] = 5;
        }
      }

      void rotateFace(Face& face, bool clockwise) {
        Face temp = face;
        if (clockwise) for (int i = 0; i < SIZE; i++) for (int j = 0; j < SIZE; j++) {
          face[j][SIZE - 1 - i] = temp[i][j]; 
        }  
        else for (int i = 0; i < SIZE; i++) for (int j = 0; j < SIZE; j++) {
          face[SIZE - 1 - j][i] = temp[i][j];
        }
      }

      void rotateRow(int startRow, int stopRow, bool clockwise) {
        std::array<uint8_t, MAX_SIZE> temp;
        for (int row = startRow; row <= stopRow; row++) {
          if (clockwise) for (int i = 0; i < SIZE; i++) {
            temp[i]       = left[row][i];
            left[row][i]  = front[row][i];
            front[row][i] = right[row][i];
            right[row][i] = back[row][i];
            back[row][i]  = temp[i];
          } 
          else for (int i = 0; i < SIZE; i++) {
            temp[i]       = left[row][i];
            left[row][i]  = back[row][i];
            back[row][i]  = right[row][i];
            right[row][i] = front[row][i];
            front[row][i] = temp[i];
          }
        }
      }

      void rotateColumn(int startCol, int stopCol, bool clockwise) {
        std::array<uint8_t, MAX_SIZE> temp;
        for (int col = startCol; col <= stopCol; col++) {
          if (clockwise) for (int i = 0; i < SIZE; i++) {
            temp[i]        = top[i][col];
            top[i][col]    = front[i][col];
            front[i][col]  = bottom[i][col];
            bottom[i][col] = back[SIZE - 1 - i][SIZE - 1 - col];
            back[SIZE - 1 - i][SIZE - 1 - col] = temp[i];   
          }    
          else for (int i = 0; i < SIZE; i++) {
            temp[i]        = top[i][col];
            top[i][col]    = back[SIZE - 1 - i][SIZE - 1 - col];
            back[SIZE - 1 - i][SIZE - 1 - col] = bottom[i][col];
            bottom[i][col] = front[i][col];
            front[i][col]  = temp[i];
          }
        }
      }

      void rotateFaceLayer(bool clockwise, int startLayer, int endLayer) {
        for (int layer = startLayer; layer <= endLayer; layer++) {
          std::array<uint8_t, MAX_SIZE> temp;
          for (int i = 0; i < SIZE; i++) temp[i] = clockwise ? top[SIZE - 1 - layer][i] : bottom[layer][i];
          for (int i = 0; i < SIZE; i++) {
            if (clockwise) {
              top[SIZE - 1 - layer][i] = left[SIZE - 1 - i][SIZE - 1 - layer];
              left[SIZE - 1 - i][SIZE - 1 - layer] = bottom[layer][SIZE - 1 - i];
              bottom[layer][SIZE - 1 - i] = right[i][layer];
              right[i][layer] = temp[i];
            } else {
              bottom[layer][SIZE - 1 - i] = left[SIZE - 1 - i][SIZE - 1 - layer];
              left[SIZE - 1 - i][SIZE - 1 - layer] = top[SIZE - 1 - layer][i];
              top[SIZE - 1 - layer][i] = right[i][layer];
              right[i][layer] = temp[SIZE - 1 - i];
            }
          }
        }
      }

      void rotateFront(bool clockwise, uint8_t width) {
        rotateFaceLayer(clockwise, 0, width - 1);
        rotateFace(front, clockwise);
        if (width >= SIZE) rotateFace(back, !clockwise);
      }
      void rotateBack(bool clockwise, uint8_t width) {
        rotateFaceLayer(!clockwise, SIZE - width, SIZE - 1);
        rotateFace(back, clockwise);
        if (width >= SIZE) rotateFace(front, !clockwise);
      }
      void rotateLeft(bool clockwise, uint8_t width) {
        rotateFace(left, clockwise);
        rotateColumn(0, width - 1, !clockwise);
        if (width >= SIZE) rotateFace(right, !clockwise);
      }
      void rotateRight(bool clockwise, uint8_t width) {
        rotateFace(right, clockwise);
        rotateColumn(SIZE - width, SIZE - 1, clockwise);
        if (width >= SIZE) rotateFace(left, !clockwise);
      }
      void rotateTop(bool clockwise, uint8_t width) {
        rotateFace(top, clockwise);
        rotateRow(0, width - 1, clockwise);
        if (width >= SIZE) rotateFace(bottom, !clockwise);
      }
      void rotateBottom(bool clockwise, uint8_t width) {
        rotateFace(bottom, clockwise);
        rotateRow(SIZE - width, SIZE - 1, !clockwise);
        if (width >= SIZE) rotateFace(top, !clockwise);
      }

      void drawCube(VirtualLayer *layerV) {
        int sizeX = MAX(layerV->size.x-1, 1);
        int sizeY = MAX(layerV->size.y-1, 1);
        int sizeZ = MAX(layerV->size.z-1, 1);

        // 3 Sided Cube Cheat add 1 to led size if "panels" missing. May affect different fixture types
        if (layerV->layerDimension == _3D) {
          if (!layerV->isMapped(layerV->XYZUnModified(Coord3D(0, layerV->size.y/2, layerV->size.z/2))) || !layerV->isMapped(layerV->XYZUnModified(Coord3D(layerV->size.x-1, layerV->size.y/2, layerV->size.z/2)))) sizeX++;
          if (!layerV->isMapped(layerV->XYZUnModified(Coord3D(layerV->size.x/2, 0, layerV->size.z/2))) || !layerV->isMapped(layerV->XYZUnModified(Coord3D(layerV->size.x/2, layerV->size.y-1, layerV->size.z/2)))) sizeY++;
          if (!layerV->isMapped(layerV->XYZUnModified(Coord3D(layerV->size.x/2, layerV->size.y/2, 0))) || !layerV->isMapped(layerV->XYZUnModified(Coord3D(layerV->size.x/2, layerV->size.y/2, layerV->size.z-1)))) sizeZ++;
        }

        // Previously SIZE - 1. Cube size expanded by 2, makes edges thicker. Constrains are used to prevent out of bounds
        const float scaleX = (SIZE + 1.0) / sizeX;
        const float scaleY = (SIZE + 1.0) / sizeY;
        const float scaleZ = (SIZE + 1.0) / sizeZ;

        // Calculate once for optimization
        const int halfX = sizeX / 2;
        const int halfY = sizeY / 2;
        const int halfZ = sizeZ / 2;

        const CRGB COLOR_MAP[] = {CRGB::Red, CRGB::DarkOrange, CRGB::Blue, CRGB::Green, CRGB::Yellow, CRGB::White};
        
        for (int x = 0; x < layerV->size.x; x++) for (int y = 0; y < layerV->size.y; y++) for (int z = 0; z < layerV->size.z; z++) { 
          Coord3D led = Coord3D(x, y, z);
          if (layerV->isMapped(layerV->XYZUnModified(led)) == 0) continue; // skip if not a physical LED

          // Normalize the coordinates to the Rubik's cube range. Subtract 1 since cube expanded by 2
          int normalizedX = constrain(round(x * scaleX) - 1, 0, SIZE - 1);
          int normalizedY = constrain(round(y * scaleY) - 1, 0, SIZE - 1);
          int normalizedZ = constrain(round(z * scaleZ) - 1, 0, SIZE - 1);
          
          // Calculate the distance to the closest face
          int distX = min(x, sizeX - x);
          int distY = min(y, sizeY - y);
          int distZ = min(z, sizeZ - z);
          int dist  = min(distX, min(distY, distZ));

          if      (dist == distZ && z < halfZ)  layerV->setRGB(led, COLOR_MAP[front[normalizedY][normalizedX]]);
          else if (dist == distX && x < halfX)  layerV->setRGB(led, COLOR_MAP[left[normalizedY][SIZE - 1 - normalizedZ]]);
          else if (dist == distY && y < halfY)  layerV->setRGB(led, COLOR_MAP[top[SIZE - 1 - normalizedZ][normalizedX]]);
          else if (dist == distZ && z >= halfZ) layerV->setRGB(led, COLOR_MAP[back[normalizedY][SIZE - 1 - normalizedX]]);
          else if (dist == distX && x >= halfX) layerV->setRGB(led, COLOR_MAP[right[normalizedY][normalizedZ]]);
          else if (dist == distY && y >= halfY) layerV->setRGB(led, COLOR_MAP[bottom[normalizedZ][normalizedX]]);
        }
      }
  };

  struct Move {
      uint8_t face;      // 0-5 (3 bits)
      uint8_t width;     // 0-7 (3 bits)
      uint8_t direction; // 0 or 1 (1 bit)
  };

  Move createRandomMoveStruct(uint8_t cubeSize, uint8_t prevFace) {
      Move move;
      do {
        move.face = random(6);
      } while (move.face/2 == prevFace/2);
      move.width     = random(cubeSize-2);
      move.direction = random(2);
      return move;
  }

  uint8_t packMove(Move move) {
      uint8_t packed = (move.face & 0b00000111) | 
                      ((move.width << 3) & 0b00111000) | 
                      ((move.direction << 6) & 0b01000000);
      return packed;
  }

  Move unpackMove(uint8_t packedMove) {
      Move move;
      move.face      = packedMove & 0b00000111;
      move.width     = (packedMove >> 3) & 0b00000111;
      move.direction = (packedMove >> 6) & 0b00000001;
      return move;
  }

  // UI control variables
  uint8_t turnsPerSecond = 2;
  uint8_t cubeSize = 3;
  bool randomTurning = false;

  void setup() override {
    addControl(turnsPerSecond, "turnsPerSecond", "range", 0, 20);   
    addControl(cubeSize, "cubeSize", "range", 1, 8);   
    addControl(randomTurning, "randomTurning", "checkbox");   
  }

  bool doInit = false;
  void onUpdate(String &oldValue, JsonObject control) override {
    Node::onUpdate(oldValue, control);

    if (control["name"] == "cubeSize" || control["name"] == "randomTurning") {
      doInit = true;
    }
  }

  unsigned long step;
  Cube    cube;
  uint8_t moveList[100];
  uint8_t moveIndex;
  uint8_t prevFaceMoved;

  void init() {
      cube.init(cubeSize);
      uint8_t moveCount = cubeSize * 10 + random(20);
      // Randomly turn entire cube
      for (int x = 0; x < 3; x++) {
        if (random(2)) cube.rotateRight(1, cubeSize);
        if (random(2)) cube.rotateTop  (1, cubeSize);
        if (random(2)) cube.rotateFront(1, cubeSize);
      }
      // Generate scramble
      for (int i = 0; i < moveCount; i++) {
        Move move = createRandomMoveStruct(cubeSize, prevFaceMoved);
        prevFaceMoved = move.face;
        moveList[i] = packMove(move);

        (cube.*rotateFuncs[move.face])(move.direction, move.width + 1);
      }

      moveIndex = moveCount - 1;

      cube.drawCube(layerV);

  }

  typedef void (Cube::*RotateFunc)(bool direction, uint8_t width);
  RotateFunc rotateFuncs[6] = {&Cube::rotateFront, &Cube::rotateBack, &Cube::rotateLeft, &Cube::rotateRight, &Cube::rotateTop, &Cube::rotateBottom};

  void loop() override {
    
    if (doInit && millis() > step || step - 3100 > millis()) { // step - 3100 > millis() temp fix for default on boot
      step = millis() + 1000;
      doInit = false;
      init();
    }

    if (!turnsPerSecond || millis() - step < 1000 / turnsPerSecond || millis() < step) return;

    Move move = randomTurning ? createRandomMoveStruct(cubeSize, prevFaceMoved) : unpackMove(moveList[moveIndex]);

    (cube.*rotateFuncs[move.face])(!move.direction, move.width + 1);
      
    cube.drawCube(layerV);
    
    if (!randomTurning && moveIndex == 0) {
      step = millis() + 3000;
      doInit = true;
      return;
    }
    if (!randomTurning) moveIndex--;
    step = millis();
  }
};

//by WildCats08 / @Brandon502
class ParticlesEffect: public Node {
  public:
  static const char * name() {return "Particles 🔥💫🧭🎨🧊";}
  static uint8_t      dim() {return _3D;}
  static const char * tags() {return "";}
  
  struct Particle {
    float x, y, z;
    float vx, vy, vz;
    CRGB color;

    void update() {
      x += vx;
      y += vy;
      z += vz;
    }
    void revert() {
      x -= vx;
      y -= vy;
      z -= vz;
    }

    Coord3D toCoord3DRounded() {
      return Coord3D(round(x), round(y), round(z));
    }

    void updatePositionandDraw(VirtualLayer *layerV, int particleIndex = 0, bool debugPrint = false) {
      if (debugPrint) MB_LOGD(ML_TAG, "Particle %d: Pos: %f, %f, %f Velocity: %f, %f, %f\n", particleIndex, x, y, z, vx, vy, vz);

      Coord3D prevPos = toCoord3DRounded();
      if (debugPrint) MB_LOGD(ML_TAG, "     PrevPos: %d, %d, %d\n", prevPos.x, prevPos.y, prevPos.z);
      
      update();
      Coord3D newPos = toCoord3DRounded();
      if (debugPrint) MB_LOGD(ML_TAG, "     NewPos: %d, %d, %d\n", newPos.x, newPos.y, newPos.z);

      if (newPos == prevPos) return; // Skip if no change in position

      layerV->setRGB(prevPos, CRGB::Black); // Clear previous position

      if (layerV->isMapped(layerV->XYZUnModified(newPos)) && !newPos.isOutofBounds(layerV->size) && layerV->getRGB(newPos) == CRGB::Black) {
        if (debugPrint) MB_LOGD(ML_TAG, "     New Pos was mapped and particle placed\n");
        layerV->setRGB(newPos, color); // Set new position
        return;
      }
      
      // Particle is not mapped, find nearest mapped pixel
      Coord3D nearestMapped = prevPos;                          // Set nearest to previous position
      unsigned nearestDist = newPos.distanceSquared(prevPos);   // Set distance to previous position
      int diff = 0;                                             // If distance the same check how many coordinates are different (larger is better)
      bool changed = false;

      if (debugPrint) MB_LOGD(ML_TAG, "     %d, %d, %d, Not Mapped! Nearest: %d, %d, %d dist: %d diff: %d\n", newPos.x, newPos.y, newPos.z, nearestMapped.x, nearestMapped.y, nearestMapped.z, nearestDist, diff);
      
      // Check neighbors for nearest mapped pixel. This should be changed to check neighbors with similar velocity
      for (int i = -1; i <= 1; i++) for (int j = -1; j <= 1; j++) for (int k = -1; k <= 1; k++) {
        Coord3D testPos = newPos + Coord3D(i, j, k);
        if (testPos == prevPos)                         continue; // Skip current position
        if (!layerV->isMapped(layerV->XYZUnModified(testPos)))    continue; // Skip if not mapped
        if (testPos.isOutofBounds(layerV->size))           continue; // Skip out of bounds
        if (layerV->getRGB(testPos) != CRGB::Black) continue; // Skip if already colored by another particle
        unsigned dist = testPos.distanceSquared(newPos);
        int differences = (prevPos.x != testPos.x) + (prevPos.y != testPos.y) + (prevPos.z != testPos.z);
        if (debugPrint) MB_LOGD(ML_TAG, "     TestPos: %d %d %d Dist: %d Diff: %d", testPos.x, testPos.y, testPos.z, dist, differences);
        if (debugPrint) MB_LOGD(ML_TAG, "     New Velocities: %d, %d, %d\n", (testPos.x - prevPos.x), (testPos.y - prevPos.y), (testPos.z - prevPos.z));
        if (dist < nearestDist || (dist == nearestDist && differences >= diff)) {
          nearestDist = dist;
          nearestMapped = testPos;
          diff = differences;
          changed = true;
        }
      }
      if (changed) { // Change velocity to move towards nearest mapped pixel. Update position.
        if (newPos.x != nearestMapped.x) vx = constrain(nearestMapped.x - prevPos.x, -1, 1);
        if (newPos.y != nearestMapped.y) vy = constrain(nearestMapped.y - prevPos.y, -1, 1);
        if (newPos.z != nearestMapped.z) vz = constrain(nearestMapped.z - prevPos.z, -1, 1);

        x = nearestMapped.x; 
        y = nearestMapped.y; 
        z = nearestMapped.z;
        
        if (debugPrint) MB_LOGD(ML_TAG, "     New Position: %d, %d, %d New Velocity: %f, %f, %f\n", nearestMapped.x, nearestMapped.y, nearestMapped.z, vx, vy, vz);
      }
      else {
        // No valid position found, revert to previous position
        // Find which direction is causing OoB / not mapped and set velocity to 0
        Coord3D testing = toCoord3DRounded();
        revert();
        // change X val
        testing.x = newPos.x;
        if (testing.isOutofBounds(layerV->size) || !layerV->isMapped(layerV->XYZUnModified(testing))) vx = 0;
        // change Y val
        testing = toCoord3DRounded();
        testing.y = newPos.y;
        if (testing.isOutofBounds(layerV->size) || !layerV->isMapped(layerV->XYZUnModified(testing))) vy = 0;
        // change Z val
        testing = toCoord3DRounded();
        testing.z = newPos.z;
        if (testing.isOutofBounds(layerV->size) || !layerV->isMapped(layerV->XYZUnModified(testing))) vz = 0;
        
        if (debugPrint) MB_LOGD(ML_TAG, "     No valid position found, reverted. Velocity Updated\n");
        if (debugPrint) MB_LOGD(ML_TAG, "     New Pos: %f, %f, %f Velo: %f, %f, %f\n", x, y, z, vx, vy, vz);
      }

      layerV->setRGB(toCoord3DRounded(), color);
    }
  };

  uint8_t speed        = 15;
  uint8_t numParticles = 10;
  bool   barriers        = false;
  #ifdef STARBASE_USERMOD_MPU6050
    bool gyro = true;
  #else
    bool gyro = false;
  #endif
  bool randomGravity = true;
  uint8_t gravityChangeInterval = 5;
  // bool debugPrint    = layerV->effectData.read<bool>();
  bool debugPrint = false;

  void setup() override {
    addControl(speed, "speed", "range", 0, 30);
    addControl(numParticles, "number of Particles", "range", 1, 255);
    addControl(barriers, "barriers", "checkbox");
    #ifdef STARBASE_USERMOD_MPU6050
      addControl(gyro, "gyro", "checkbox");
    #endif
    addControl(randomGravity, "randomGravity", "checkbox");
    addControl(gravityChangeInterval, "gravityChangeInterval", "range", 1, 10);
    // addControl(bool, "Debug Print",             layerV->effectData.write<bool>(0));
  }

  void onUpdate(String &oldValue, JsonObject control) override {
    Node::onUpdate(oldValue, control);

    if (control["name"] == "number of Particles" || control["name"] == "barriers") {
      settingUpParticles();
    }
  }

  void settingUpParticles() {
    MB_LOGD(ML_TAG, "Setting Up Particles\n");
    layerV->fill_solid(CRGB::Black);

    if (barriers) {
      // create a 2 pixel thick barrier around middle y value with gaps
      for (int x = 0; x < layerV->size.x; x++) for (int z = 0; z < layerV->size.z; z++) {
        if (!random8(5)) continue;
        layerV->setRGB(Coord3D(x, layerV->size.y/2, z), CRGB::White);
        layerV->setRGB(Coord3D(x, layerV->size.y/2 - 1, z), CRGB::White);
      }
    }

    for (int index = 0 ; index < numParticles; index++) {
      Coord3D rPos; 
      int attempts = 0; 
      do { // Get random mapped position that isn't colored (infinite loop if small fixture size and high particle count)
        rPos = {random8(layerV->size.x), random8(layerV->size.y), random8(layerV->size.z)};
        attempts++;
      } while ((!layerV->isMapped(layerV->XYZUnModified(rPos)) || layerV->getRGB(rPos) != CRGB::Black) && attempts < 1000);
      // rPos = {1,1,0};
      particles[index].x = rPos.x;
      particles[index].y = rPos.y;
      particles[index].z = rPos.z;

      particles[index].vx = (random8() / 256.0f) * 2.0f - 1.0f;
      particles[index].vy = (random8() / 256.0f) * 2.0f - 1.0f;
      if (layerV->layerDimension == _3D) particles[index].vz = (random8() / 256.0f) * 2.0f - 1.0f;
      else particles[index].vz = 0;

      particles[index].color = ColorFromPalette(layerV->layerP->palette, random8());
      Coord3D initPos = particles[index].toCoord3DRounded();
      layerV->setRGB(initPos, particles[index].color);
    }
    MB_LOGD(ML_TAG, "Particles Set Up\n");
    step = millis();
  }

  Particle particles[255];
  unsigned long step;
  unsigned long gravUpdate = 0;
  float gravity[3];

  void loop() override {

    if (!speed || millis() - step < 1000 / speed) return; // Not enough time passed

    float gravityX, gravityY, gravityZ; // Gravity if using gyro or random gravity

    #ifdef STARBASE_USERMOD_MPU6050
    if (gyro) {
      gravity[0] = -mpu6050->gravityVector.x;
      gravity[1] =  mpu6050->gravityVector.z; // Swap Y and Z axis
      gravity[2] = -mpu6050->gravityVector.y;

      if (layerV->layerDimension == _2D) { // Swap back Y and Z axis set Z to 0
        gravity[1] = -gravity[2];
        gravity[2] = 0;
      }
    }
    #endif

    if (randomGravity) {
      if (millis() - gravUpdate > gravityChangeInterval * 1000) {
        gravUpdate = millis();
        float scale = 5.0f;
        // Generate Perlin noise values and scale them
        gravity[0] = (inoise8(step, 0, 0) / 128.0f - 1.0f) * scale;
        gravity[1] = (inoise8(0, step, 0) / 128.0f - 1.0f) * scale;
        gravity[2] = (inoise8(0, 0, step) / 128.0f - 1.0f) * scale;

        gravity[0] = constrain(gravity[0], -1.0f, 1.0f);
        gravity[1] = constrain(gravity[1], -1.0f, 1.0f);
        gravity[2] = constrain(gravity[2], -1.0f, 1.0f);

        if (layerV->layerDimension == _2D) gravity[2] = 0;
        // MB_LOGD(ML_TAG, "Random Gravity: %f, %f, %f\n", gravity[0], gravity[1], gravity[2]);
      }
    }

    for (int index = 0; index < numParticles; index++) {
      if (gyro || randomGravity) { // Lerp gravity towards gyro or random gravity if enabled
        float lerpFactor = .75;
        particles[index].vx += (gravity[0] - particles[index].vx) * lerpFactor;
        particles[index].vy += (gravity[1] - particles[index].vy) * lerpFactor; // Swap Y and Z axis
        particles[index].vz += (gravity[2] - particles[index].vz) * lerpFactor;
      }
      particles[index].updatePositionandDraw(layerV, index, debugPrint);  
    }

    step = millis();
  }
};

#if USE_M5UNIFIED

class MoonManEffect: public Node {
  public:
  static const char * name() {return "Moon Man 🔥🎵🎨☾";}
  static uint8_t dim() {return _3D;}
  static const char * tags() {return "";}
  
  // Create an M5Canvas for PNG processing
  M5Canvas *canvas;//(&M5.Display);

  void setup() override {
    canvas = new M5Canvas(&M5.Display);
  }

  void onSizeChanged(Coord3D prevSize) override {
    // Create canvas for processing
    canvas->deleteSprite();
    canvas->createSprite(layerV->size.x, layerV->size.y);
    // Load and display PNG
    displayPNGToPanel();
  }

  bool success = false;

  void displayPNGToPanel() {
    // Method 1: Direct decode to canvas (if PNG fits in memory)
    canvas->fillSprite(TFT_BLACK);
    
    // Draw PNG to canvas - M5GFX handles scaling automatically
    success = canvas->drawPng(moonmanpng, moonmanpng_len, 0, 0, 0, 0, 0, 0, layerV->size.x/320.0, layerV->size.y/320.0);
    if (success) {
      Serial.println("PNG decoded successfully!");
      
      // Transfer canvas to LED panel
      transferCanvasToPanel();
    } else {
      Serial.println("PNG decode failed!");
    }
  }

  void transferCanvasToPanel() {
    // Read each pixel from canvas and send to LED panel
    for (int y = 0; y < layerV->size.y; y++) {
      for (int x = 0; x < layerV->size.x; x++) {
        // Get pixel color from canvas
        uint16_t color = canvas->readPixel(x, y);
        
        // Convert RGB565 to RGB888 for LED panel
        uint8_t r = ((color >> 11) & 0x1F) << 3;  // 5 bits -> 8 bits
        uint8_t g = ((color >> 5) & 0x3F) << 2;   // 6 bits -> 8 bits  
        uint8_t b = (color & 0x1F) << 3;          // 5 bits -> 8 bits
        
        // Set pixel on LED panel
        layerV->setRGB(Coord3D(x,y), CRGB(r,g,b));
      }
    }
  }

  void loop() override {

    if (success)
      // Transfer canvas to LED panel
      transferCanvasToPanel();
  } //loop

}; // MoonManEffect


#endif





// // By: Stepko https://editor.soulmatelights.com/gallery/1012 , Modified by: Andrew Tuline
// class BlackHoleEffect: public Node {
//   static const char * name() {return "BlackHole";}
//   static uint8_t dim() {return _2D;}
//   static const char * tags() {return "💡";}
  
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
//   const char * tags() override {return "💡💫";}
  
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
//   const char * tags() override {return "💡";}

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
//   const char * tags() override {return "💡";}

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
//   const char * tags() override {return "💡";}

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
//   const char * tags() override {return "♫💡💫";}

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
//   const char * tags() override {return "💡";} //💡 means wled origin
  
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
//   const char * tags() override {return "💡";}
  
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
//   const char * tags() override {return "💡💫";}
  
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
//   const char * tags() override {return "💡💫♥";}
  
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
//   const char * tags() override {return "♫💡";}

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

//     uint32_t color = SEGMENT. ColorFromPalette(layerV->layerP->palette, spotlights[i].colorIdx);
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
//       layerV->setRGB(i, c.red, c.green, c.blue);
//     } else if (deltabright > 0) {
//       // If the new pixel is just slightly brighter than the background color,
//       // mix a blend of the new color and the background color
//       layerV->setRGB(i, color_blend(RGBW32(bg.r,bg.g,bg.b,0), RGBW32(c.r,c.g,c.b,0), deltabright * 8));
//     } else {
//       // if the new pixel is not at all brighter than the background color,
//       // just use the background color.
//       layerV->setRGB(i, bg.r, bg.g, bg.b);
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