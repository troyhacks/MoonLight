/**
    @title     MoonLight
    @file      ModuleLightsControl.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/modules/module/lightsControl/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
**/

#ifndef ModuleLightsControl_h
#define ModuleLightsControl_h

#if FT_MOONLIGHT

#undef TAG
#define TAG "💫"

#include "FastLED.h"
#include "../MoonBase/Module.h"

#include "ModuleEditor.h"

class ModuleLightsControl : public Module
{
public:

    PsychicHttpServer *_server;

    ModuleLightsControl(PsychicHttpServer *server,
        ESP32SvelteKit *sveltekit,
        FilesService *filesService
    ) : Module("lightsControl", server, sveltekit, filesService) {
        ESP_LOGD(TAG, "constructor");
        _server = server;
    }

    void begin() {
        Module::begin();

        ESP_LOGD(TAG, "L:%d(%d) LH:%d N:%d PL:%d(%d) VL:%d", sizeof(Lights), sizeof(LightsHeader), sizeof(Lights) - sizeof(LightsHeader), sizeof(Node), sizeof(PhysicalLayer), sizeof(PhysicalLayer)-sizeof(Lights), sizeof(VirtualLayer));

        setPresetsFromFolder(); //set the right values during boot
        
        //update presets if files changed in presets folder
        _filesService->addUpdateHandler([&](const String &originId)
        { 
            ESP_LOGD(TAG, "FilesService::updateHandler %s", originId.c_str());
            //read the file state (read all files and folders on FS and collect changes)
            _filesService->read([&](FilesState &filesState) {
                // loop over all changed files (normally only one)
                bool presetChanged = false;
                for (auto updatedItem : filesState.updatedItems) {
                    //if file is the current live script, recompile it (to do: multiple live effects)
                    ESP_LOGD(TAG, "updateHandler updatedItem %s", updatedItem.c_str());
                    if (strstr(updatedItem.c_str(), "/config/.editor")) {
                        ESP_LOGD(TAG, " preset.json updated -> call update %s", updatedItem.c_str());
                        presetChanged = true;
                    }
                }
                if (presetChanged) {
                    ESP_LOGD(TAG, "setPresetsFromFolder");
                    setPresetsFromFolder(); // update the presets from the folder
                }
            });
        });
    }

    //define the data model
    void setupDefinition(JsonArray root) override {
        ESP_LOGD(TAG, "");
        JsonObject property; // state.data has one or more properties
        JsonArray details = root; // if a property is an array, this is the details of the array
        JsonArray values; // if a property is a select, this is the values of the select

        property = root.add<JsonObject>(); property["name"] = "lightsOn"; property["type"] = "checkbox"; property["default"] = true;
        property = root.add<JsonObject>(); property["name"] = "brightness"; property["type"] = "range"; property["default"] = 10;
        property = root.add<JsonObject>(); property["name"] = "red"; property["type"] = "range"; property["default"] = 255; property["color"] = "Red";
        property = root.add<JsonObject>(); property["name"] = "green"; property["type"] = "range"; property["default"] = 255; property["color"] = "Green";
        property = root.add<JsonObject>(); property["name"] = "blue"; property["type"] = "range"; property["default"] = 255; property["color"] = "Blue";
        property = root.add<JsonObject>(); property["name"] = "palette"; property["type"] = "select"; property["default"] = 6; values = property["values"].to<JsonArray>();
        values.add("CloudColors");
        values.add("LavaColors");
        values.add("OceanColors");
        values.add("ForestColors");
        values.add("RainbowColors");
        values.add("RainbowStripeColors");
        values.add("PartyColors");
        values.add("HeatColors");
        values.add("RandomColors");
        property = root.add<JsonObject>(); property["name"] = "preset"; property["type"] = "pad";  property["width"] = 8; property["height"] = 8; property["size"] = 18;
        property["default"].to<JsonObject>(); // clear the preset array before adding new presets
        property["default"]["active"] = 1;
        property["default"]["list"].to<JsonArray>();

        property = root.add<JsonObject>(); property["name"] = "driverOn"; property["type"] = "checkbox"; property["default"] = true;

        #if FT_ENABLED(FT_MONITOR)
            property = root.add<JsonObject>(); property["name"] = "monitorOn"; property["type"] = "checkbox"; property["default"] = true;
        #endif

    }

    //implement business logic
    void onUpdate(UpdatedItem &updatedItem) override
    {
        if (updatedItem.name == "red" || updatedItem.name == "green" || updatedItem.name == "blue") {
            ESP_LOGD(TAG, "handle %s[%d]%s[%d].%s = %s -> %s", updatedItem.parent[0], updatedItem.index[0], updatedItem.parent[1], updatedItem.index[1], updatedItem.name, updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
            layerP.ledsDriver.setColorCorrection(_state.data["red"], _state.data["green"], _state.data["blue"]);
        } else if (updatedItem.name == "lightsOn" || updatedItem.name == "brightness") {
            ESP_LOGD(TAG, "handle %s[%d]%s[%d].%s = %s -> %s", updatedItem.parent[0], updatedItem.index[0], updatedItem.parent[1], updatedItem.index[1], updatedItem.name, updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
            layerP.lights.header.brightness = _state.data["lightsOn"]?_state.data["brightness"]:0;
            layerP.ledsDriver.setBrightness(layerP.lights.header.brightness);
        } else if (updatedItem.name == "palette") {
            ESP_LOGD(TAG, "handle %s[%d]%s[%d].%s = %s -> %s", updatedItem.parent[0], updatedItem.index[0], updatedItem.parent[1], updatedItem.index[1], updatedItem.name, updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
            // String value = _state.data["palette"];//updatedItem.oldValue;
            if (updatedItem.value == 0) layerP.palette = CloudColors_p;
            else if (updatedItem.value == 1) layerP.palette = LavaColors_p;
            else if (updatedItem.value == 2) layerP.palette = OceanColors_p;
            else if (updatedItem.value == 3) layerP.palette = ForestColors_p;
            else if (updatedItem.value == 4) layerP.palette = RainbowColors_p;
            else if (updatedItem.value == 5) layerP.palette = RainbowStripeColors_p;
            else if (updatedItem.value == 6) layerP.palette = PartyColors_p;
            else if (updatedItem.value == 7) layerP.palette = HeatColors_p;
            else if (updatedItem.value == 8) {
                for (int i=0; i < sizeof(layerP.palette.entries) / sizeof(CRGB); i++) {
                    layerP.palette[i] = CHSV(random8(), 255, 255); //take the max saturation, max brightness of the colorwheel
                }
            }
            else {
                layerP.palette = PartyColors_p; //should never occur
            }
            // layerP.palette = LavaColors_p;
        } else if (updatedItem.name == "preset") {
            ESP_LOGD(TAG, "handle %s[%d]%s[%d].%s = %s -> %s", updatedItem.parent[0], updatedItem.index[0], updatedItem.parent[1], updatedItem.index[1], updatedItem.name, updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
            //copy /config/animations.json to the hidden folder /config/.animations/preset[x].json
            if (!updatedItem.value["active"].isNull()) {
                Char<32> presetFile;
                presetFile.format("/config/.editor/preset%02d.json", updatedItem.value["active"].as<int>());

                copyFile("/config/editor.json", presetFile.c_str());

                setPresetsFromFolder(); //update presets in UI
            }

        } else 
            ESP_LOGD(TAG, "no handle for %s[%d]%s[%d].%s = %s -> %s", updatedItem.parent[0], updatedItem.index[0], updatedItem.parent[1], updatedItem.index[1], updatedItem.name, updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
    }

    //ledsDriver.show
    void loop() {
        if (layerP.lights.header.isPositions == 0 && _state.data["driverOn"])
            layerP.ledsDriver.show();
    }

    void loop50ms() {
        #if FT_ENABLED(FT_MONITOR)

            static int monitorMillis = 0;
            if (millis() - monitorMillis >= layerP.lights.header.nrOfLights / 12) { //max 12000 leds per second -> 1 second for 12000 leds
                monitorMillis = millis();
                
                if (layerP.lights.header.isPositions == 10) { //send to UI
                    if (_socket->getConnectedClients() && _state.data["monitorOn"])
                        _socket->emitEvent("monitor", (char *)&layerP.lights, sizeof(LightsHeader) + MIN(layerP.lights.header.nrOfLights * sizeof(Coord3D), MAX_CHANNELS));
                    layerP.lights.header.isPositions = 0;
                } else if (layerP.lights.header.isPositions == 0) {//send to UI
                    if (_socket->getConnectedClients() && _state.data["monitorOn"])
                        _socket->emitEvent("monitor", (char *)&layerP.lights, sizeof(LightsHeader) + MIN(layerP.lights.header.nrOfLights * layerP.lights.header.channelsPerLight, MAX_CHANNELS));
                }
            }
        #endif
    }

    //update _state.data["preset"]["list"] and send update to endpoints
    void setPresetsFromFolder() {
        //loop over all files in the presets folder and add them to the preset array
        File rootFolder = ESPFS.open("/config/.editor/");
        _state.data["preset"]["list"].to<JsonArray>(); // clear the active preset array before adding new presets
        bool changed = false;
        walkThroughFiles(rootFolder, [&](File folder, File file) {
            int seq = -1;
            if (sscanf(file.name(), "preset%02d.json", &seq) == 1) {
                // seq now contains the 2-digit number, e.g., 34
                ESP_LOGD(TAG, "Preset %d found", seq);
                _state.data["preset"]["list"].add(seq); // add the preset to the preset array
                changed = true;
            }
        });

        if (changed) {
            update([&](ModuleState &state) {
                return StateUpdateResult::CHANGED; // notify StatefulService by returning CHANGED
            }, "server");
        }
    }

}; // class ModuleLightsControl

#endif
#endif