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

  bool hasSetup = false;
  bool hasLoop = false; //equal to hasEffect?
  bool hasModifier = false;
  bool hasLayout = false; //Mapper?

  bool on = false; //onUpdate will set it on

  //C++ constructor and destructor are not inherited, so declare it as normal functions
  virtual void constructor(VirtualLayer *layerV) {
    this->layerV = layerV;
  }
  virtual void destructor() {
    //delete any allocated memory
  }

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

    return control;
  };

  virtual void updateControl(JsonObject control) {
    if (!control["name"].isNull() && !control["type"].isNull() && !control["p"].isNull()) { //name and type can be null if controll is removed in compareRecursive
      ESP_LOGD(TAG, "%s = %s %s %s", control["name"].as<String>().c_str(), control["value"].as<String>().c_str(), control["type"].as<String>().c_str(), control["p"].as<String>().c_str());
      int pointer = control["p"];

      if (control["type"] == "range" || control["type"] == "pin") {
        uint8_t *valuePointer = (uint8_t *)pointer;
        *valuePointer = control["value"];
        ESP_LOGD(TAG, "%s = %d", control["name"].as<String>().c_str(), *valuePointer);
      }
      else if (control["type"] == "select") {
        char *valuePointer = (char *)pointer;
        strncpy(valuePointer, control["value"].as<String>().c_str(), control["max"].isNull()?32:control["max"]);
      }
      else if (control["type"] == "number") {
        uint16_t *valuePointer = (uint16_t *)pointer;
        *valuePointer = control["value"];
      }
      else if (control["type"] == "checkbox") {
        bool *valuePointer = (bool *)pointer;
        *valuePointer = control["value"];
      }
      // else if (control["type"] == "coord3D") {
      //   Coord3D *valuePointer = (Coord3D *)pointer;
      //   *valuePointer = value;
      // }
      else
        ESP_LOGE(TAG, "type not supported yet %s", control["type"].as<String>().c_str());

    }

  };

  //effect and layout
  virtual void setup() {};

  //effect and modifier
  virtual void loop() {}

  //layout
  virtual void map() {}

  //modifier
  virtual void modifyLayout() {}
  virtual void modifyLight(Coord3D &position) {} //not const as position is changed
  // virtual void modifyXYZ(Coord3D &position) {}
};
  

#if FT_LIVESCRIPT
class LiveScriptNode: public Node {
  public:

  static const char * name() {return "LiveScriptNode⚙️";}
  static uint8_t dim() {return _2D;}
  static const char * tags() {return "⚙️";}

  const char *animation;

  //controls
  uint8_t speed = 128;
  uint8_t intensity = 128;
  uint8_t custom1 = 128;
  uint8_t custom2 = 128;
  uint8_t custom3 = 128;

  void setup() override;
  void loop() override;
  void destructor() override;

  void getScriptsJson(JsonArray scripts);

  void compileAndRun();
  void kill();
  void free();
  void killAndDelete();
  void execute();

  void addControls(JsonArray controls) override;
  void updateControl(JsonObject control) override;

  void map() override;
  
};

#endif

#include "Layouts.h"

#include "Effects.h"
// #include "EffectsFastLED.h"

#include "Modifiers.h"

#endif //FT_MOONLIGHT