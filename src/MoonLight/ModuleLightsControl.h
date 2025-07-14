/**
    @title     MoonLight
    @file      ModuleLightsControl.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/moonbase/module/lightsControl/
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
    FileManager *_fileManager;

    ModuleLightsControl(PsychicHttpServer *server,
        ESP32SvelteKit *sveltekit,
        FileManager *fileManager
    ) : Module("lightsControl", server, sveltekit) {
        ESP_LOGD(TAG, "constructor");
        _server = server;
        _fileManager = fileManager;
    }

    void begin() {
        Module::begin();

        ESP_LOGD(TAG, "Lights:%d(Header:%d) L-H:%d Node:%d PL:%d(PL-L:%d) VL:%d PM:%d C3D:%d", sizeof(Lights), sizeof(LightsHeader), sizeof(Lights) - sizeof(LightsHeader), sizeof(Node), sizeof(PhysicalLayer), sizeof(PhysicalLayer)-sizeof(Lights), sizeof(VirtualLayer), sizeof(PhysMap), sizeof(Coord3D));

        setPresetsFromFolder(); //set the right values during boot
        
        //update presets if files changed in presets folder
        _fileManager->addUpdateHandler([&](const String &originId)
        { 
            ESP_LOGD(TAG, "FileManager::updateHandler %s", originId.c_str());
            //read the file state (read all files and folders on FS and collect changes)
            _fileManager->read([&](FilesState &filesState) {
                // loop over all changed files (normally only one)
                bool presetChanged = false;
                for (auto updatedItem : filesState.updatedItems) {
                    //if file is the current live script, recompile it (to do: multiple live effects)
                    ESP_LOGD(TAG, "updateHandler updatedItem %s", updatedItem.c_str());
                    if (strstr(updatedItem.c_str(), "/.config/editor")) {
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
        property = root.add<JsonObject>(); property["name"] = "preset"; property["type"] = "pad";  property["width"] = 8; property["size"] = 18;
        property["default"].to<JsonObject>(); // clear the preset array before adding new presets
        property["default"]["list"].to<JsonArray>();
        property["default"]["count"] = 64;

        #if FT_ENABLED(FT_MONITOR)
            property = root.add<JsonObject>(); property["name"] = "monitorOn"; property["type"] = "checkbox"; property["default"] = true;
        #endif

    }

    //implement business logic
    void onUpdate(UpdatedItem &updatedItem) override
    {
        if (updatedItem.name == "red") {
            ESP_LOGD(TAG, "handle %s[%d]%s[%d].%s = %s -> %s", updatedItem.parent[0], updatedItem.index[0], updatedItem.parent[1], updatedItem.index[1], updatedItem.name, updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
            layerP.lights.header.red = _state.data["red"];
        } else if (updatedItem.name == "green") {
            ESP_LOGD(TAG, "handle %s[%d]%s[%d].%s = %s -> %s", updatedItem.parent[0], updatedItem.index[0], updatedItem.parent[1], updatedItem.index[1], updatedItem.name, updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
            layerP.lights.header.green = _state.data["green"];
        } else if (updatedItem.name == "blue") {
            ESP_LOGD(TAG, "handle %s[%d]%s[%d].%s = %s -> %s", updatedItem.parent[0], updatedItem.index[0], updatedItem.parent[1], updatedItem.index[1], updatedItem.name, updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
            layerP.lights.header.blue = _state.data["blue"];
        } else if (updatedItem.name == "lightsOn" || updatedItem.name == "brightness") {
            ESP_LOGD(TAG, "handle %s[%d]%s[%d].%s = %s -> %s", updatedItem.parent[0], updatedItem.index[0], updatedItem.parent[1], updatedItem.index[1], updatedItem.name, updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
            layerP.lights.header.brightness = _state.data["lightsOn"]?_state.data["brightness"]:0;
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
            //copy /.config/editor.json to the hidden folder /.config/editor/preset[x].json
            //do not set preset at boot...
            if (updatedItem.oldValue != "null" && !updatedItem.value["action"].isNull()) {
                uint16_t select = updatedItem.value["select"];
                Char<32> presetFile;
                presetFile.format("/.config/editor/preset%02d.json", select);
                ESP_LOGD(TAG, "handle %s[%d]%s[%d].%s = %s -> %s", updatedItem.parent[0], updatedItem.index[0], updatedItem.parent[1], updatedItem.index[1], updatedItem.name, updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());

                if (updatedItem.value["action"] == "click") {
                    updatedItem.value["selected"] = select; //store the selected preset
                    if (arrayContainsValue(updatedItem.value["list"], select)) {
                        copyFile(presetFile.c_str(), "/.config/editor.json");

                        //trigger notification of update of editor.json
                        _fileManager->update([&](FilesState &state) {
                            state.updatedItems.push_back("/.config/editor.json");
                            return StateUpdateResult::CHANGED; // notify StatefulService by returning CHANGED
                        }, "server");

                    } else {
                        copyFile("/.config/editor.json", presetFile.c_str());
                        setPresetsFromFolder(); //update presets in UI
                    }
                } else if (updatedItem.value["action"] == "dblclick") {
                    ESPFS.remove(presetFile.c_str());
                    setPresetsFromFolder(); //update presets in UI
                }
            }
        } else 
            ESP_LOGD(TAG, "no handle for %s[%d]%s[%d].%s = %s -> %s", updatedItem.parent[0], updatedItem.index[0], updatedItem.parent[1], updatedItem.index[1], updatedItem.name, updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
    }

    void loop50ms() {
        #if FT_ENABLED(FT_MONITOR)

            static int monitorMillis = 0;
            if (millis() - monitorMillis >= layerP.lights.header.nrOfLights / 12) { //max 12000 leds per second -> 1 second for 12000 leds
                monitorMillis = millis();
                
                if (layerP.lights.header.isPositions == 10) { //send to UI
                    if (_socket->getConnectedClients() && _state.data["monitorOn"])
                        _socket->emitEvent("monitor", (char *)&layerP.lights, sizeof(LightsHeader) + MIN(layerP.lights.header.nrOfLights * 3, MAX_CHANNELS));
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
        File rootFolder = ESPFS.open("/.config/editor/");
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
            // requestUIUpdate ...
            update([&](ModuleState &state) {
                return StateUpdateResult::CHANGED; // notify StatefulService by returning CHANGED
            }, "server");
        }
    }

}; // class ModuleLightsControl

#endif
#endif