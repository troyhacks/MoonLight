/**
    @title     MoonLight
    @file      Nodes.cpp
    @repo      https://github.com/ewowi/MoonBase, submit changes to this file as PRs
    @Authors   https://github.com/ewowi/MoonBase/commits/main
    @Doc       https://ewowi.github.io/MoonBase/general/utilities/
    @Copyright © 2025 Github MoonBase Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
**/

#include "Nodes.h"

#if FT_LIVESCRIPT

#define USE_FASTLED //as ESPLiveScript.h calls hsv ! one of the reserved functions!!
#include "ESPLiveScript.h"

PhysicalLayer *glayerP = nullptr;
VirtualLayer *glayerV = nullptr;

static void _addPin(uint8_t pinNr) {glayerP->addPin(pinNr);}
static void _addPixelsPre() {glayerP->addPixelsPre();}
static void _addPixel(uint16_t x, uint16_t y, uint16_t z) {glayerP->addPixel({x, y, z});}
static void _addPixelsPost() {glayerP->addPixelsPost();}

void _fadeToBlackBy(uint8_t fadeValue) { glayerV->fadeToBlackBy(fadeValue);}
static void _sPC(uint16_t pixel, CRGB color) {glayerV->setPixelColor(pixel, color);} //setPixelColor with color
static void _sCFP(uint16_t pixel, uint8_t index, uint8_t brightness) { glayerV->setPixelColor(pixel, ColorFromPalette(PartyColors_p, index, brightness));} //setPixelColor within palette

static float _triangle(float j) {return 1.0 - fabs(fmod(2 * j, 2.0) - 1.0);}
static float _time(float j) {
    float myVal = millis();;
    myVal = myVal / 65535 / j;           // PixelBlaze uses 1000/65535 = .015259. 
    myVal = fmod(myVal, 1.0);               // ewowi: with 0.015 as input, you get fmod(millis/1000,1.0), which has a period of 1 second, sounds right
    return myVal;
}

void sync() {
    delay(1); //feed the watchdog
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

  addExternal(    "void fadeToBlackBy(uint8_t)", (void *)_fadeToBlackBy);
  addExternal(   "CRGB* leds", (void *)layerV->layerP->leds);
  addExternal(    "void sPC(uint16_t,CRGB)", (void *)_sPC);
  addExternal(    "void sCFP(uint16_t,uint8_t,uint8_t)", (void *)_sCFP);
  addExternal("uint16_t width", &layerV->size.x);
  addExternal("uint16_t height", &layerV->size.y);
  addExternal("uint16_t depth", &layerV->size.z);


  for (asm_external el: external_links) {
      ESP_LOGD(TAG, "elink %s %s %d", el.shortname.c_str(), el.name.c_str(), el.type);
  }

  runningPrograms.setFunctionToSync(sync);

  //send UI spinner

  //run the recompile not in httpd but in main loopTask (otherwise we run out of stack space)
  // runInLoopTask.push_back([&, animation, type, error] {
      ESP_LOGD(TAG, "compileAndRun %s %s", animation, type);
      File file = ESPFS.open(animation);
      if (file) {
        Char<32> pre;
        pre.format("#define NUM_LEDS %d\n", layerV->nrOfLeds);

          std::string scScript = pre.c_str();
          scScript += file.readString().c_str();
          file.close();

          const char * post = "";
          if (equal(type, "Effect")) {
              post= "void main(){setup();while(2>1){loop();sync();}}"; //;
          } else if (equal(type, "Fixture definition")) {
              post = "void main(){addPixelsPre();setup();addPixelsPost();}";
          }

          scScript += post;

          ESP_LOGD(TAG, "script %s", scScript.c_str());

          // TaskHandle_t currentTask = xTaskGetCurrentTaskHandle();
          // ESP_LOGI(TAG, "task %s %d", pcTaskGetName(currentTask), uxTaskGetStackHighWaterMark(currentTask));
  
          // ESP_LOGD(TAG, "parsing %s", scScript.c_str());

          Executable executable = parser.parseScript(&scScript); // note that this class will be deleted after the function call !!!
          executable.name = animation;
          ESP_LOGD(TAG, "parsing %s done\n", animation);
          scriptRuntime.addExe(executable); //if already exists, delete it first
          ESP_LOGD(TAG, "addExe success %s\n", executable.exeExist?"true":"false");

          glayerP = layerV->layerP;
          glayerV = layerV;

          if (executable.exeExist) {
              if (equal(type, "Fixture definition")) {
                  // executable.execute("main"); //background task (async - vs sync)
                  scriptRuntime.execute(executable.name, "main"); //background task (async - vs sync)
                  // pass = 1;
                  // liveM->executeTask(liveFixtureID, "c");
                  // pass = 2;
                  // liveM->executeTask(liveFixtureID, "c");
              }
              if (equal(type, "Effect")) {
                  // setup : create controls
                  // executable.execute("setup"); 
                  // send controls to UI
                  // executable.executeAsTask("main"); //background task (async - vs sync)
                  scriptRuntime.executeAsTask(executable.name, "main"); //background task (async - vs sync)
                  //assert failed: xEventGroupSync event_groups.c:228 (uxBitsToWaitFor != 0)
              }
          } else
              ESP_LOGD(TAG, "error %s", executable.error.error_message.c_str());

          //send error to client ... not working yet
          // error.set(executable.error.error_message); //String(executable.error.error_message.c_str());
          // _state.data["nodes"][2]["error"] = executable.error.error_message;

      }
  // });
  
  //stop UI spinner      
}

void LiveScriptNode::loop() {

    // if (isSyncalled)
    //     while (!resetSync)
    //     {
    //     }
}

#endif