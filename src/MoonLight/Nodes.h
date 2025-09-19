/**
    @title     MoonLight
    @file      Nodes.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/moonlight/overview/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact us for more information.
**/

#pragma once

#if FT_MOONLIGHT

#include "VirtualLayer.h" //VirtualLayer.h will include PhysicalLayer.h

#include <ESPFS.h>

class Node {
public:
  static const char * name() {return "noname";}
  static const char * tags() {return "";}
  static uint8_t dim() {return _1D;};

  VirtualLayer *layerV = nullptr; //the virtual layer this effect is using
  JsonArray controls;

  bool hasLayout = false; //run map on monitor (pass1) and modifier new Node, on/off, control changed or layout setup, on/off or control changed (pass1 and 2) 
  bool hasModifier = false; //modifier new Node, on/off, control changed: run layout.requestMapLayout. addLayoutPre: modifySize, addLight: modifyPosition XYZ: modifyXYZ

  virtual bool isLiveScriptNode() const { return false; }

  bool on = false; //onUpdate will set it on

  //C++ constructors are not inherited, so declare it as normal functions
  virtual void constructor(VirtualLayer *layerV, JsonArray controls) {
    this->layerV = layerV;
    this->controls = controls;
  }

  //effect and layout
  virtual void setup() {};

  //effect, layout and modifier

  template <class ControlType>
  JsonObject addControl(ControlType &variable, const char *name, const char *type, int min = 0, int max = UINT8_MAX, bool ro = false) {
    uint32_t pointer = (uint32_t)&variable;

    bool newControl = false; //flag to check if control is new or already exists
    // if control already exists only update it's pointer
    JsonObject control;
    for (JsonObject control1 : controls) {
      if (control1["name"] == name) {
        // MB_LOGD(ML_TAG, "%s t:%s p:%p ps:%d", name, type, pointer, sizeof(ControlType));
        control1["p"] = pointer;
        control = control1; //set control to the found one
        break;
      }
    }

    if (control.isNull()) { //if control not found, create a new one
      control = controls.add<JsonObject>();
      control["name"] = name;
      control["type"] = type;
      control["default"] = variable;

      control["p"] = pointer;

      if (ro) control["ro"] = true; //else if (!control["ro"].isNull()) control.remove("ro");
      if (min != 0) control["min"] = min; //else if (!control["min"].isNull()) control.remove("min");
      if (max != UINT8_MAX) control["max"] = max; //else if (!control["max"].isNull()) control.remove("max");

      newControl = true; //set flag to true, as control is new
    }

    // MB_LOGD(ML_TAG, "%s t:%s p:%p ps:%d", name, type, pointer, sizeof(ControlType));

    //setValue
    if (control["type"] == "range" || control["type"] == "select" || control["type"] == "pin") {
      if (sizeof(ControlType) != 1) {
        MB_LOGE(ML_TAG, "sizeof mismatch for %s", name);
      } else if (newControl) {
        // uint8_t * valuePointer = (uint8_t *)pointer;
        // *valuePointer = variable;
        // control["value"] = *valuePointer;
        control["value"] = variable;
      }
    }
    else if (control["type"] == "selectFile" || control["type"] == "text") {
      // if (sizeof(ControlType) != 4) {
      //   MB_LOGE(ML_TAG, "sizeof mismatch for %s", name);
      // } else 
      if (newControl) {
        // char *valuePointer = (char *)pointer;
        // *valuePointer = variable;
        // control["value"] = valuePointer;
        control["value"] = variable;
      }
    }
    else if (control["type"] == "number") {
      if (sizeof(ControlType) != 2) {
        MB_LOGE(ML_TAG, "sizeof mismatch for %s", name);
      } else if (newControl) {
        // uint16_t *valuePointer = (uint16_t *)pointer;
        // *valuePointer = variable;
        // control["value"] = *valuePointer;
        control["value"] = variable;
      }
    }
    else if (control["type"] == "checkbox") {
      if (sizeof(ControlType) != 1) {
        MB_LOGE(ML_TAG, "sizeof mismatch for %s", name);
      } else if (newControl) {
        // bool *valuePointer = (bool *)pointer;
        // control["value"] = variable;
        // *valuePointer = control["value"];
        control["value"] = variable;
      }
    }
    else if (control["type"] == "coord3D") {
      if (sizeof(ControlType) != sizeof(Coord3D)) {
        MB_LOGE(ML_TAG, "sizeof mismatch for %s", name);
      } else if (newControl) {
        // Coord3D *valuePointer = (Coord3D *)pointer;
        // control["value"] = variable;
        // *valuePointer = control["value"];
        control["value"] = variable;
      }
    }
    else
      MB_LOGE(ML_TAG, "type not supported yet %s", control["type"].as<String>().c_str());

    if (newControl) {
      String oldValue = "";
      onUpdate(oldValue, control);
    }

    return control;
  }

  //called in addControl (oldValue = "") and in NodeManager onUpdate nodes[i].control[j]
  virtual void onUpdate(String &oldValue, JsonObject control); // see Nodes.cpp for implementation

  void requestMappings() {
    if (hasModifier || hasLayout) {
        // MB_LOGD(ML_TAG, "hasLayout or Modifier -> requestMapVirtual");
        layerV->layerP->requestMapVirtual = true;
    }
    if (hasLayout) {
        // MB_LOGD(ML_TAG, "hasLayout -> requestMapPhysical");
        layerV->layerP->requestMapPhysical = true;
    }
  }

  //effect, layout and modifier (?)
  virtual void loop() {}
  virtual void onSizeChanged(Coord3D oldSize) {} //virtual/effect nodes: virtual size, physical/driver nodes: physical size

  //layout
  virtual void addLayout() {} //the definition of the layout, called by mapLayout()

  //convenience functions to add a light
  void addLight(Coord3D position) {
    layerV->layerP->addLight(position);
  }

  //convenience functions to add a pin
  void addPin(uint8_t pinNr) {
    layerV->layerP->addPin(pinNr);
  }

  //modifier
  virtual void modifySize() {}
  virtual void modifyPosition(Coord3D &position) {} //not const as position is changed
  virtual void modifyXYZ(Coord3D &position) {}

  virtual ~Node() {
    //delete any allocated memory

    MB_LOGD(ML_TAG, "Node destructor 🚥:%d 💎:%d", hasLayout, hasModifier);

  }

};

#if FT_LIVESCRIPT
class LiveScriptNode: public Node {
  public:

  static const char * name() {return "LiveScriptNode ⚙️";}
  static uint8_t dim() {return _2D;}
  static const char * tags() {return "";}

  bool hasSetup = false;
  bool hasLoop = false;
  bool hasAddControl = false;
  bool isLiveScriptNode() const override { return true; }

  const char *animation = nullptr; //which animation (file) to run

  void setup() override; //addExternal, compileAndRun
  
  void loop() override; //call Node.loop to process requestMapLayout. todo: sync with script...

  //layout
  void addLayout() override; // call map in LiveScript
  
  ~LiveScriptNode(); 

  // LiveScript functions
  void compileAndRun();
  void execute();
  void kill();
  void free();
  void killAndDelete();
  static void getScriptsJson(JsonArray scripts);
  
};

#endif

// Helper function to generate a triangle wave similar to beat16
inline uint8_t triangle8(uint8_t bpm, uint32_t timebase = 0) {
    uint8_t beat = beat8(bpm, timebase);
    if (beat < 128)
        return beat * 2; // rising edge
    else
        return (255 - ((beat - 128) * 2)); // falling edge
}

#include "Mods.h"

#include "Layouts.h"

#include "Effects.h"
// #include "EffectsFastLED.h"

#include "Modifiers.h"

#endif //FT_MOONLIGHT