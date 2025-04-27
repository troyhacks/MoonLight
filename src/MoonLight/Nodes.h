/**
    @title     MoonLight
    @file      Nodes.h
    @repo      https://github.com/ewowi/MoonBase, submit changes to this file as PRs
    @Authors   https://github.com/ewowi/MoonBase/commits/main
    @Doc       https://ewowi.github.io/MoonBase/general/utilities/
    @Copyright © 2025 Github MoonBase Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
**/

#pragma once

#define _1D 1
#define _2D 2
#define _3D 3

#include "VirtualLayer.h" //VirtualLayer.h will include PhysicalLayer.h

#include <ESPFS.h>

class Node {
public:
  VirtualLayer *layerV = nullptr; //the virtual layer this effect is using
  const char *animation;

  bool hasSetup = false;
  bool hasLoop = false;
  bool hasModifier = false;
  bool hasFixDef = false; //Mapper?

  bool on = true;

  //C++ constructor and destructor are not inherited, so declare it as normal functions
  virtual void constructor(VirtualLayer *layerV, const char *animation) {
    this->layerV = layerV;
    this->animation = animation;
  }
  virtual void destructor() {
    //delete any allocated memory
  }

  virtual const char * name() {return "noname";}
  virtual const char * tags() {return "";}
  virtual uint8_t dim() {return _1D;};

  virtual void getControls(JsonArray controls) {};
  virtual void setControl(JsonObject control) {};

  //effect and fixdef
  virtual void setup() {};

  //effect and modifier
  virtual void loop() {}

  //modifier
  virtual void modifyPixelsPre() {}
  virtual void modifyPixel(Coord3D &pixel) {} //not const as pixel is changed
  virtual void modifyXYZ(Coord3D &pixel) {}
};
  

#if FT_LIVESCRIPT
class LiveScriptNode: public Node {
  public:

  //controls
  uint8_t speed = 128;
  uint8_t intensity = 128;
  uint8_t custom1 = 128;
  uint8_t custom2 = 128;
  uint8_t custom3 = 128;

  const char * name() override {return "LiveScriptNode";}

  void setup() override;
  void loop() override;
  void destructor() override;

  void getScriptsJson(JsonArray scripts);

  void compileAndRun();
  void kill();
  void free();
  void killAndDelete();
  void execute();

  void getControls(JsonArray controls) override;
  void setControl(JsonObject control) override;
  
};

#endif

#include "Fixtures.h"

#include "Effects.h"
// #include "EffectsFastLED.h"

#include "Modifiers.h"