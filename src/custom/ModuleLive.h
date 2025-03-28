/**
    @title     MoonLight
    @file      Module.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/custom/animation/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
**/

#ifndef ModuleLive_h
#define ModuleLive_h

#if FT_LIVEANIMATION == 1

#include "Module.h"

class ModuleLive : public Module
{
    // using Module::Module; //constructor inheritance
public:

    ModuleLive(PsychicHttpServer *server,
        ESP32SvelteKit *sveltekit
        #if FT_ENABLED(FT_FILEMANAGER)
            , FilesService *filesService
        #endif
    ) : Module("live", server, sveltekit, filesService) {
        ESP_LOGD("", "ModuleLive::constructor");
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

        char buffer[256];
        serializeJson(root, buffer, sizeof(buffer));
        ESP_LOGD("", "definition %s", buffer);
    }

    void onUpdate(UpdatedItem updatedItem) override
    {
        if (updatedItem.name == "lightsOn" || updatedItem.name == "brightness") {
            ESP_LOGD("", "handle %s.%s[%d] %s", updatedItem.parent.c_str(), updatedItem.name.c_str(), updatedItem.index, updatedItem.value.as<String>());
        } else if (updatedItem.parent == "nodes" && updatedItem.name == "animation" && updatedItem.index == 0) {    
            ESP_LOGD("", "handle %s.%s[%d] %s", updatedItem.parent.c_str(), updatedItem.name.c_str(), updatedItem.index, _state.data["nodes"][0]["animation"].as<String>().c_str());
        } else
            ESP_LOGD("", "no handle for %s.%s[%d] %s (%d %s)", updatedItem.parent.c_str(), updatedItem.name.c_str(), updatedItem.index, updatedItem.value.as<String>().c_str(), _state.data["driverOn"].as<bool>(), _state.data["nodes"][0]["animation"].as<String>());
    }

    void loop()
    {
    }

};

#endif
#endif
