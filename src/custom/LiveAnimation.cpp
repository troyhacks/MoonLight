/**
    @title     MoonLight
    @file      LiveAnimation.cpp
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/moonlight/animation/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
**/

#if FT_LIVEANIMATION == 1

#include "LiveAnimation.h"

#include "ESPLiveScript.h" //note: contains declarations AND definitions, therefore can only be included once!

void LiveAnimationState::read(LiveAnimationState &state, JsonObject &root)
{
    TaskHandle_t httpdTask = xTaskGetHandle("httpd");
    ESP_LOGI("", "LiveAnimation::read task %s %d", pcTaskGetName(httpdTask), uxTaskGetStackHighWaterMark(httpdTask));
    root["lightsOn"] = state.lightsOn;
    root["brightness"] = state.brightness;
    root["animation"] = state.animation;

    root["animations"].to<JsonArray>();
    root["animations"].add("Random");
    root["animations"].add("Rainbow");
    //find all the .sc files on FS
    #if FT_ENABLED(FT_FILEMANAGER)
        File rootFolder = ESPFS.open("/");
        walkThroughFiles(rootFolder, [&](File folder, File file) {
            if (strstr(file.name(), ".sc")) {
                // ESP_LOGD("", "found file %s", file.path());
                root["animations"].add((char *)file.path());
            }
        });
        rootFolder.close();
    #endif

    root["driverOn"] = state.driverOn;
}

StateUpdateResult LiveAnimationState::update(JsonObject &root, LiveAnimationState &state)
{
    TaskHandle_t httpdTask = xTaskGetHandle("httpd");
    ESP_LOGI("", "LiveAnimation::update task %s %d", pcTaskGetName(httpdTask), uxTaskGetStackHighWaterMark(httpdTask));
    state.updatedItems.clear();

    if (state.lightsOn != root["lightsOn"] | state.lightsOn) {
        state.lightsOn = root["lightsOn"]; state.updatedItems.push_back("lightsOn");
    }
    if (state.brightness != root["brightness"] | state.brightness) {
        state.brightness = root["brightness"]; state.updatedItems.push_back("brightness");
        FastLED.setBrightness(state.lightsOn?state.brightness:0);
    }
    if (state.animation != root["animation"].as<String>()) {
        state.animation = root["animation"].as<String>(); state.updatedItems.push_back("animation");
    }
    if (state.driverOn != root["driverOn"] | state.driverOn) {
        state.driverOn = root["driverOn"]; state.updatedItems.push_back("driverOn");
    }

    if (state.updatedItems.size())
        ESP_LOGD("", "LiveAnimation::update %s", state.updatedItems.front().c_str());

    return state.updatedItems.size()?StateUpdateResult::CHANGED:StateUpdateResult::UNCHANGED;
}

LiveAnimation::LiveAnimation(PsychicHttpServer *server,
                                     ESP32SvelteKit *sveltekit
                                     #if FT_ENABLED(FT_FILEMANAGER)
                                         , FilesService *filesService
                                     #endif
                                    ) : _httpEndpoint(LiveAnimationState::read,
                                                                                                         LiveAnimationState::update,
                                                                                                         this,
                                                                                                         server,
                                                                                                         "/rest/liveAnimation",
                                                                                                         sveltekit->getSecurityManager(),
                                                                                                         AuthenticationPredicates::IS_AUTHENTICATED),
                                                                                           _eventEndpoint(LiveAnimationState::read,
                                                                                                          LiveAnimationState::update,
                                                                                                          this,
                                                                                                          sveltekit->getSocket(),
                                                                                                          "liveAnimation"),
                                                                                           _webSocketServer(LiveAnimationState::read,
                                                                                                            LiveAnimationState::update,
                                                                                                            this,
                                                                                                            server,
                                                                                                            "/ws/liveAnimation",
                                                                                                            sveltekit->getSecurityManager(),
                                                                                                            AuthenticationPredicates::IS_AUTHENTICATED),
                                                                                            _socket(sveltekit->getSocket()),
                                                                                             _fsPersistence(LiveAnimationState::read,
                                                                                                      LiveAnimationState::update,
                                                                                                      this,
                                                                                                      sveltekit->getFS(),
                                                                                                      "/config/liveAnimation.json")
{
    ESP_LOGD("", "LiveAnimation::constructor");

    // configure settings service update handler to update state
    #if FT_ENABLED(FT_FILEMANAGER)
        _filesService = filesService;
    #endif
}

void LiveAnimation::begin()
{
    ESP_LOGD("", "LiveAnimation::begin");
    FastLED.addLeds<WS2812B, 16, GRB>(_state.leds, 0, _state.nrOfLeds); //pin 2
    FastLED.setMaxPowerInMilliWatts(10000); // 5v, 2000mA, to protect usb while developing
    ESP_LOGD("", "FastLED.addLeds n:%d o:%d b:%d", _state.nrOfLeds, _state.lightsOn, _state.brightness);

    _httpEndpoint.begin();
    _eventEndpoint.begin();
    _fsPersistence.readFromFS();

    #if FT_ENABLED(FT_FILEMANAGER)
        //create a handler which recompiles the animation when the file of the current animation changes
        _filesService->addUpdateHandler([&](const String &originId)
        { 
            ESP_LOGD("", "FilesService::updateHandler %s", originId.c_str());
            //read the file state
            _filesService->read([&](FilesState &state) {
                // loop over all changed files (normally only one)
                for (auto updatedItem : state.updatedItems) {
                    //if file is the current animation, recompile it (to do: multiple animations)
                    if (updatedItem == _state.animation) {
                        ESP_LOGD("", "LiveAnimation::updateHandler updatedItem %s", updatedItem.c_str());
                        compileAndRun();
                    }
                }
            });
        });

        addUpdateHandler([&](const String &originId)
        {
            ESP_LOGD("", "LiveAnimation::updateHandler %s", originId.c_str());
            read([&](LiveAnimationState &state) {
                for (String updatedItem : state.updatedItems) {
                    if (updatedItem == "animation") {
                        compileAndRun();
                    } 
                }
            });
        });
    #endif

    //handler not active yet so manually trigger
    compileAndRun(); //change from livescript causes crashes
}

void LiveAnimation::loop()
{
    bool showLeds = false;
    //select the right effect
    if (_state.animation == "Random") {
        fadeToBlackBy(_state.leds, _state.nrOfLeds, 70);
        _state.leds[random16(_state.nrOfLeds)] = CRGB(255, random8(), 0);
        showLeds = true;
    } else if (_state.animation == "Rainbow") {
        static uint8_t hue = 0;
        fill_rainbow(_state.leds, _state.nrOfLeds, hue++, 7);
        showLeds = true;
    } else {
        //Done by live script (Yves)
    }

    if (showLeds) driverShow();
}

void LiveAnimation::driverShow()
{
    if (_state.driverOn)
        FastLED.show();
    else 
        delay(1); //to avoid watchdog crash
}

void LiveAnimation::compileAndRun() {

    if (_state.animation[0] != '/') { //no sc script
        runInLoopTask.push_back([&] { // run in loopTask to avoid stack overflow
            scriptRuntime.killAndFreeRunningProgram();
        });
        return;
    }

    //send UI spinner

    //run the recompile not in httpd but in main loopTask (otherwise we run out of stack space)
    runInLoopTask.push_back([&] {
        File file = ESPFS.open(_state.animation);
        if (file) {
            std::string scScript = file.readString().c_str();
            // scScript += "void main(){setup();sync();}";
            file.close();

            Parser parser = Parser();
            Executable executable = parser.parseScript(&scScript);
            ESP_LOGD("", "parsing %s done\n", _state.animation.c_str());
            scriptRuntime.addExe(executable);
            ESP_LOGD("", "addExe success %s\n", executable.exeExist?"true":"false");

            if (executable.exeExist)
                executable.executeAsTask("main"); //background task (async - vs sync)
        }
    });

    //stop UI spinner
}

#endif


/* homework assignment: 

Step 1: create an .sc file with this and verify it is working (no coding required):
=======================

void setup() {
  printf("Run Live Script good afternoon\n");
}
void main() {
  setup();
}

Step 2: make below script work, including controls: brightness/on, slider (easy) and driverOn
=======================

void setup() {
  printf("Run Live Script good morning\n");
}
void loop() {
  fadeToBlackBy(40);
  leds[random16(255)] = CRGB(0, 0, 255);
}
void main() {
  setup();
  while (true) {
    loop();
    sync(); //or show??
  }
}

*/