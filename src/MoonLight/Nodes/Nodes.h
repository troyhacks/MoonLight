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

#include "../Layers/VirtualLayer.h" //VirtualLayer.h will include PhysicalLayer.h

#include <ESPFS.h>

class Node {
public:
  static const char * name() {return "noname";}
  static const char * tags() {return "";}
  static uint8_t dim() {return _1D;};

  VirtualLayer *layerV = nullptr; //the virtual layer this effect is using
  JsonArray controls;

  virtual bool isLiveScriptNode() const { return false; }
  virtual bool hasLayout() const { return false; } //run map on monitor (pass1) and modifier new Node, on/off, control changed or layout setup, on/off or control changed (pass1 and 2) 
  virtual bool hasModifier() const { return false; } //modifier new Node, on/off, control changed: run layout.requestMapLayout. onLayoutPre: modifySize, addLight: modifyPosition XYZ: modifyXYZ

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
      MB_LOGE(ML_TAG, "type of %s not supported yet: %s", control["name"].as<String>().c_str(), control["type"].as<String>().c_str());

    if (newControl) {
      String oldValue = "";
      onUpdate(oldValue, control);
    }

    return control;
  }

  //called in addControl (oldValue = "") and in NodeManager onUpdate nodes[i].control[j]
  virtual void onUpdate(String &oldValue, JsonObject control); // see Nodes.cpp for implementation

  void requestMappings() {
    if (hasModifier() || hasLayout()) {
        // MB_LOGD(ML_TAG, "hasLayout or Modifier -> requestMapVirtual");
        layerV->layerP->requestMapVirtual = true;
    }
    if (hasLayout()) {
        // MB_LOGD(ML_TAG, "hasLayout -> requestMapPhysical");
        layerV->layerP->requestMapPhysical = true;
    }
  }

  //effect, layout and modifier (?)
  virtual void loop() {}
  virtual void onSizeChanged(Coord3D oldSize) {} //virtual/effect nodes: virtual size, physical/driver nodes: physical size

  //layout
  virtual void onLayout() {} //the definition of the layout, called by mapLayout()

  //convenience functions to add a light
  void addLight(Coord3D position) {
    layerV->layerP->addLight(position);
  }

  //convenience functions to add a pin
  void addPin(uint8_t pinNr) {
    layerV->layerP->addPin(pinNr);
  }
  char * addNextPin(char * &nextPin) { //&: by reference to change the pointer to the next pin
      while (*nextPin && !isdigit((unsigned char)*nextPin)) 
        nextPin++; 
      if (*nextPin) {
        int pin = strtol(nextPin, (char**)&nextPin, 10);
        addPin(pin);
      } //add next pin
      return nextPin;
  }

  //modifier
  virtual void modifySize() {}
  virtual void modifyPosition(Coord3D &position) {} //not const as position is changed
  virtual void modifyXYZ(Coord3D &position) {}

  virtual ~Node() {
    //delete any allocated memory

    MB_LOGD(ML_TAG, "Node destructor 🚥:%d 💎:%d", hasLayout(), hasModifier());
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
  bool hasModifyPosition = false;
  bool hasOnLayout = false;
  bool isLiveScriptNode() const override { return true; }
  bool hasModifier() const override { return hasModifyPosition; }
  bool hasLayout() const override { return hasOnLayout; }

  const char *animation = nullptr; //which animation (file) to run

  void setup() override; //addExternal, compileAndRun
  
  void loop() override; //call Node.loop to process requestMapLayout. todo: sync with script...

  //layout
  void onLayout() override; // call map in LiveScript
  
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

#if HP_ALL_DRIVERS
  // #define NUM_LEDS_PER_STRIP 256 not for non virtal... (only setting __delay when NO_WAIT)
  #include "I2SClocklessLedDriver.h"
  extern I2SClocklessLedDriver ledsDriver; //defined in Nodes.cpp
#else //ESP32_LEDSDRIVER  
  #include "ESP32-LedsDriver.h"
  #define MAX_PINS 20 // this is also defined in ESP32-LedsDriver.h...
  #if CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32S2
    static PhysicalDriverESP32S3 ledsDriver; //    sizeof(driver) = 1080K !
  #elif CONFIG_IDF_TARGET_ESP32
    static PhysicalDriverESP32D0 ledsDriver; //    sizeof(driver) = 1080K !
  #else
    static LedsDriver ledsDriver; //   only the core driver, for setBrightness and setColorCorrection and LUT
  #endif
#endif

class DriverNode: public Node {
  uint16_t maxPower = 10;
  uint8_t brightnessSaved = UINT8_MAX;
  
  protected:
  
  bool lightPresetSaved = false; //initLeds can only start if this has been saved

  #if HP_ALL_DRIVERS
    CRGB savedColorCorrection;
    bool initDone = false;
  #endif

  protected:
  uint8_t lightPreset = 2; //GRB

  public:

  void setup() override;

  void loop() override;

  void reOrderAndDimRGBW(uint8_t *packetRGBChannel, uint8_t *lightsRGBChannel);

  //called in addControl (oldValue = "") and in NodeManager onUpdate nodes[i].control[j]
  void onUpdate(String &oldValue, JsonObject control) override;
};

// Helper function to generate a triangle wave similar to beat16
inline uint8_t triangle8(uint8_t bpm, uint32_t timebase = 0) {
    uint8_t beat = beat8(bpm, timebase);
    if (beat < 128)
        return beat * 2; // rising edge
    else
        return (255 - ((beat - 128) * 2)); // falling edge
}

//data shared between nodes
static struct SharedData {
  uint8_t bands[16]= {0}; // Our calculated freq. channel result table to be used by effects
  float volume; // either sampleAvg or sampleAgc depending on soundAgc; smoothed sample
  int16_t volumeRaw; 
  float majorPeak; // FFT: strongest (peak) frequency
  uint16_t fps;
  uint8_t connectionStatus;
  size_t connectedClients;
  size_t clientListSize;
} sharedData;

/**
 * Nodes Guidelines:
 * 
 * 1) Don't use String type class variables but char[x] as can crash when node is destructed (node in PSRAM, string in heap)
 * 2) no static variables in node classes as we can run multiple instances of the same node which should not share data -> class variables.
 */

//Drivers first as used by others
#include "Drivers/D_PhysicalDriver.h"
#include "Drivers/D_FastLED.h"
#include "Drivers/D_Artnet.h"
#include "Drivers/D_AudioSync.h"
#include "Drivers/D_VirtualDriver.h"
#include "Drivers/D_Hub75.h"

#include "Layouts/L_MoonLight.h"
#include "Layouts/L_SE16.h"

#include "Effects/E_MoonLight.h"
#include "Effects/E_MoonModules.h"
#include "Effects/E_WLED.h"
#include "Effects/E_MovingHeads.h"
#include "Effects/E_FastLED.h"
#include "Effects/E_SoulmateLights.h"

#include "Modifiers/M_MoonLight.h"

#endif //FT_MOONLIGHT