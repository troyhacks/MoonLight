/**
    @title     MoonLight
    @file      E_MovingHeads.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/moonlight/overview/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact us for more information.
**/


#if FT_MOONLIGHT

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


#endif