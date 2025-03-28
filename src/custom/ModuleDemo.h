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

#ifndef ModuleDemo_h
#define ModuleDemo_h

#if FT_LIVEANIMATION == 1

#include "Module.h"

class ModuleDemo : public Module
{
public:

ModuleDemo(PsychicHttpServer *server,
        ESP32SvelteKit *sveltekit
        #if FT_ENABLED(FT_FILEMANAGER)
            , FilesService *filesService
        #endif
    ) : Module("demo", server, sveltekit, filesService) {
        ESP_LOGD("", "ModuleDemo::constructor");
    }

    void setupDefinition(JsonArray root) override{
        JsonObject property;
        JsonArray details;
        JsonArray values;

        //remaining is for demo purposes (temp)
        property = root.add<JsonObject>(); property["name"] = "hostName"; property["type"] = "text"; property["default"] = "MoonLight";
        property = root.add<JsonObject>(); property["name"] = "connectionMode"; property["type"] = "select"; property["default"] = "Signal Strength"; values = property["values"].to<JsonArray>();
        values.add("Offline");
        values.add("Signal Strength");
        values.add("Priority");

        property = root.add<JsonObject>(); property["name"] = "savedNetworks"; property["type"] = "array"; details = property["n"].to<JsonArray>();
        {
            property = details.add<JsonObject>(); property["name"] = "SSID"; property["type"] = "text"; property["default"] = "ewtr"; property["min"] = 3; property["max"] = 32; 
            property = details.add<JsonObject>(); property["name"] = "Password"; property["type"] = "password"; property["default"] = "";
        }

        property = root.add<JsonObject>(); property["name"] = "invoices"; property["type"] = "array"; details = property["n"].to<JsonArray>();
        {
            property = details.add<JsonObject>(); property["name"] = "number"; property["type"] = "number"; property["default"] = 1000; property["min"] = 1000; property["max"] = 9999; 
            property = details.add<JsonObject>(); property["name"] = "name"; property["type"] = "text"; property["default"] = "ewowi";
            property = details.add<JsonObject>(); property["name"] = "date"; property["type"] = "date"; property["default"] = "2025-03-21";
            property = details.add<JsonObject>(); property["name"] = "lines"; property["type"] = "array"; details = property["n"].to<JsonArray>();
            {
                property = details.add<JsonObject>(); property["name"] = "service"; property["type"] = "text"; property["default"] = "consulting";
                property = details.add<JsonObject>(); property["name"] = "price"; property["type"] = "number"; property["default"] = 100;
            }
        }

        char buffer[256];
        serializeJson(root, buffer, sizeof(buffer));
        ESP_LOGD("", "definition %s", buffer);
    }

    void onUpdate(UpdatedItem updatedItem) override
    {
        ESP_LOGD("", "no handle for %s.%s[%d] %s", updatedItem.parent.c_str(), updatedItem.name.c_str(), updatedItem.index, updatedItem.value.as<String>().c_str());
    }
};

#endif
#endif
