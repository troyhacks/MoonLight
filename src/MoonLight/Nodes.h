/**
    @title     MoonLight
    @file      Nodes.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/general/utilities/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
**/

#pragma once

#if FT_MOONLIGHT

#define _1D 1
#define _2D 2
#define _3D 3

#include "VirtualLayer.h" //VirtualLayer.h will include PhysicalLayer.h

#include <ESPFS.h>

class Node {
public:
  static const char * name() {return "noname";}
  static const char * tags() {return "";}
  static uint8_t dim() {return _1D;};

  VirtualLayer *layerV = nullptr; //the virtual layer this effect is using

  bool hasLayout = false; //run map on monitor (pass1) and modifier new Node, on/off, control changed or layout setup, on/off or control changed (pass1 and 2) 
  bool requestMap = false; //collect requests to map as it is requested by setup and updateControl and only need to be done once
  bool hasModifier = false; //modifier new Node, on/off, control changed: run layout.requestMap. addLayoutPre: modifyLayout, addLight: modifyLight (todo XYZ: modifyXYZ)

  bool on = false; //onUpdate will set it on

  //C++ constructor and destructor are not inherited, so declare it as normal functions
  virtual void constructor(VirtualLayer *layerV) {
    this->layerV = layerV;
  }

  //effect and layout
  virtual void setup() {
    if (hasLayout) {
      layerV->layerP->lights.header.resetOffsets(); //reset offsets to default
      requestMap = true;
    }
  };

  //effect, layout and modifier
  virtual void addControls(JsonArray controls) {};

  template <class ControlType, class PointerType>
  JsonObject addControl(JsonArray controls, PointerType pointer2, const char *name, const char *type, ControlType defaul, int min = INT_MIN, int max = INT_MAX) {
    uint32_t pointer = (uint32_t)pointer2;
    JsonObject control = controls.add<JsonObject>(); 
    control["name"] = name;
    control["type"] = type;
    control["default"] = defaul;
    control["p"] = pointer;
    if (min != INT_MIN) control["min"] = min;
    if (max != INT_MAX) control["max"] = max;

    if (pointer) {
      //setValue
      if (control["type"] == "range" || control["type"] == "pin") {
        uint8_t *valuePointer = (uint8_t *)pointer;
        control["value"] = *valuePointer;
      }
      else if (control["type"] == "select") {
        char *valuePointer = (char *)pointer;
        control["value"] = valuePointer;
      }
      else if (control["type"] == "number") {
        uint16_t *valuePointer = (uint16_t *)pointer;
        control["value"] = *valuePointer;
      }
      else if (control["type"] == "checkbox") {
        bool *valuePointer = (bool *)pointer;
        control["value"] = *valuePointer;
      }
      // else if (control["type"] == "coord3D") {
      //   Coord3D *valuePointer = (Coord3D *)pointer;
        // control["value"] = *valuePointer;
      // }
      else
        ESP_LOGE(TAG, "type not supported yet %s", control["type"].as<String>().c_str());
    }

    return control;
  };

  virtual void updateControl(JsonObject control); // see Nodes.cpp for implementation

  //effect, layout and modifier (?)
  virtual void loop() {
    if (requestMap) { //not too early? otherwise change to loop1s
      requestMap = false;
      if (on) {
        for (layerV->layerP->pass = 1; layerV->layerP->pass <= 2; layerV->layerP->pass++)
          mapLayout(); //calls also addLayout
      } else {
        layerV->resetMapping();
      }
    }
  }

  //layout

  //calls addLayout functions, non virtual, only addLayout can be redefined in derived class
  virtual void mapLayout() {
    if (hasLayout) {
      ESP_LOGD(TAG, "%s", name());
      layerV->layerP->addLayoutPre();
      addLayout();
      layerV->layerP->addLayoutPost();
    }
  }

  virtual void addLayout() {} //the definition of the layout, called by mapLayout()

  void addLight(Coord3D position) {
    layerV->layerP->addLight(position);
  }

  void addPin(uint8_t pinNr) {
    layerV->layerP->addPin(pinNr);
  }

  //modifier
  virtual void modifyLayout() {}
  virtual void modifyLight(Coord3D &position) {} //not const as position is changed
  virtual void modifyXYZ(Coord3D &position) {}

  virtual void destructor() {
    //delete any allocated memory
    if (hasLayout)
      layerV->resetMapping();
  }

};
  

#if FT_LIVESCRIPT
class LiveScriptNode: public Node {
  public:

  static const char * name() {return "LiveScriptNode⚙️";}
  static uint8_t dim() {return _2D;}
  static const char * tags() {return "⚙️";}

  bool hasSetup = false;
  bool hasLoop = false;
  bool hasAddControls = false;

  const char *animation; //which animation (file) to run

  void setup() override; //addExternal, compileAndRun
  
  void addControls(JsonArray controls) override; //call addControls in LiveScript
  
  void loop() override; //call Node.loop to process requestMap. todo: sync with script...

  //layout
  void mapLayout() override; // call map in LiveScript
  
  void destructor() override;

  // LiveScript functions
  void compileAndRun();
  void execute();
  void kill();
  void free();
  void killAndDelete();
  void getScriptsJson(JsonArray scripts);
  
};

#endif

//data shared between nodes
static byte geq[16]= {0};
static float volume;

#include "Layouts.h"

#include "Effects.h"
// #include "EffectsFastLED.h"

#include "Modifiers.h"

#include "Mods.h"

#endif //FT_MOONLIGHT