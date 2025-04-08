/**
    @title     MoonBase
    @file      ModuleAnimations.h
    @repo      https://github.com/ewowi/MoonBase, submit changes to this file as PRs
    @Authors   https://github.com/ewowi/MoonBase/commits/main
    @Doc       https://ewowi.github.io/MoonBase/modules/module/animations/
    @Copyright © 2025 Github MoonBase Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
**/

#ifndef ModuleAnimations_h
#define ModuleAnimations_h

#if FT_MOONLIGHT == 1

#include "Module.h"

#undef TAG
#define TAG "💫"

#include "FastLED.h"
#define MAXLEDS 8192

#if FT_LIVESCRIPT
    #include "ESPLiveScript.h" //note: contains declarations AND definitions, therefore can only be included once!
#endif

class ModuleAnimations : public Module
{
public:

    CRGB leds[MAXLEDS];
    uint16_t nrOfLeds = 256;
    #if FT_LIVESCRIPT
        Parser parser = Parser();
    #endif

    ModuleAnimations(PsychicHttpServer *server,
        ESP32SvelteKit *sveltekit
        #if FT_ENABLED(FT_FILEMANAGER)
            , FilesService *filesService
        #endif
    ) : Module("animations", server, sveltekit, filesService) {
        ESP_LOGD(TAG, "constructor");

        //In constructor so before onUpdate
        FastLED.addLeds<WS2812B, 16, GRB>(leds, 0, nrOfLeds); //pin 2
        FastLED.setMaxPowerInMilliWatts(10000); // 5v, 2000mA, to protect usb while developing
        FastLED.setBrightness(0);
        ESP_LOGD(TAG, "FastLED.addLeds n:%d", nrOfLeds);
    }

    void begin() {
        Module::begin();

        ESP_LOGD(TAG, "");

        #if FT_ENABLED(FT_FILEMANAGER)

        //create a handler which recompiles the animation when the file of the current animation changes
        _filesService->addUpdateHandler([&](const String &originId)
        { 
            ESP_LOGD(TAG, "FilesService::updateHandler %s", originId.c_str());
            //read the file state
            _filesService->read([&](FilesState &filesState) {
                // loop over all changed files (normally only one)
                for (auto updatedItem : filesState.updatedItems) {
                    //if file is the current animation, recompile it (to do: multiple animations)
                    for (JsonObject node: _state.data["nodes"].as<JsonArray>()) {
                        String animation = node["animation"];

                        if (updatedItem == animation) {
                            ESP_LOGD(TAG, "updateHandler updatedItem %s", updatedItem.c_str());
                            compileAndRun(animation.c_str());
                        }
                    }
                }
            });
        });

        #endif

        _socket->registerEvent("livescripts");
    }

    void setupDefinition(JsonArray root) override {
        ESP_LOGD(TAG, "");
        JsonObject property;
        JsonArray details;
        JsonArray values;

        property = root.add<JsonObject>(); property["name"] = "lightsOn"; property["type"] = "checkbox"; property["default"] = true;
        property = root.add<JsonObject>(); property["name"] = "brightness"; property["type"] = "range"; property["min"] = 0; property["max"] = 255; property["default"] = 10;
        property = root.add<JsonObject>(); property["name"] = "driverOn"; property["type"] = "checkbox"; property["default"] = true;
        property = root.add<JsonObject>(); property["name"] = "nodes"; property["type"] = "array"; details = property["n"].to<JsonArray>();
        {
            property = details.add<JsonObject>(); property["name"] = "animation"; property["type"] = "selectFile"; property["default"] = "Random"; values = property["values"].to<JsonArray>();
            values.add("Random");
            values.add("Sinelon");
            values.add("Rainbow");
            //find all the .sc files on FS
            #if FT_ENABLED(FT_FILEMANAGER)
                File rootFolder = ESPFS.open("/");
                walkThroughFiles(rootFolder, [&](File folder, File file) {
                    if (strstr(file.name(), ".sc")) {
                        // ESP_LOGD(TAG, "found file %s", file.path());
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
            property = details.add<JsonObject>(); property["name"] = "controls"; property["type"] = "array"; details = property["n"].to<JsonArray>();
            {
                property = details.add<JsonObject>(); property["name"] = "name"; property["type"] = "text"; property["default"] = "speed";
                property = details.add<JsonObject>(); property["name"] = "type"; property["type"] = "select"; property["default"] = "Number"; values = property["values"].to<JsonArray>();
                values.add("Number");
                values.add("Range");
                values.add("Text");
                values.add("Coordinate");
                property = details.add<JsonObject>(); property["name"] = "value"; property["type"] = "text"; property["default"] = "128";
            }
            //, "controls":[{"name":"speed","type":"range", "value":"128"}]
        }

        property = root.add<JsonObject>(); property["name"] = "scripts"; property["type"] = "array"; details = property["n"].to<JsonArray>();
        {
            property = details.add<JsonObject>(); property["name"] = "name"; property["type"] = "text";
            property = details.add<JsonObject>(); property["name"] = "isRunning"; property["type"] = "checkbox";
            property = details.add<JsonObject>(); property["name"] = "isHalted"; property["type"] = "checkbox";
            property = details.add<JsonObject>(); property["name"] = "exeExist"; property["type"] = "checkbox";
            property = details.add<JsonObject>(); property["name"] = "handle"; property["type"] = "number";
            property = details.add<JsonObject>(); property["name"] = "binary_size"; property["type"] = "number";
            property = details.add<JsonObject>(); property["name"] = "data_size"; property["type"] = "number";
            property = details.add<JsonObject>(); property["name"] = "kill"; property["type"] = "button";
        }
    }

    void onUpdate(UpdatedItem updatedItem) override
    {
        if (equal(updatedItem.name, "lightsOn") || equal(updatedItem.name, "brightness")) {
            ESP_LOGD(TAG, "handle %s = %s -> %s", updatedItem.name, updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
            FastLED.setBrightness(_state.data["lightsOn"]?_state.data["brightness"]:0);
        } else if (equal(updatedItem.parent[0], "nodes") && equal(updatedItem.name, "animation")) {    
            ESP_LOGD(TAG, "handle %s = %s -> %s", updatedItem.name, updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
            if (updatedItem.oldValue.length())
                ESP_LOGD(TAG, "delete %s ...", updatedItem.oldValue.c_str());
            if (updatedItem.value.as<String>().length())
                compileAndRun(updatedItem.value.as<String>().c_str());
        } else
            ESP_LOGD(TAG, "no handle for %s = %s -> %s", updatedItem.name, updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
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
            } else if (animation == "Sinelon") {
                fadeToBlackBy(leds, nrOfLeds, 20);
                uint8_t bpm = 60;
                int pos = beatsin16( bpm, 0, 255 );
                leds[pos] += CHSV( millis()/50, 255, 255); //= CRGB(255, random8(), 0);
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
    }

    void compileAndRun(const char * animation) {

        #if FT_LIVESCRIPT
        
            ESP_LOGD(TAG, "animation %s", animation);

            if (animation[0] != '/') { //no sc script
                return;
            }

            //if this animation is already running, kill it ...

            // ESP_LOGD(TAG, "killAndFreeRunningProgram %s", animation.c_str());
            // runInLoopTask.push_back([&] { // run in loopTask to avoid stack overflow
            //     scriptRuntime.killAndFreeRunningProgram();
            // });

            //send UI spinner
        
            //run the recompile not in httpd but in main loopTask (otherwise we run out of stack space)
            // runInLoopTask.push_back([&] {
                ESP_LOGD(TAG, "compileAndRun %s", animation);
                File file = ESPFS.open(animation);
                if (file) {
                    std::string scScript = file.readString().c_str();
                    // scScript += "void main(){setup();sync();}";
                    file.close();
        
                    Executable executable = parser.parseScript(&scScript);
                    executable.name = string(animation);
                    ESP_LOGD(TAG, "parsing %s done\n", animation);
                    scriptRuntime.addExe(executable);
                    ESP_LOGD(TAG, "addExe success %s\n", executable.exeExist?"true":"false");
        
                    if (executable.exeExist)
                        executable.execute("main"); //background task (async - vs sync)

                }
            // });
        #endif
    
        //stop UI spinner
    }
};

#endif
#endif
