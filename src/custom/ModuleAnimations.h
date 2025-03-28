/**
    @title     MoonLight
    @file      ModuleAnimations.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/custom/animation/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
**/

#ifndef ModuleAnimations_h
#define ModuleAnimations_h

#if FT_LIVEANIMATION == 1

#include "Module.h"

#include "FastLED.h"
#define MAXLEDS 8192

#include "ESPLiveScript.h" //note: contains declarations AND definitions, therefore can only be included once!

class ModuleAnimations : public Module
{
public:

    CRGB leds[MAXLEDS];
    uint16_t nrOfLeds = 256;
    Parser parser = Parser();
    String animation = "Random";

    ModuleAnimations(PsychicHttpServer *server,
        ESP32SvelteKit *sveltekit
        #if FT_ENABLED(FT_FILEMANAGER)
            , FilesService *filesService
        #endif
    ) : Module("animations", server, sveltekit, filesService) {
        ESP_LOGD("", "constructor");
    }

    void begin() {
        Module::begin();

        ESP_LOGD("", "begin");
        FastLED.addLeds<WS2812B, 16, GRB>(leds, 0, nrOfLeds); //pin 2
        FastLED.setMaxPowerInMilliWatts(10000); // 5v, 2000mA, to protect usb while developing
        ESP_LOGD("", "FastLED.addLeds n:%d", nrOfLeds);

        #if FT_ENABLED(FT_FILEMANAGER)
        //create a handler which recompiles the animation when the file of the current animation changes

        _filesService->addUpdateHandler([&](const String &originId)
        { 
            ESP_LOGD("", "FilesService::updateHandler %s", originId.c_str());
            //read the file state
            _filesService->read([&](FilesState &filesState) {
                // loop over all changed files (normally only one)
                for (auto updatedItem : filesState.updatedItems) {
                    //if file is the current animation, recompile it (to do: multiple animations)
                    for (JsonObject node: _state.data["nodes"].as<JsonArray>()) {
                        String animation = node["animation"];

                        if (updatedItem == animation) {
                            ESP_LOGD("", "LiveAnimation::updateHandler updatedItem %s", updatedItem.c_str());
                            compileAndRun(animation);
                        }
                    }
                }
            });
        });

        #endif

    }

    void setupDefinition(JsonArray root) override {
        JsonObject property;
        JsonArray details;
        JsonArray values;

        property = root.add<JsonObject>(); property["name"] = "lightsOn"; property["type"] = "checkbox"; property["default"] = true;
        property = root.add<JsonObject>(); property["name"] = "brightness"; property["type"] = "range"; property["min"] = 0; property["max"] = 255; property["default"] = 73;
        property = root.add<JsonObject>(); property["name"] = "driverOn"; property["type"] = "checkbox"; property["default"] = true;
        property = root.add<JsonObject>(); property["name"] = "nodes"; property["type"] = "array"; details = property["n"].to<JsonArray>();
        {
            property = details.add<JsonObject>(); property["name"] = "animation"; property["type"] = "selectFile"; property["default"] = "Random"; values = property["values"].to<JsonArray>();
            values.add("Random");
            values.add("Rainbow");
            //find all the .sc files on FS
            #if FT_ENABLED(FT_FILEMANAGER)
                File rootFolder = ESPFS.open("/");
                walkThroughFiles(rootFolder, [&](File folder, File file) {
                    if (strstr(file.name(), ".sc")) {
                        // ESP_LOGD("", "found file %s", file.path());
                        values.add((char *)file.path());
                    }
                });
                rootFolder.close();
            #endif
            property = details.add<JsonObject>(); property["name"] = "type"; property["type"] = "select"; property["default"] = "Effect"; values = property["values"].to<JsonArray>();
            values.add("Fixture definition");
            values.add("Fixture mapping");
            values.add("Effect");
            values.add("Modifier");
            values.add("Driver show");
            property = details.add<JsonObject>(); property["name"] = "size"; property["type"] = "number"; property["default"] = 85;
        }

        property = root.add<JsonObject>(); property["name"] = "scripts"; property["type"] = "array"; details = property["n"].to<JsonArray>();
        {
            property = details.add<JsonObject>(); property["name"] = "name"; property["type"] = "text"; property["default"] = "ewowi";
            property = details.add<JsonObject>(); property["name"] = "type"; property["type"] = "text"; property["default"] = "ewowi";
        }

        char buffer[256];
        serializeJson(root, buffer, sizeof(buffer));
        ESP_LOGD("", "definition %s", buffer);
    }

    void onUpdate(UpdatedItem updatedItem) override
    {
        if (updatedItem.name == "lightsOn" || updatedItem.name == "brightness") {
            ESP_LOGD("", "handle %s.%s[%d] %s", updatedItem.parent.c_str(), updatedItem.name.c_str(), updatedItem.index, updatedItem.value.as<String>());
            FastLED.setBrightness(_state.data["lightsOn"]?_state.data["brightness"]:0);
        } else if (updatedItem.parent == "nodes" && updatedItem.name == "animation") {    
            ESP_LOGD("", "handle %s.%s[%d] %s", updatedItem.parent.c_str(), updatedItem.name.c_str(), updatedItem.index, _state.data["nodes"][updatedItem.index]["animation"].as<String>().c_str());
            animation = _state.data["nodes"][updatedItem.index]["animation"].as<String>();
            compileAndRun(animation);
        } else
            ESP_LOGD("", "no handle for %s.%s[%d] %s (%d %s)", updatedItem.parent.c_str(), updatedItem.name.c_str(), updatedItem.index, updatedItem.value.as<String>().c_str(), _state.data["driverOn"].as<bool>(), _state.data["nodes"][0]["animation"].as<String>());
    }

    void loop()
    {
        bool showLeds = false;
        
        for (JsonObject node: _state.data["nodes"].as<JsonArray>()) {
            String animation = node["animation"];
            //select the right effect
            if (animation == "Random") {
                fadeToBlackBy(leds, nrOfLeds, 70);
                leds[random16(nrOfLeds)] = CRGB(255, random8(), 0);
                showLeds = true;
            } else if (animation == "Rainbow") {
                static uint8_t hue = 0;
                fill_rainbow(leds, nrOfLeds, hue++, 7);
                showLeds = true;
            } else {
                //Done by live script (Yves)
            }
        }
        // Serial.printf(" %s", animation.c_str());

        if (showLeds) driverShow();
    }

    void driverShow()
    {
        if (_state.data["driverOn"])
            FastLED.show();
        // else 
        //     delay(1); //to avoid watchdog crash
    }

    void compileAndRun(String &animation) {

        ESP_LOGD("", "animation %s", animation.c_str());

        if (animation[0] != '/') { //no sc script
            return;
        }

        //if this animation is already running, kill it ...

        // ESP_LOGD("", "killAndFreeRunningProgram %s", animation.c_str());
        // runInLoopTask.push_back([&] { // run in loopTask to avoid stack overflow
        //     scriptRuntime.killAndFreeRunningProgram();
        // });

        //send UI spinner
    
        //run the recompile not in httpd but in main loopTask (otherwise we run out of stack space)
        runInLoopTask.push_back([&] {
            ESP_LOGD("", "compileAndRun %s (%d)", animation.c_str());
            File file = ESPFS.open(animation);
            if (file) {
                std::string scScript = file.readString().c_str();
                // scScript += "void main(){setup();sync();}";
                file.close();
    
                Executable executable = parser.parseScript(&scScript);
                executable.name = animation.c_str();
                ESP_LOGD("", "parsing %s done\n", animation.c_str());
                scriptRuntime.addExe(executable);
                ESP_LOGD("", "addExe success %s\n", executable.exeExist?"true":"false");
    
                if (executable.exeExist)
                    executable.execute("main"); //background task (async - vs sync)
                    
                for (Executable &exec: scriptRuntime._scExecutables) {
                    exe_info exeInfo = scriptRuntime.getExecutableInfo(exec.name);
                    ESP_LOGD("", "scriptRuntime exec %s r:%d h:%d, e:%d h:%d b:%d + d:%d = %d", exec.name.c_str(), exec.isRunning(), exec.isHalted, exec.exeExist, exec.__run_handle_index, exeInfo.binary_size, exeInfo.data_size, exeInfo.total_size);
                }
            }
        });
    
        //stop UI spinner
    }
};

#endif
#endif
