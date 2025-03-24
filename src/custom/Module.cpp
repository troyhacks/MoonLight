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

Module::Module(PsychicHttpServer *server,
                                     ESP32SvelteKit *sveltekit
                                     #if FT_ENABLED(FT_FILEMANAGER)
                                         , FilesService *filesService
                                     #endif
                                    ) : _httpEndpoint(ModuleState::read,
                                                                                                         ModuleState::update,
                                                                                                         this,
                                                                                                         server,
                                                                                                         "/rest/module",
                                                                                                         sveltekit->getSecurityManager(),
                                                                                                         AuthenticationPredicates::IS_AUTHENTICATED),
                                                                                           _eventEndpoint(ModuleState::read,
                                                                                                          ModuleState::update,
                                                                                                          this,
                                                                                                          sveltekit->getSocket(),
                                                                                                          "module"),
                                                                                           _webSocketServer(ModuleState::read,
                                                                                                            ModuleState::update,
                                                                                                            this,
                                                                                                            server,
                                                                                                            "/ws/module",
                                                                                                            sveltekit->getSecurityManager(),
                                                                                                            AuthenticationPredicates::IS_AUTHENTICATED),
                                                                                            _socket(sveltekit->getSocket()),
                                                                                             _fsPersistence(ModuleState::read,
                                                                                                      ModuleState::update,
                                                                                                      this,
                                                                                                      sveltekit->getFS(),
                                                                                                      "/config/module.json")
{
    ESP_LOGD("", "Module::constructor");
    _server = server;

    // configure settings service update handler to update state
    #if FT_ENABLED(FT_FILEMANAGER)
        _filesService = filesService;
    #endif
}

void Module::begin()
{
    ESP_LOGD("", "Module::begin");
    _httpEndpoint.begin();
    _eventEndpoint.begin();
    _fsPersistence.readFromFS(); //overwrites the default settings in state
    _server->on("/rest/moduleDef", HTTP_GET, [&](PsychicRequest *request) {
        PsychicJsonResponse response = PsychicJsonResponse(request, false);
        JsonArray root = response.getRoot().to<JsonArray>();

        _state.setupDefinition(root);

        char buffer[256];
        serializeJson(root, buffer, sizeof(buffer));
        ESP_LOGD("", "moduleDef %s", buffer);
    
        return response.send();
    });

    //if update, for all updated items, run onUpdate
    addUpdateHandler([&](const String &originId)
    {
        ESP_LOGD("", "Module::updateHandler %s", originId.c_str());
        read([&](ModuleState &state) {
            for (UpdatedItem updatedItem : state.updatedItems) {
                onUpdate(updatedItem);
            }
        });
    });

}

void Module::onUpdate(UpdatedItem updatedItem)
{
    if (updatedItem.name == "brightness") {
        ESP_LOGD("", "handle %s.%s[%d] %d", updatedItem.parent.c_str(), updatedItem.name.c_str(), updatedItem.index, updatedItem.value.as<int>());
    } else
        ESP_LOGD("", "no handle for %s.%s[%d] %s", updatedItem.parent.c_str(), updatedItem.name.c_str(), updatedItem.index, updatedItem.value.as<String>().c_str());
}

void Module::loop()
{
}

#endif

