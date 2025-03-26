/**
    @title     MoonLight
    @file      Module.cpp
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/moonlight/animation/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
**/

#if FT_LIVEANIMATION == 1

#include "Module.h"

void ModuleState::read(ModuleState &state, JsonObject &root)
{
    TaskHandle_t httpdTask = xTaskGetHandle("httpd");
    ESP_LOGI("", "Module::read task %s %d", pcTaskGetName(httpdTask), uxTaskGetStackHighWaterMark(httpdTask));

    root.set(state.data.as<JsonObject>()); //copy

    // char buffer[256];
    // serializeJson(root, buffer, sizeof(buffer));
    // ESP_LOGD("", "state.doc %s", buffer);

}

//Utility function?
void compareRecursive(JsonString parent, JsonVariant oldData, JsonVariant newData, std::vector<UpdatedItem> &updatedItems, int index = -1) {
    for (JsonPair oldProperty: oldData.as<JsonObject>()) { //loop over old properties
        JsonVariant newValue = newData[oldProperty.key().c_str()]; //find according new value
        if (oldProperty.value() != newValue) { //if value changed
            if (oldProperty.value().is<JsonArray>()) { // if the propery is an array
                JsonArray oldArray = oldProperty.value().as<JsonArray>();
                JsonArray newArray = newValue.as<JsonArray>();

                for (int i = 0; i < oldArray.size(); i++) { //compare each item in the array
                    compareRecursive(oldProperty.key().c_str(), oldArray[i], newArray[i], updatedItems, i);
                }
                //to do: oldArray size is not new array size
            } else { // if property is key/value
                ESP_LOGD("", "compareRecursive %s[%d]: %s -> %s", oldProperty.key().c_str(), index, oldProperty.value().as<String>().c_str(), newValue.as<String>().c_str());
                UpdatedItem newItem;
                newItem.parent = parent;
                newItem.name = oldProperty.key();
                newItem.value = newValue;
                newItem.index = index;
                updatedItems.push_back(newItem);
            }
        }
    }
}

StateUpdateResult ModuleState::update(JsonObject &root, ModuleState &state)
{
    
    if (root.size() != 0) { // in case of empty file
        TaskHandle_t httpdTask = xTaskGetHandle("httpd");
        ESP_LOGI("", "Module::update task %s %d", pcTaskGetName(httpdTask), uxTaskGetStackHighWaterMark(httpdTask));
        state.updatedItems.clear();

        //check which propertys have updated
        if (root != state.data) {
            compareRecursive("/", state.data, root, state.updatedItems);


            state.data.set(root); //copy

            // char buffer[256];
            // serializeJson(state.data, buffer, sizeof(buffer));
            // ESP_LOGD("", "state.doc %s", buffer);

            if (state.updatedItems.size())
                ESP_LOGD("", "Module::update %s", state.updatedItems.front().name.c_str());

            return state.updatedItems.size()?StateUpdateResult::CHANGED:StateUpdateResult::UNCHANGED;
        } else
            return StateUpdateResult::UNCHANGED;
    } else
        return StateUpdateResult::UNCHANGED;
}

#endif

