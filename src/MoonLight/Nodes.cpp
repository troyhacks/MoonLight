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

// extern void _addPin(uint8_t pinNr);
// extern void _addPixelsPre();
// extern void _addPixel(uint16_t x, uint16_t y, uint16_t z);
// extern void _addPixelsPost();

PhysicalLayer *glayerP = nullptr;
VirtualLayer *glayerV = nullptr;

static void _addPin(uint8_t pinNr) {glayerP->addPin(pinNr);}
static void _addPixelsPre() {glayerP->addPixelsPre();}
static void _addPixel(uint16_t x, uint16_t y, uint16_t z) {glayerP->addPixel({x, y, z});}
static void _addPixelsPost() {glayerP->addPixelsPost();}

void _fadeToBlackBy(uint8_t fadeValue) { glayerV->fadeToBlackBy(fadeValue);}
static void _sPCLive(uint16_t pixel, CRGB color) {glayerV->setPixelColor(pixel, color);} //setPixelColor with color
static void _sCFPLive(uint16_t pixel, uint8_t index, uint8_t brightness) { glayerV->setPixelColor(pixel, ColorFromPalette(PartyColors_p, index, brightness));} //setPixelColor within palette

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

void addExternalVal(string result, string name, void * ptr) {
  if (findLink(name, externalType::value) == -1) //not allready added earlier
      addExternalVariable(name, result, "", ptr);
}

void addExternalFun(string result, string name, string parameters, void * ptr) {
  if (findLink(name, externalType::function) == -1) //not allready added earlier
      addExternalFunction(name, result, parameters, ptr);
}

Parser parser = Parser();

void LiveScriptNode::setup() {
    
  // ESP_LOGD(TAG, "animation %s", animation);

  if (animation[0] != '/') { //no sc script
      return;
  }

  //send UI spinner

  addExternalFun("void", "addPin", "uint8_t", (void *)_addPin);
  addExternalFun("void", "addPixelsPre", "", (void *)_addPixelsPre);
  addExternalFun("void", "addPixel", "uint16_t,uint16_t,uint16_t", (void *)_addPixel);
  addExternalFun("void", "addPixelsPost", "", (void *)_addPixelsPost);
  addExternalFun("uint32_t", "millis", "", (void *)millis);
  float (*_sin)(float) = sin;
  addExternalFun("float", "sin", "float", (void *)_sin);
  float (*_cos)(float) = cos;
  addExternalFun("float", "cos", "float", (void *)_cos);
  float (*_atan2)(float, float) = atan2;
  addExternalFun("float", "atan2", "float,float",(void*)_atan2);
  float (*_hypot)(float, float) = hypot;
  addExternalFun("float", "hypot", "float,float",(void*)_hypot);
  addExternalFun("float", "time", "float", (void *)_time);
  addExternalFun("float", "triangle", "float", (void *)_triangle);

  addExternalFun("void", "pinMode", "(int a1, int a2)", (void *)&pinMode);
  addExternalFun("void", "digitalWrite", "(int a1, int a2)", (void *)&digitalWrite);
  addExternalFun("void", "delay", "(int a1)", (void *)&delay);

  addExternalFun("uint8_t", "cos8", "uint8_t", (void *)cos8);
  addExternalFun("void", "delay", "uint8_t", (void *)delay);
  uint8_t (*_inoise8)(uint16_t, uint16_t, uint16_t)=inoise8;
  addExternalFun("uint8_t", "inoise8", "uint16_t,uint16_t,uint16_t", (void *)_inoise8);
  addExternalFun("uint32_t", "now", "", (void *)millis);
  uint16_t (*_random16)(uint16_t)=random16; //enforce specific random16 function
  addExternalFun("uint16_t", "random16", "uint16_t", (void *)_random16);
  addExternalFun("uint8_t", "sin8", "uint8_t", (void *)sin8);

  addExternalFun("void", "fadeToBlackBy", "uint8_t", (void *)_fadeToBlackBy);
  addExternalVal("CRGB *", "leds", (void *)layerV->layerP->leds);
  addExternalFun("void", "sPC", "uint16_t,CRGB", (void *)_sPCLive);
  addExternalFun("void", "sCFP", "uint16_t,uint8_t,uint8_t", (void *)_sCFPLive);
  addExternalVal("uint16_t", "width", &layerV->size.x);
  addExternalVal("uint16_t", "height", &layerV->size.y);
  addExternalVal("uint16_t", "depth", &layerV->size.z);

  //void sPC(uint16_t,CRGB)
  //CRGB * leds 

  for (asm_external el: external_links) {
      ESP_LOGD(TAG, "elink %s %s %d", el.shortname.c_str(), el.name.c_str(), el.type);
  }

  runningPrograms.setFunctionToSync(sync);

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