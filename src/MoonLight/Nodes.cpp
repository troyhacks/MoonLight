/**
    @title     MoonLight
    @file      Nodes.cpp
    @repo      https://github.com/ewowi/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/ewowi/MoonLight/commits/main
    @Doc       https://ewowi.github.io/MoonLight/general/utilities/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
**/

#include "Nodes.h"

#if FT_LIVESCRIPT

#define USE_FASTLED //as ESPLiveScript.h calls hsv ! one of the reserved functions!!
#include "ESPLiveScript.h"

Node *gNode = nullptr;

static void _addPin(uint8_t pinNr) {gNode->layerV->layerP->addPin(pinNr);}
static void _addPixelsPre() {gNode->layerV->layerP->addPixelsPre();}
static void _addPixel(uint16_t x, uint16_t y, uint16_t z) {gNode->layerV->layerP->addPixel({x, y, z});}
static void _addPixelsPost() {gNode->layerV->layerP->addPixelsPost();}

static void _modifyPixelsPre() {gNode->modifyPixelsPre();}
// static void _modifyPixel() {gNode->modifyPixel();}
// static void _modifyXYZ() {gNode->modifyXYZ();}

void _fadeToBlackBy(uint8_t fadeValue) { gNode->layerV->fadeToBlackBy(fadeValue);}
static void _sPC(uint16_t pixel, CRGB color) {gNode->layerV->setPixelColor(pixel, color);}
static void _sCFP(uint16_t pixel, uint8_t index, uint8_t brightness) { gNode->layerV->setPixelColor(pixel, ColorFromPalette(PartyColors_p, index, brightness));}

static float _triangle(float j) {return 1.0 - fabs(fmod(2 * j, 2.0) - 1.0);}
static float _time(float j) {
    float myVal = millis();;
    myVal = myVal / 65535 / j;           // PixelBlaze uses 1000/65535 = .015259. 
    myVal = fmod(myVal, 1.0);               // ewowi: with 0.015 as input, you get fmod(millis/1000,1.0), which has a period of 1 second, sounds right
    return myVal;
}

void sync() {
    static uint32_t frameCounter = 0;
    frameCounter++;
    delay(1); //feed the watchdog, otherwise watchdog will reset the ESP
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
        ESP_LOGE(TAG, "Failed to parse function definition: %s", definition.c_str());
    }
}
  
Parser parser = Parser();

void LiveScriptNode::setup() {
    
  // ESP_LOGD(TAG, "animation %s", animation);

  if (animation[0] != '/') { //no sc script
      return;
  }

  //generic functions
  addExternal("uint32_t millis()", (void *)millis);
  addExternal("uint32_t now()", (void *)millis); //todo: synchronized time (sys->now)
  addExternal("uint16_t random16(uint16_t)", (void *)(uint16_t (*)(uint16_t))random16);
  addExternal(    "void delay(uint8_t)", (void *)delay);
  addExternal(    "void pinMode(int,int)", (void *)pinMode);
  addExternal(    "void digitalWrite(int,int)", (void *)digitalWrite);

  //trigonometric functions
  addExternal(   "float sin(float)", (void *)(float (*)(float))sin);
  addExternal(   "float cos(float)", (void *)(float (*)(float))cos);
  addExternal( "uint8_t sin8(uint8_t)", (void *)sin8);
  addExternal( "uint8_t cos8(uint8_t)", (void *)cos8);
  addExternal(   "float atan2(float,float)", (void*)(float (*)(float,float))atan2);
  addExternal( "uint8_t inoise8(uint16_t,uint16_t,uint16_t)", (void *)(uint8_t (*)(uint16_t,uint16_t,uint16_t))inoise8);
  addExternal(   "float hypot(float,float)", (void*)(float (*)(float,float))hypot);
  addExternal(   "float time(float)", (void *)_time);
  addExternal(   "float triangle(float)", (void *)_triangle);

  //MoonLight functions
  addExternal(    "void addPin(uint8_t)", (void *)_addPin);
  addExternal(    "void addPixelsPre()", (void *)_addPixelsPre);
  addExternal(    "void addPixel(uint16_t,uint16_t,uint16_t)", (void *)_addPixel);
  addExternal(    "void addPixelsPost()", (void *)_addPixelsPost);
  addExternal(    "void modifyPixelsPre()", (void *)_modifyPixelsPre);
//   addExternal(    "void modifyPixel(uint16_t,uint16_t,uint16_t)", (void *)_modifyPixel);
//   addExternal(    "void modifyXYZ(uint16_t,uint16_t,uint16_t)", (void *)_modifyXYZ);

  addExternal(    "void fadeToBlackBy(uint8_t)", (void *)_fadeToBlackBy);
  addExternal(   "CRGB* leds", (void *)layerV->layerP->leds);
  addExternal(    "void sPC(uint16_t,CRGB)", (void *)_sPC);
  addExternal(    "void sCFP(uint16_t,uint8_t,uint8_t)", (void *)_sCFP);
  addExternal("uint16_t width", &layerV->size.x);
  addExternal("uint16_t height", &layerV->size.y);
  addExternal("uint16_t depth", &layerV->size.z);

  //controls
  addExternal("uint8_t speed", &speed);
  addExternal("uint8_t intensity", &intensity);
  addExternal("uint8_t custom1", &custom1);
  addExternal("uint8_t custom2", &custom2);
  addExternal("uint8_t custom3", &custom3);

  for (asm_external el: external_links) {
      ESP_LOGD(TAG, "elink %s %s %d", el.shortname.c_str(), el.name.c_str(), el.type);
  }

  compileAndRun();
}

void LiveScriptNode::compileAndRun() {
  runningPrograms.setFunctionToSync(sync);

  //send UI spinner

  //run the recompile not in httpd but in main loopTask (otherwise we run out of stack space)
  // runInLoopTask.push_back([&, animation, type, error] {
      ESP_LOGD(TAG, "%s", animation);
      File file = ESPFS.open(animation);
      if (file) {
        Char<32> pre;
        pre.format("#define NUM_LEDS %d\n", layerV->nrOfLeds);

          std::string scScript = pre.c_str();
          scScript += file.readString().c_str();
          file.close();

          if (scScript.find("setup(") != std::string::npos) hasSetup = true;
          if (scScript.find("loop(") != std::string::npos) hasLoop = true;
          if (scScript.find("addPixel(") != std::string::npos) hasFixDef = true;
          if (scScript.find("modifyPixel(") != std::string::npos) hasModifier = true;

          scScript += "void main(){";
          if (hasFixDef) scScript += "addPixelsPre();";
          if (hasSetup) scScript += "setup();";
          if (hasFixDef) scScript += "addPixelsPost();";
          if (hasLoop) scScript += "while(2>1){loop();sync();}";
          scScript += "}";

          ESP_LOGD(TAG, "script %s", scScript.c_str());

          // TaskHandle_t currentTask = xTaskGetCurrentTaskHandle();
          // ESP_LOGI(TAG, "task %s %d", pcTaskGetName(currentTask), uxTaskGetStackHighWaterMark(currentTask));
  
          // ESP_LOGD(TAG, "parsing %s", scScript.c_str());

          Executable executable = parser.parseScript(&scScript); // note that this class will be deleted after the function call !!!
          executable.name = animation;
          ESP_LOGD(TAG, "parsing %s done\n", animation);
          scriptRuntime.addExe(executable); //if already exists, delete it first
          ESP_LOGD(TAG, "addExe success %s\n", executable.exeExist?"true":"false");

          gNode = this; //todo: this is not working well with multiple scripts running!!!

          if (executable.exeExist) {
            execute();
          } else
              ESP_LOGD(TAG, "error %s", executable.error.error_message.c_str());

          //send error to client ... not working yet
          // error.set(executable.error.error_message); //String(executable.error.error_message.c_str());
          // _state.data["nodes"][2]["error"] = executable.error.error_message;

      }
  // });
  
  //stop UI spinner      
}

void LiveScriptNode::destructor() {
    ESP_LOGD(TAG, "%s", animation);
    scriptRuntime.kill(animation);
}

void LiveScriptNode::kill() {
    ESP_LOGD(TAG, "%s", animation);
    scriptRuntime.kill(animation);
}

void LiveScriptNode::execute() {
    ESP_LOGD(TAG, "%s", animation);
    if (hasLoop) {
        // setup : create controls
        // executable.execute("setup"); 
        // send controls to UI
        // executable.executeAsTask("main"); //background task (async - vs sync)
        scriptRuntime.executeAsTask(animation, "main"); //background task (async - vs sync)
        //assert failed: xEventGroupSync event_groups.c:228 (uxBitsToWaitFor != 0)
    } else {
        scriptRuntime.execute(animation, "main"); //background task (async - vs sync)
        // pass = 1;
        // liveM->executeTask(liveFixtureID, "c");
        // pass = 2;
        // liveM->executeTask(liveFixtureID, "c");
    }
}

void LiveScriptNode::free() {
    ESP_LOGD(TAG, "%s", animation);
    scriptRuntime.free(animation);
}

void LiveScriptNode::killAndDelete() {
    ESP_LOGD(TAG, "%s", animation);
    scriptRuntime.kill(animation);
    // scriptRuntime.free(animation);
    scriptRuntime.deleteExe(animation);
};

void LiveScriptNode::loop() {

    // if (isSyncalled)
    //     while (!resetSync)
    //     {
    //     }
}

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
        // ESP_LOGD(TAG, "scriptRuntime exec %s r:%d h:%d, e:%d h:%d b:%d + d:%d = %d", exec.name.c_str(), exec.isRunning(), exec.isHalted, exec.exeExist, exec.__run_handle_index, exeInfo.binary_size, exeInfo.data_size, exeInfo.total_size);
    }
}

void LiveScriptNode::getControls(JsonArray controls)  {
    JsonObject control;
    control = controls.add<JsonObject>(); control["name"] = "speed"; control["type"] = "range"; control["default"] = 128; control["value"] = speed;
    control = controls.add<JsonObject>(); control["name"] = "intensity"; control["type"] = "range"; control["default"] = 128; control["value"] = intensity;
    control = controls.add<JsonObject>(); control["name"] = "custom1"; control["type"] = "checkbox"; control["default"] = 128; control["value"] = custom1;
    control = controls.add<JsonObject>(); control["name"] = "custom2"; control["type"] = "checkbox"; control["default"] = 128; control["value"] = custom3;
    control = controls.add<JsonObject>(); control["name"] = "custom3"; control["type"] = "checkbox"; control["default"] = 128; control["value"] = custom2;
  }
  
  void LiveScriptNode::setControl(JsonObject control)  {
    ESP_LOGD(TAG, "%s = %s", control["name"].as<String>().c_str(), control["value"].as<String>().c_str());
    if (control["name"] == "speed") speed = control["value"];
    if (control["name"] == "intensity") intensity = control["value"];
    if (control["name"] == "custom1") custom1 = control["value"];
    if (control["name"] == "custom2") custom2 = control["value"];
    if (control["name"] == "custom3") custom3 = control["value"];
    //if changed run setup
    setup();
  }

#endif