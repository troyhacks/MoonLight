/**
    @title     MoonBase
    @file      ModuleChannelView.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/modules/module/demo/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
**/

#ifndef ModuleChannelView_h
#define ModuleChannelView_h

#if FT_MOONBASE == 1

#include "../MoonBase/Module.h"
class ModuleChannelView : public Module
{
public:

    ModuleChannelView(PsychicHttpServer *server,
            ESP32SvelteKit *sveltekit,
            FilesService *filesService
        ) : Module("ChannelView", server, sveltekit, filesService) {
            ESP_LOGD(TAG, "constructor");
    }

    void setupDefinition(JsonArray root) override{
        ESP_LOGD(TAG, "");
        JsonObject property; // state.data has one or more properties
        // JsonArray details; // if a property is an array, this is the details of the array
        // JsonArray values; // if a property is a select, this is the values of the select

        property = root.add<JsonObject>(); property["name"] = "channel"; property["type"] = "pad"; property["width"] = 12; property["height"] = 64; property["hover"] = true; property["size"] = 10;
    }

    void onUpdate(UpdatedItem &updatedItem) override
    {
        if (updatedItem.name == "channel") {
            ESP_LOGD(TAG, "handle %s[%d]%s[%d].%s = %s -> %s", updatedItem.parent[0], updatedItem.index[0], updatedItem.parent[1], updatedItem.index[1], updatedItem.name, updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
            //copy the file to the hidden folder...
            layerP.lights.header.brightness;
        } else
            ESP_LOGD(TAG, "no handle for %s[%d]%s[%d].%s = %s -> %s", updatedItem.parent[0], updatedItem.index[0], updatedItem.parent[1], updatedItem.index[1], updatedItem.name, updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
    }
};

#endif
#endif