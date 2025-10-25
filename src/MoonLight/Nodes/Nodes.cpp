/**
    @title     MoonLight
    @file      Nodes.cpp
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/moonlight/overview/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact us for more information.
**/

#if FT_MOONLIGHT
#include "Nodes.h"

#include <ESP32SvelteKit.h> //for safeModeMB

void Node::updateControl(String &oldValue, JsonObject control) {
    MB_LOGD(ML_TAG, "onUpdate %s", control["name"].as<String>().c_str());
    if (oldValue == "") return; //newControl, value already set
    if (!control["name"].isNull() && !control["type"].isNull() && !control["p"].isNull()) { //name and type can be null if control is removed in compareRecursive
        int pointer = control["p"];
        MB_LOGD(ML_TAG, "%s = %s t:%s p:%p", control["name"].as<String>().c_str(), control["value"].as<String>().c_str(), control["type"].as<String>().c_str(), pointer);

        if (pointer) {
            if (control["type"] == "range" || control["type"] == "select" || control["type"] == "pin" || control["type"] == "number") {
                if (control["size"] == 8) {
                    uint8_t *valuePointer = (uint8_t *)pointer;
                    *valuePointer = control["value"];
                    // MB_LOGV(ML_TAG, "%s = %d", control["name"].as<String>().c_str(), *valuePointer);
                }
                else if (control["size"] == 16) {
                    uint16_t *valuePointer = (uint16_t *)pointer;
                    *valuePointer = control["value"];
                    // MB_LOGV(ML_TAG, "%s = %d", control["name"].as<String>().c_str(), *valuePointer);
                }
                else if (control["size"] == 32) {
                    int *valuePointer = (int *)pointer;
                    *valuePointer = control["value"];
                    // MB_LOGV(ML_TAG, "%s = %d", control["name"].as<String>().c_str(), *valuePointer);
                }
                else if (control["size"] == 33) {
                    float *valuePointer = (float *)pointer;
                    *valuePointer = control["value"];
                    // MB_LOGV(ML_TAG, "%s = %d", control["name"].as<String>().c_str(), *valuePointer);
                }
                else {
                    MB_LOGW(ML_TAG, "size not supported or not set for %s: %d", control["name"].as<String>().c_str(), control["size"].as<int>());
                }
            }
            else if (control["type"] == "selectFile" || control["type"] == "text") {
                char *valuePointer = (char *)pointer;
                strncpy(valuePointer, control["value"].as<String>().c_str(), control["max"].isNull()?32:control["max"]);
            }
            else if (control["type"] == "checkbox" && control["size"] == sizeof(bool)) {
                bool *valuePointer = (bool *)pointer;
                *valuePointer = control["value"].as<bool>();
            }
            else if (control["type"] == "coord3D" && control["size"] == sizeof(Coord3D)) {
                Coord3D *valuePointer = (Coord3D *)pointer;
                *valuePointer = control["value"].as<Coord3D>();
            }
            else
                MB_LOGE(ML_TAG, "type of %s not compatible: %s (%d)", control["name"].as<String>().c_str(), control["type"].as<String>().c_str(), control["size"].as<uint8_t>());
        }
    }
};

#if FT_LIVESCRIPT

#define USE_FASTLED //as ESPLiveScript.h calls hsv ! one of the reserved functions!!
#include "ESPLiveScript.h"

Node *gNode = nullptr;

static void _addControl(uint8_t *var, char *name, char* type, uint8_t min = 0, uint8_t max = UINT8_MAX) {MB_LOGV(ML_TAG, "%s %s %d (%d-%d)", name, type, var, min, max);gNode->addControl(*var, name, type, min, max);}
static void _addPin(uint8_t pinNr) {gNode->layer->layerP->addPin(pinNr);}
static void _addLight(uint8_t x, uint8_t y, uint8_t z) {gNode->layer->layerP->addLight({x, y, z});}

static void _modifySize() {gNode->modifySize();}
static void _modifyPosition(Coord3D &position) {gNode->modifyPosition(position);} //need &position parameter
// static void _modifyXYZ() {gNode->modifyXYZ();}//need &position parameter

void _fadeToBlackBy(uint8_t fadeValue) { gNode->layer->fadeToBlackBy(fadeValue);}
static void _setRGB(uint16_t indexV, CRGB color) {gNode->layer->setRGB(indexV, color);}
static void _setRGBPal(uint16_t indexV, uint8_t index, uint8_t brightness) { gNode->layer->setRGB(indexV, ColorFromPalette(PartyColors_p, index, brightness));}
static void _setPan(uint16_t indexV, uint8_t value) {gNode->layer->setPan(indexV, value);}
static void _setTilt(uint16_t indexV, uint8_t value) {gNode->layer->setTilt(indexV, value);}

volatile xSemaphoreHandle WaitAnimationSync = xSemaphoreCreateBinary();

void sync() {
    static uint32_t frameCounter = 0;
    frameCounter++;
    delay(1); //feed the watchdog, otherwise watchdog will reset the ESP
    // Serial.print("s");
    // 🌙 adding semaphore wait too long logging
    if (xSemaphoreTake(WaitAnimationSync, pdMS_TO_TICKS(100))==pdFALSE) {
        MB_LOGW(ML_TAG, "WaitAnimationSync wait too long");
        xSemaphoreTake(WaitAnimationSync, portMAX_DELAY);
    }
}

void addExternal(string definition, void * ptr) {
    bool success = false;
    size_t firstSpace = definition.find(' ');
    if (firstSpace != std::string::npos) {
        string returnType = definition.substr(0, firstSpace);

        string parameters = "";
        string functionName = "";

        size_t openParen = definition.find('(', firstSpace + 1);
        if (openParen != std::string::npos) { // function
            functionName = definition.substr(firstSpace + 1, openParen - firstSpace - 1);

            size_t closeParen = definition.find(')', openParen + 1);
            if (closeParen != std::string::npos) { // function with parameters
                parameters = definition.substr(openParen + 1, closeParen - openParen - 1);
                success = true;
                if (findLink(functionName, externalType::function) == -1) //not allready added earlier
                    addExternalFunction(functionName, returnType, parameters, ptr);
            }
        } else { // variable
            functionName = definition.substr(firstSpace + 1);
            success = true;
            if (findLink(functionName, externalType::value) == -1) //not allready added earlier
                addExternalVariable(functionName, returnType, "", ptr);
        }
    }
    if (!success) {
        MB_LOGE(ML_TAG, "Failed to parse function definition: %s", definition.c_str());
    }
}
  
Parser parser = Parser();

void LiveScriptNode::setup() {
    
  // MB_LOGV(ML_TAG, "animation %s", animation);

  if (animation[0] != '/') { //no sc script
      return;
  }

  //make sure types in below functions are correct !!! otherwise livescript will crash

  //generic functions
  addExternal("uint32_t millis()", (void *)millis);
  addExternal("uint32_t now()", (void *)millis); //todo: synchronized time (sys->now)
  addExternal("uint16_t random16(uint16_t)", (void *)(uint16_t (*)(uint16_t))random16);
  addExternal(    "void delay(uint32_t)", (void *)delay);
  addExternal(    "void pinMode(uint8_t,uint8_t)", (void *)pinMode);
  addExternal(    "void digitalWrite(uint8_t,uint8_t)", (void *)digitalWrite);

  //trigonometric functions
  addExternal(   "float sin(float)", (void *)(float (*)(float))sin);
  addExternal(   "float cos(float)", (void *)(float (*)(float))cos);
  addExternal( "uint8_t sin8(uint8_t)", (void *)sin8);
  addExternal( "uint8_t cos8(uint8_t)", (void *)cos8);
  addExternal(   "float atan2(float,float)", (void*)(float (*)(float,float))atan2);
  addExternal( "uint8_t inoise8(uint16_t,uint16_t,uint16_t)", (void *)(uint8_t (*)(uint16_t,uint16_t,uint16_t))inoise8);
  addExternal( "uint8_t beatsin8(uint16_t,uint8_t,uint8_t,uint32_t,uint8_t)", (void *)beatsin8);
  addExternal(   "float hypot(float,float)", (void*)(float (*)(float,float))hypot);
  addExternal(   "float beat8(uint8_t,uint32_t)", (void *)beat8); //saw wave
  addExternal( "uint8_t triangle8(uint8_t)", (void *)triangle8);

  //MoonLight functions
  addExternal(    "void addControl(void*,char*,char*,uint8_t,uint8_t)", (void *)_addControl);
  addExternal(    "void addPin(uint8_t)", (void *)_addPin);
  addExternal(    "void addLight(uint8_t,uint8_t,uint8_t)", (void *)_addLight);
  addExternal(    "void modifySize()", (void *)_modifySize);
//   addExternal(    "void modifyPosition(Coord3D &position)", (void *)_modifyPosition);
//   addExternal(    "void modifyXYZ(uint16_t,uint16_t,uint16_t)", (void *)_modifyXYZ);


  //MoonLight physical and virtual driver vars
  //  but keep enabled to avoid compile errors when used in non virtual context
//   addExternal( "uint8_t colorOrder", &layer->layerP->ledsDriver.colorOrder);
//   addExternal( "uint8_t clockPin", &layer->layerP->ledsDriver.clockPin);
//   addExternal( "uint8_t latchPin", &layer->layerP->ledsDriver.latchPin);
//   addExternal( "uint8_t clockFreq", &layer->layerP->ledsDriver.clockFreq);
//   addExternal( "uint8_t dmaBuffer", &layer->layerP->ledsDriver.dmaBuffer);

  addExternal(    "void fadeToBlackBy(uint8_t)", (void *)_fadeToBlackBy);
  addExternal(   "CRGB* leds", (void *)(CRGB *)layer->layerP->lights.channels);
  addExternal(    "void setRGB(uint16_t,CRGB)", (void *)_setRGB);
  addExternal(    "void setRGBPal(uint16_t,uint8_t,uint8_t)", (void *)_setRGBPal);
  addExternal(    "void setPan(uint16_t,uint8_t)", (void *)_setPan);
  addExternal(    "void setTilt(uint16_t,uint8_t)", (void *)_setTilt);
  addExternal( "uint8_t width", &layer->size.x);
  addExternal( "uint8_t height", &layer->size.y);
  addExternal( "uint8_t depth", &layer->size.z);
  addExternal(    "bool on", &on);

//   for (asm_external el: external_links) {
//       MB_LOGV(ML_TAG, "elink %s %s %d", el.shortname.c_str(), el.name.c_str(), el.type);
//   }


//   runningPrograms.setPrekill(layer->layerP->ledsDriver.preKill, layer->layerP->ledsDriver.postKill); //for clockless driver...
  runningPrograms.setFunctionToSync(sync);

  compileAndRun();

}

void LiveScriptNode::loop() {
    // Serial.print("l");
    xSemaphoreGive(WaitAnimationSync);
}

void LiveScriptNode::onLayout() {
    if (hasOnLayout()) {
        MB_LOGV(ML_TAG, "%s", animation);
        scriptRuntime.execute(animation, "onLayout"); 
    }
}

LiveScriptNode::~LiveScriptNode() {
    MB_LOGV(ML_TAG, "%s", animation);
    scriptRuntime.kill(animation);
}

// LiveScriptNode functions

void LiveScriptNode::compileAndRun() {
  //send UI spinner

  //run the recompile not in httpd but in main loopTask (otherwise we run out of stack space)
  // std::lock_guard<std::mutex> lock(runInTask_mutex);
  // runInTask1.push_back([&, animation, type, error] {
      MB_LOGV(ML_TAG, "%s", animation);
      File file = ESPFS.open(animation);
      if (file) {
        Char<32> pre;
        pre.format("#define NUM_LEDS %d\n", layer->nrOfLights);

          std::string scScript = pre.c_str();
          scScript += file.readString().c_str();
          file.close();

          if (scScript.find("setup()") != std::string::npos) hasSetupFunction = true;
          if (scScript.find("loop()") != std::string::npos) hasLoopFunction = true;
          if (scScript.find("onLayout()") != std::string::npos) hasOnLayoutFunction = true;
          if (scScript.find("modifyPosition(") != std::string::npos) hasModifyFunction = true;
        //   if (scScript.find("modifyXYZ(") != std::string::npos) hasModifier = true;

          //add main function
          scScript += "void main(){";
          if (hasSetupFunction) scScript += "setup();";
          if (hasLoopFunction) scScript += "while(true){if(on){loop();sync();}else delay(1);}"; //loop must pauze when layout changes pass == 1! delay to avoid idle
          scScript += "}";

          MB_LOGV(ML_TAG, "script \n%s", scScript.c_str());

          // MB_LOGV(ML_TAG, "parsing %s", scScript.c_str());

          Executable executable = parser.parseScript(&scScript); // note that this class will be deleted after the function call !!!
          executable.name = animation;
          MB_LOGV(ML_TAG, "parsing %s done", animation);
          scriptRuntime.addExe(executable); //if already exists, delete it first
          MB_LOGV(ML_TAG, "addExe success %s", executable.exeExist?"true":"false");

          gNode = this; //todo: this is not working well with multiple scripts running!!!

          if (executable.exeExist) {
            execute();
          } else
              MB_LOGV(ML_TAG, "error %s", executable.error.error_message.c_str());

          //send error to client ... not working yet
          // error.set(executable.error.error_message); //String(executable.error.error_message.c_str());
          // _state.data["nodes"][2]["error"] = executable.error.error_message;
      }
  // });
  
  //stop UI spinner      
}

void LiveScriptNode::execute() {

    if (safeModeMB) {
        MB_LOGW(ML_TAG, "Safe mode enabled, not executing script %s", animation);
        return;
    }
    MB_LOGV(ML_TAG, "%s", animation);

    requestMappings(); // requestMapPhysical and requestMapVirtual will call the script onLayout function (check if this can be done in case the script also has loop running !!!)

    if (hasLoopFunction) {
        // setup : create controls
        // executable.execute("setup"); 
        // send controls to UI
        // executable.executeAsTask("main"); //background task (async - vs sync)
        MB_LOGV(ML_TAG, "%s executeAsTask main", animation);
        scriptRuntime.executeAsTask(animation, "main"); //background task (async - vs sync)
        //assert failed: xEventGroupSync event_groups.c:228 (uxBitsToWaitFor != 0)
    } else {
        MB_LOGV(ML_TAG, "%s execute main", animation);
        scriptRuntime.execute(animation, "main");
    }
    MB_LOGV(ML_TAG, "%s execute started", animation);
}

void LiveScriptNode::kill() {
    MB_LOGV(ML_TAG, "%s", animation);
    scriptRuntime.kill(animation);
}

void LiveScriptNode::free() {
    MB_LOGV(ML_TAG, "%s", animation);
    scriptRuntime.free(animation);
}

void LiveScriptNode::killAndDelete() {
    MB_LOGV(ML_TAG, "%s", animation);
    scriptRuntime.kill(animation);
    // scriptRuntime.free(animation);
    scriptRuntime.deleteExe(animation);
};

void LiveScriptNode::getScriptsJson(JsonArray scripts) {
    for (Executable &exec: scriptRuntime._scExecutables) {
        exe_info exeInfo = scriptRuntime.getExecutableInfo(exec.name);
        JsonObject object = scripts.add<JsonObject>();
        object["name"] = exec.name;
        object["isRunning"] = exec.isRunning();
        object["isHalted"] = exec.isHalted;
        object["exeExist"] = exec.exeExist;
        object["handle"] = exec.__run_handle_index;
        object["binary_size"] = exeInfo.binary_size;
        object["data_size"] = exeInfo.data_size;
        object["error"] = exec.error.error_message;
        object["kill"] = 0;
        object["free"] = 0;
        object["delete"] = 0;
        object["execute"] = 0;
        // MB_LOGV(ML_TAG, "scriptRuntime exec %s r:%d h:%d, e:%d h:%d b:%d + d:%d = %d", exec.name.c_str(), exec.isRunning(), exec.isHalted, exec.exeExist, exec.__run_handle_index, exeInfo.binary_size, exeInfo.data_size, exeInfo.total_size);
    }
}

#endif //FT_LIVESCRIPT

#if HP_ALL_DRIVERS
  I2SClocklessLedDriver ledsDriver;
#endif

void DriverNode::setup() {
  addControl(maxPower, "maxPower", "number", 0, 100);
  JsonObject property = addControl(lightPreset, "lightPreset", "select"); 
  JsonArray values = property["values"].to<JsonArray>();
  values.add("RGB");
  values.add("RBG");
  values.add("GRB"); //default WS2812
  values.add("GBR");
  values.add("BRG");
  values.add("BGR");
  values.add("RGBW"); //e.g. 4 channel par/dmx light
  values.add("GRBW"); //rgbw LED eg. sk6812
  values.add("GRB6"); //some LED curtains
  values.add("RGBWYP"); //6 channel par/dmx light with UV etc
  values.add("MHBeeEyes150W-15 🐺"); //15 channels moving head, see https://moonmodules.org/MoonLight/moonlight/drivers/#art-net
  values.add("MHBeTopper19x15W-32 🐺"); //32 channels moving head
  values.add("MH19x15W-24"); //24 channels moving heads
}

void DriverNode::loop() {

  LightsHeader *header = &layer->layerP->lights.header;

  //use ledsDriver LUT for super efficient leds dimming 🔥 (used by reOrderAndDimRGBW)

  uint8_t brightness = (header->offsetBrightness == UINT8_MAX)?header->brightness:255; //set brightness to 255 if offsetBrightness is set (fixture will do its own brightness)

  if (brightness != brightnessSaved) {
    //Use FastLED for setMaxPowerInMilliWatts stuff
    uint8_t correctedBrightness = calculate_max_brightness_for_power_mW((CRGB *)&layer->layerP->lights.channels, layer->layerP->lights.header.nrOfLights, brightness, maxPower * 1000);
    // MB_LOGD(ML_TAG, "setBrightness b:%d + p:%d -> cb:%d", brightness, maxPower, correctedBrightness);
    ledsDriver.setBrightness(correctedBrightness);
    brightnessSaved = brightness;
  }

  #if HP_ALL_DRIVERS
    if (savedColorCorrection.red != layer->layerP->lights.header.red || savedColorCorrection.green != layer->layerP->lights.header.green || savedColorCorrection.blue != layer->layerP->lights.header.blue) {
      ledsDriver.setGamma(layer->layerP->lights.header.red/255.0, layer->layerP->lights.header.blue/255.0, layer->layerP->lights.header.green/255.0, 1.0);
      // MB_LOGD(ML_TAG, "setColorCorrection r:%d, g:%d, b:%d (%d %d %d)", layer->layerP->lights.header.red, layer->layerP->lights.header.green, layer->layerP->lights.header.blue, savedColorCorrection.red, savedColorCorrection.green, savedColorCorrection.blue);
      savedColorCorrection.red = layer->layerP->lights.header.red;
      savedColorCorrection.green = layer->layerP->lights.header.green;
      savedColorCorrection.blue = layer->layerP->lights.header.blue;
    }
  #else //ESP32_LEDSDRIVER
    CRGB correction;
    uint8_t white;
    ledsDriver.getColorCorrection(correction.red, correction.green, correction.blue, white);
    if (correction.red != layer->layerP->lights.header.red || correction.green != layer->layerP->lights.header.green || correction.blue != layer->layerP->lights.header.blue) {
      MB_LOGD(ML_TAG, "setColorCorrection r:%d, g:%d, b:%d (%d %d %d)", layer->layerP->lights.header.red, layer->layerP->lights.header.green, layer->layerP->lights.header.blue, correction.red, correction.green, correction.blue);
      ledsDriver.setColorCorrection(layer->layerP->lights.header.red, layer->layerP->lights.header.green, layer->layerP->lights.header.blue);
    }
  #endif
}

void DriverNode::onUpdate(String &oldValue, JsonObject control) {

  LightsHeader *header = &layer->layerP->lights.header;

  MB_LOGD(ML_TAG, "%s: %s ", control["name"].as<String>().c_str(), control["value"].as<String>().c_str());

  if (control["name"] == "maxPower") {
    uint8_t brightness = (header->offsetBrightness == UINT8_MAX)?header->brightness:255; //set brightness to 255 if offsetBrightness is set (fixture will do its own brightness)
    uint8_t correctedBrightness = calculate_max_brightness_for_power_mW((CRGB *)&layer->layerP->lights.channels, layer->layerP->lights.header.nrOfLights, brightness, maxPower * 1000);
    MB_LOGD(ML_TAG, "setBrightness b:%d + p:%d -> cb:%d", brightness, maxPower, correctedBrightness);
    ledsDriver.setBrightness(correctedBrightness);
  }
  else if (control["name"] == "lightPreset") {
    uint8_t oldChannelsPerLight = header->channelsPerLight;

    header->resetOffsets();

    switch (lightPreset) {
      case 0: header->channelsPerLight = 3; header->offsetRed = 0; header->offsetGreen = 1; header->offsetBlue = 2; break; //RGB
      case 1: header->channelsPerLight = 3; header->offsetRed = 0; header->offsetGreen = 2; header->offsetBlue = 1; break; //RBG
      case 2: header->channelsPerLight = 3; header->offsetRed = 1; header->offsetGreen = 0; header->offsetBlue = 2; break; //GRB
      case 3: header->channelsPerLight = 3; header->offsetRed = 2; header->offsetGreen = 0; header->offsetBlue = 1; break; //GBR
      case 4: header->channelsPerLight = 3; header->offsetRed = 1; header->offsetGreen = 2; header->offsetBlue = 0; break; //BRG
      case 5: header->channelsPerLight = 3; header->offsetRed = 2; header->offsetGreen = 1; header->offsetBlue = 0; break; //BGR
      case 6: header->channelsPerLight = 4; header->offsetRed = 0; header->offsetGreen = 1; header->offsetBlue = 2; header->offsetWhite = 3; break; //RGBW - Par Lights
      case 7: header->channelsPerLight = 4; header->offsetRed = 1; header->offsetGreen = 0; header->offsetBlue = 2; header->offsetWhite = 3; break; //GRBW - RGBW Leds
      case 8: header->channelsPerLight = 6; header->offsetRed = 1; header->offsetGreen = 0; header->offsetBlue = 2; break; //GRB6
      case 9: header->channelsPerLight = 6; header->offsetRed = 0; header->offsetGreen = 1; header->offsetBlue = 2; header->offsetWhite = 3; break; //RGBWYP - 6 channel Par/DMX Lights with UV etc
      case 10: //MHBeTopper19x15W-32
        layer->layerP->lights.header.channelsPerLight = 32;
        header->offsetRed = 0; header->offsetGreen = 1; header->offsetBlue = 2;
        layer->layerP->lights.header.offsetRGB = 9;
        layer->layerP->lights.header.offsetRGB1 = 13;
        layer->layerP->lights.header.offsetRGB2 = 17;
        layer->layerP->lights.header.offsetRGB3 = 24;
        layer->layerP->lights.header.offsetPan = 0;
        layer->layerP->lights.header.offsetTilt = 2;
        layer->layerP->lights.header.offsetZoom = 5;
        layer->layerP->lights.header.offsetBrightness = 6;
        break;
      case 11: //MHBeeEyes150W-15
        layer->layerP->lights.header.channelsPerLight = 15; //set channels per light to 15 (RGB + Pan + Tilt + Zoom + Brightness)
        header->offsetRed = 0; header->offsetGreen = 1; header->offsetBlue = 2;
        layer->layerP->lights.header.offsetRGB = 10; //set offset for RGB lights in DMX map
        layer->layerP->lights.header.offsetPan = 0;
        layer->layerP->lights.header.offsetTilt = 1;
        layer->layerP->lights.header.offsetZoom = 7;
        layer->layerP->lights.header.offsetBrightness = 8; //set offset for brightness
        layer->layerP->lights.header.offsetGobo = 5; //set offset for color wheel in DMX map
        layer->layerP->lights.header.offsetBrightness2 = 3; //set offset for color wheel brightness in DMX map    } //BGR
        break;
      case 12: //MH19x15W-24
        layer->layerP->lights.header.channelsPerLight = 24;
        header->offsetRed = 0; header->offsetGreen = 1; header->offsetBlue = 2;
        layer->layerP->lights.header.offsetPan = 0;
        layer->layerP->lights.header.offsetTilt = 1;
        layer->layerP->lights.header.offsetBrightness = 3;
        layer->layerP->lights.header.offsetRGB = 4;
        layer->layerP->lights.header.offsetRGB1 = 8;
        layer->layerP->lights.header.offsetRGB2 = 12;
        layer->layerP->lights.header.offsetZoom = 17;
        break;
    }

    MB_LOGI(ML_TAG, "setLightPreset %d (cPL:%d, o:%d,%d,%d,%d)", lightPreset, header->channelsPerLight, header->offsetRed, header->offsetGreen, header->offsetBlue, header->offsetWhite);
    
    // FASTLED_ASSERT(true, "oki");
    
    #if HP_ALL_DRIVERS
      #ifndef CONFIG_IDF_TARGET_ESP32P4
        if (initDone) {
          
          // ledsDriver.setOffsets(layer->layerP->lights.header.offsetRed, layer->layerP->lights.header.offsetGreen, layer->layerP->lights.header.offsetBlue, layer->layerP->lights.header.offsetWhite);

          // if (oldChannelsPerLight != header->channelsPerLight)
          //   restartNeeded = true; //in case 
        }
      #endif

    #else //ESP32_LEDSDRIVER
      if (ledsDriver.initLedsDone) {
        
        ledsDriver.setOffsets(layer->layerP->lights.header.offsetRed, layer->layerP->lights.header.offsetGreen, layer->layerP->lights.header.offsetBlue, layer->layerP->lights.header.offsetWhite);

        if (oldChannelsPerLight != header->channelsPerLight)
          restartNeeded = true; //in case 
      }
    #endif

    lightPresetSaved = true;
  }
}

void DriverNode::reOrderAndDimRGBW(uint8_t *packetRGBChannel, uint8_t *lightsRGBChannel) {
  //use ledsDriver.__rbg_map[0]; for super fast brightness and gamma correction! see secondPixel in ESP32-LedDriver!
  //apply the LUT to the RGB channels !
  
  packetRGBChannel[layer->layerP->lights.header.offsetRed] = ledsDriver.__red_map[lightsRGBChannel[0]];
  packetRGBChannel[layer->layerP->lights.header.offsetGreen] = ledsDriver.__green_map[lightsRGBChannel[1]];
  packetRGBChannel[layer->layerP->lights.header.offsetBlue] = ledsDriver.__blue_map[lightsRGBChannel[2]];
  if (layer->layerP->lights.header.offsetWhite != UINT8_MAX)
    packetRGBChannel[layer->layerP->lights.header.offsetWhite] = ledsDriver.__white_map[lightsRGBChannel[3]];
}

#endif //FT_MOONLIGHT