/**
    @title     MoonBase
    @file      ModuleChannels.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/moonbase/module/Channels/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact us for more information.
**/

#ifndef ModuleChannels_h
#define ModuleChannels_h

#if FT_MOONBASE == 1

#include "../MoonBase/Module.h"
class ModuleChannels : public Module
{
public:

    ModuleChannels(PsychicHttpServer *server,
            ESP32SvelteKit *sveltekit
        ) : Module("channels", server, sveltekit) {
            ESP_LOGD(TAG, "constructor");
    }

    void setupDefinition(JsonArray root) override{
        ESP_LOGD(TAG, "");
        JsonObject property; // state.data has one or more properties
        // JsonArray details; // if a property is an array, this is the details of the array
        JsonArray values; // if a property is a select, this is the values of the select

        property = root.add<JsonObject>(); property["name"] = "view"; property["type"] = "select"; property["default"] = 0; values = property["values"].to<JsonArray>();
        values.add("Physical layer");
        uint8_t i = 0;
        for (VirtualLayer * layer: layerP.layerV) {
            Char<32> layerName;
            layerName.format("Virtual layer %d", i);
            values.add(layerName.c_str());
            i++;
        }
        property = root.add<JsonObject>(); property["name"] = "group"; property["type"] = "checkbox"; property["default"] = true;

        property = root.add<JsonObject>(); property["name"] = "channel"; property["type"] = "pad"; property["width"] = 12; property["hoverToServer"] = true; property["size"] = 10;
        property["default"]["select"] = 0;
        property["default"]["action"] = "";
        property["default"]["list"].to<JsonArray>();
        property["default"]["count"] = 512;
    }

    void onUpdate(UpdatedItem &updatedItem) override
    {
        uint8_t view = _state.data["view"];
        bool group =  _state.data["group"];

        if (updatedItem.name == "view" || updatedItem.name == "group") {
            uint16_t count = view==0?layerP.lights.header.nrOfLights: layerP.layerV[view-1]->nrOfLights;
            if (!group) count *= layerP.lights.header.channelsPerLight;
            if (count > 512) count = 512;
            _state.data["channel"]["count"] = count;
            ESP_LOGD(TAG, "set count %d", count);

            // update state to UI
            update([&](ModuleState &state) {
                return StateUpdateResult::CHANGED; // notify StatefulService by returning CHANGED
            }, "server");
        } else if (updatedItem.name == "channel") {
            //copy the file to the hidden folder...
            if (updatedItem.oldValue != "null" && !updatedItem.value["action"].isNull()) {
                ESP_LOGD(TAG, "handle %s[%d]%s[%d].%s = %s -> %s", updatedItem.parent[0], updatedItem.index[0], updatedItem.parent[1], updatedItem.index[1], updatedItem.name, updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
                uint16_t select = updatedItem.value["select"];
                uint8_t value = updatedItem.value["action"] == "mouseenter"?255:0;
                if (view == 0) {
                    if (group)
                        for (uint8_t i = 0; i < layerP.lights.header.channelsPerLight; i++)
                            layerP.lights.channels[select*layerP.lights.header.channelsPerLight+i] = value;
                    else
                        layerP.lights.channels[select] = value;
                } else {
                    if (group)
                        for (uint8_t i = 0; i < layerP.lights.header.channelsPerLight; i++)
                            layerP.layerV[view-1]->setLight(select, &value, i, 1);
                    else
                        layerP.layerV[view-1]->setLight(select/layerP.lights.header.channelsPerLight , &value, select%layerP.lights.header.channelsPerLight, 1);
                }
            }
        } else
            ESP_LOGD(TAG, "no handle for %s[%d]%s[%d].%s = %s -> %s", updatedItem.parent[0], updatedItem.index[0], updatedItem.parent[1], updatedItem.index[1], updatedItem.name, updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
    }
};

#endif
#endif