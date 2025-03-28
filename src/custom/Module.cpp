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

void ModuleState::setupData() {

    ESP_LOGD("", "ModuleState::setupData size %d", data.size());

    //only if no file ...
    if (data.size() == 0) {
        JsonDocument definition;
        if (setupDefinition) 
            setupDefinition(definition.to<JsonArray>());
        else
            ESP_LOGW("", "ModuleState::setupData no definition");

        char buffer[256];
        serializeJson(definition, buffer, sizeof(buffer));
        ESP_LOGD("", "setupDefinition %s", buffer);
        
        JsonObject root = data.to<JsonObject>();

        //create doc based on definition... only top level properties
        for (JsonObject property: definition.as<JsonArray>()) {
            if (property["type"] != "array") {
                root[property["name"]] = property["default"];
            } else {
                root[property["name"]].to<JsonArray>();
                //loop over detail propertys (recursive)
            }
        }

        // char buffer[256];
        serializeJson(root, buffer, sizeof(buffer));
        ESP_LOGD("", "setupData %s", buffer);
    }

    //to do: check if the file matches the definition
}

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
                    compareRecursive(oldProperty.key(), oldArray[i], newArray[i], updatedItems, i);
                }
                //to do: oldArray size is not new array size
            } else { // if property is key/value
                ESP_LOGD("", "compareRecursive %s[%d]: %s -> %s", oldProperty.key().c_str(), index, oldProperty.value().as<String>().c_str(), newValue.as<String>().c_str());
                UpdatedItem newItem;
                newItem.parent = String(parent.c_str());
                newItem.name = String(oldProperty.key().c_str());
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

Module::Module(String moduleName, PsychicHttpServer *server,
    ESP32SvelteKit *sveltekit
    #if FT_ENABLED(FT_FILEMANAGER)
        , FilesService *filesService
    #endif
  ): _httpEndpoint(ModuleState::read,
                      ModuleState::update,
                      this,
                      server,
                      String("/rest/" + moduleName).c_str(),
                      sveltekit->getSecurityManager(),
                      AuthenticationPredicates::IS_AUTHENTICATED),
      _eventEndpoint(ModuleState::read,
                      ModuleState::update,
                      this,
                      sveltekit->getSocket(),
                      moduleName.c_str()),
      _webSocketServer(ModuleState::read,
                      ModuleState::update,
                      this,
                      server,
                      String("/ws/" + moduleName).c_str(),
                      sveltekit->getSecurityManager(),
                      AuthenticationPredicates::IS_AUTHENTICATED),
      _socket(sveltekit->getSocket()),
          _fsPersistence(ModuleState::read,
                  ModuleState::update,
                  this,
                  sveltekit->getFS(),
                  String("/config/" + moduleName + ".json").c_str())
{
    _moduleName = moduleName;

    ESP_LOGD("", "Module::constructor %s", moduleName.c_str());
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

    _state.setupDefinition = [&](JsonArray root) {
        this->setupDefinition(root); //using here means setupDefinition must be virtual ...
    };
    // state.setupDefinition = this->setupDefinition;
    _state.setupData(); //if no data readFromFS, using overridden setupDefinition

    _server->on(String("/rest/" + _moduleName + "Def").c_str(), HTTP_GET, [&](PsychicRequest *request) {
        PsychicJsonResponse response = PsychicJsonResponse(request, false);
        JsonArray root = response.getRoot().to<JsonArray>();

        setupDefinition(root);

        char buffer[256];
        serializeJson(root, buffer, sizeof(buffer));
        ESP_LOGD("", "moduleDef %s", buffer);
    
        return response.send();
    });

    //if update, for all updated items, run onUpdate
    this->addUpdateHandler([&](const String &originId)
    {
        ESP_LOGD("", "Module::updateHandler %s", originId.c_str());
        for (UpdatedItem updatedItem : _state.updatedItems) {
            onUpdate(updatedItem);
        }
    });

}

void Module::setupDefinition(JsonArray root) { //virtual so it can be overriden in derived classes
    ESP_LOGW("", "setupDefinition not implemented");
    JsonObject property;
    JsonArray details;
    JsonArray values;

    property = root.add<JsonObject>(); property["name"] = "text"; property["type"] = "text"; property["default"] = "MoonLight";
}

void Module::onUpdate(UpdatedItem updatedItem)
{
    ESP_LOGW("", "onUpdate not implemented %s.%s[%d] %s", updatedItem.parent.c_str(), updatedItem.name.c_str(), updatedItem.index, updatedItem.value.as<String>().c_str());
}

#endif

