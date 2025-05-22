/**
    @title     MoonLight
    @file      Nodes.cpp
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/general/utilities/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
**/


#if FT_MOONLIGHT
#include "Nodes.h"

#if FT_LIVESCRIPT

#define USE_FASTLED //as ESPLiveScript.h calls hsv ! one of the reserved functions!!
#include "ESPLiveScript.h"

Node *gNode = nullptr;

// static void _addControl(uint8_t a1) {ESP_LOGD(TAG, "%d", a1);}
static void _addPin(uint8_t pinNr) {gNode->layerV->layerP->addPin(pinNr);}
static void _addLayoutPre() {gNode->layerV->layerP->addLayoutPre();}
static void _addLight(uint16_t x, uint16_t y, uint16_t z) {gNode->layerV->layerP->addLight({x, y, z});}
static void _addLayoutPost() {gNode->layerV->layerP->addLayoutPost();}

static void _modifyLayout() {gNode->modifyLayout();}
// static void _modifyLight() {gNode->modifyLight();} //need &position parameter
// static void _modifyXYZ() {gNode->modifyXYZ();}//need &position parameter

void _fadeToBlackBy(uint8_t fadeValue) { gNode->layerV->fadeToBlackBy(fadeValue);}
static void _sLC(uint16_t indexV, CRGB color) {gNode->layerV->setLightColor(indexV, color);}
static void _sCFP(uint16_t indexV, uint8_t index, uint8_t brightness) { gNode->layerV->setLightColor(indexV, ColorFromPalette(PartyColors_p, index, brightness));}

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
//   addExternal(    "void addControl(uint8_t)", (void *)_getControl);
  addExternal(    "void addPin(uint8_t)", (void *)_addPin);
  addExternal(    "void addLayoutPre()", (void *)_addLayoutPre);
  addExternal(    "void addLight(uint16_t,uint16_t,uint16_t)", (void *)_addLight);
  addExternal(    "void addLayoutPost()", (void *)_addLayoutPost);
  addExternal(    "void modifyLayout()", (void *)_modifyLayout);
//   addExternal(    "void modifyLight(uint16_t,uint16_t,uint16_t)", (void *)_modifyLight);
//   addExternal(    "void modifyXYZ(uint16_t,uint16_t,uint16_t)", (void *)_modifyXYZ);

  addExternal(    "void fadeToBlackBy(uint8_t)", (void *)_fadeToBlackBy);
  addExternal(   "CRGB* leds", (void *)layerV->layerP->lights.leds);
  addExternal(    "void sLC(uint16_t,CRGB)", (void *)_sLC);
  addExternal(    "void sCFP(uint16_t,uint8_t,uint8_t)", (void *)_sCFP);
  addExternal("uint16_t width", &layerV->size.x);
  addExternal("uint16_t height", &layerV->size.y);
  addExternal("uint16_t depth", &layerV->size.z);
  addExternal("bool on", &on);

  //controls (WLED Nostalgia)
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
        pre.format("#define NUM_LEDS %d\n", layerV->nrOfLights);

          std::string scScript = pre.c_str();
          scScript += file.readString().c_str();
          file.close();

          if (scScript.find("setup()") != std::string::npos) hasSetup = true;
          if (scScript.find("loop()") != std::string::npos) hasLoop = true;
          if (scScript.find("addLayout()") != std::string::npos) hasLayout = true;
          if (scScript.find("modifyLight(") != std::string::npos) hasModifier = true;
        //   if (scScript.find("modifyXYZ(") != std::string::npos) hasModifier = true;

          if (hasLayout) scScript += "void map(){addLayoutPre();addLayout();addLayoutPost();}"; //add map() function
          //add main function
          scScript += "void main(){";
          if (hasSetup) scScript += "setup();";
          if (hasLoop) scScript += "while(2>1){if(on){loop();sync();}else delay(1);}"; //loop must pauze when layout changes pass == 1! delay to avoid idle
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

    if (hasLayout) {
        map();
    }

    if (hasLoop) {
        // setup : create controls
        // executable.execute("setup"); 
        // send controls to UI
        // executable.executeAsTask("main"); //background task (async - vs sync)
        scriptRuntime.executeAsTask(animation, "main"); //background task (async - vs sync)
        //assert failed: xEventGroupSync event_groups.c:228 (uxBitsToWaitFor != 0)
    } else {
        scriptRuntime.execute(animation, "main");
    }
}

void LiveScriptNode::map() {
    if (hasLayout) {
        for (layerV->layerP->pass = 1; layerV->layerP->pass <= 2; layerV->layerP->pass++)
            scriptRuntime.execute(animation, "map"); 
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

void LiveScriptNode::addControls(JsonArray controls)  {
    addControl(controls, &speed, "speed", "range", 128);
    addControl(controls, &intensity, "intensity", "range", 128);
    addControl(controls, &custom1, "custom1", "range", 128);
    addControl(controls, &custom2, "custom2", "range", 128);
    addControl(controls, &custom3, "custom3", "range", 128);
    
    //if (exist addControls) scriptRuntime.execute(animation, "addControls"); 
    //
    // script example
    // uint8_t speed = 128;
    // uint8_t intensity = 128;
    // uint8_t custom1 = 128;
    // uint8_t custom2 = 128;
    // uint8_t custom3 = 128;
    // void addControls() {
    //     addControl(&speed, "speed", "range", 128);
    //     addControl(&intensity, "intensity", "range", 128);
    //     addControl(&custom1, "custom1", "range", 128);
    //     addControl(&custom2, "custom2", "range", 128);
    //     addControl(&custom3, "custom3", "range", 128);
    // }
    //
    // implementation:
    // addExternal(    "void addControl(uint32,const char*,const char*,int,int,int)", (void *)_addControl);
    // _addControl(pointerToScriptVariable, const char *name, const char *type, int default, int min = INT_MIN, int max = INT_MAX);

  }
  
  void LiveScriptNode::updateControl(JsonObject control)  {
    Node::updateControl(control); //call base class

    //if changed run setup needed??? (todo: if hasLayout then rerun mapping needed ..., if modifier then done by ModuleAnimations...)
    // setup();
  }

#endif
#endif