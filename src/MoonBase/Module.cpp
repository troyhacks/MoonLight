/**
    @title     MoonBase
    @file      Module.cpp
    @repo      https://github.com/ewowi/MoonBase, submit changes to this file as PRs
    @Authors   https://github.com/ewowi/MoonBase/commits/main
    @Doc       https://ewowi.github.io/MoonBase/modules/modules/
    @Copyright © 2025 Github MoonBase Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
**/

#if FT_MOONBASE == 1

#include "Module.h"

void setDefaults(JsonObject root, JsonArray definition) {
    for (JsonObject property: definition) {
        if (property["type"] != "array") {
            root[property["name"]] = property["default"];
        } else {
            JsonArray array = root[property["name"]].to<JsonArray>();
            //loop over detail propertys (recursive)
            JsonObject object = array.add<JsonObject>(); // add one row
            setDefaults(object, property["n"].as<JsonArray>());
        }
    }
}

void ModuleState::setupData() {

    //only if no file ...
    if (data.size() == 0) {
        ESP_LOGD(TAG, "size %d", data.size());
        JsonDocument definition;
        if (setupDefinition) 
            setupDefinition(definition.to<JsonArray>());
        else
            ESP_LOGW(TAG, "no definition");

        JsonObject root = data.to<JsonObject>();

        setDefaults(root, definition.as<JsonArray>());
    }

    //to do: check if the file matches the definition
}

void ModuleState::read(ModuleState &state, JsonObject &root)
{
    TaskHandle_t currentTask = xTaskGetCurrentTaskHandle();
    ESP_LOGI(TAG, "task %s %d", pcTaskGetName(currentTask), uxTaskGetStackHighWaterMark(currentTask));

    root.set(state.data.as<JsonObject>()); //copy
}

bool ModuleState::compareRecursive(JsonString parent, JsonVariant stateData, JsonVariant newData, UpdatedItem &updatedItem, uint8_t depth, uint8_t index) {
    bool changed = false;
    for (JsonPair newProperty : newData.as<JsonObject>()) {
        if (stateData[newProperty.key()].isNull()) { 
            stateData[newProperty.key()] = nullptr; // Initialize the key in stateData if it doesn't exist
        }
    }

    for (JsonPair stateProperty : stateData.as<JsonObject>()) {
        JsonString key = stateProperty.key();
        JsonVariant stateValue = stateData[key.c_str()];
        JsonVariant newValue = newData[key.c_str()];
        if (stateValue != newValue) { //if value changed

            if (depth != UINT8_MAX) {
                updatedItem.parent[depth] = parent.c_str();
                updatedItem.index[depth] = index;
            }

            if (stateValue.is<JsonArray>() || newValue.is<JsonArray>()) { // if the property is an array
                if (stateValue.isNull()) {stateData[key.c_str()].to<JsonArray>(); stateValue = stateData[key.c_str()];} // if old value is null, set to empty array
                JsonArray oldArray = stateValue.as<JsonArray>();
                JsonArray newArray = newValue.as<JsonArray>();
                
                for (int i = 0; i < max(oldArray.size(), newArray.size()); i++) { //compare each item in the array
                    if (oldArray.size() < newArray.size()) oldArray.add<JsonObject>(); //add new row
                    changed = compareRecursive(key, oldArray[i], newArray[i], updatedItem, depth+1, i) || changed;
                    if (oldArray.size() > newArray.size()) oldArray.remove(i); //remove old row
                }
            } else { // if property is key/value
                changed = true;
                updatedItem.name = key.c_str();
                updatedItem.oldValue = stateValue.as<String>();
                updatedItem.value = newValue;
                stateData[key.c_str()] = newValue; //update state
                
                TaskHandle_t currentTask = xTaskGetCurrentTaskHandle();
                ESP_LOGD(TAG, "changed %s = %s -> %s (%s %d)", updatedItem.name, updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str(), pcTaskGetName(currentTask), uxTaskGetStackHighWaterMark(currentTask));

                if (onUpdate) onUpdate(updatedItem);
            }
        }
    }
    return changed;
}

StateUpdateResult ModuleState::update(JsonObject &root, ModuleState &state)
{
    
    if (root.size() != 0) { // in case of empty file
        TaskHandle_t currentTask = xTaskGetCurrentTaskHandle();
        ESP_LOGI(TAG, "task %s %d", pcTaskGetName(currentTask), uxTaskGetStackHighWaterMark(currentTask));

        // char buffer[1024];
        // serializeJson(state.data, buffer, sizeof(buffer));
        // ESP_LOGD(TAG, "state.doc %s", buffer);
        // serializeJson(root, buffer, sizeof(buffer));
        // ESP_LOGD(TAG, "root %s", buffer);

        //check which propertys have updated
        if (root != state.data) {
            UpdatedItem updatedItem;
            
            return state.compareRecursive("", state.data, root, updatedItem)?StateUpdateResult::CHANGED:StateUpdateResult::UNCHANGED;
        } else
            return StateUpdateResult::UNCHANGED;
    } else
        return StateUpdateResult::UNCHANGED;
}

Module::Module(String moduleName, PsychicHttpServer *server,
    ESP32SvelteKit *sveltekit,
    FilesService *filesService
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

    ESP_LOGD(TAG, "constructor %s", moduleName.c_str());
    _server = server;

    // configure settings service update handler to update state
    _filesService = filesService;

    _state.onUpdate = [&](UpdatedItem &updatedItem) {
        onUpdate(updatedItem); // Ensure updatedItem is of type UpdatedItem&
    };

    addUpdateHandler([&](const String &originId)
                     { onConfigUpdated(); },
                     false);
}

void Module::begin()
{
    ESP_LOGD(TAG, "");
    _httpEndpoint.begin();
    _eventEndpoint.begin();
    _fsPersistence.readFromFS(); //overwrites the default settings in state

    //no virtual functions in constructor so this is in begin()
    _state.setupDefinition = [&](JsonArray root) {
        this->setupDefinition(root); //using here means setupDefinition must be virtual ...
    };
    // _state.setupDefinition = this->setupDefinition;
    _state.setupData(); //if no data readFromFS, using overridden virtual function setupDefinition

    _server->on(String("/rest/" + _moduleName + "Def").c_str(), HTTP_GET, [&](PsychicRequest *request) {
        PsychicJsonResponse response = PsychicJsonResponse(request, false);
        JsonArray root = response.getRoot().to<JsonArray>();

        setupDefinition(root); //virtual function

        // char buffer[2048];
        // serializeJson(root, buffer, sizeof(buffer));
        // ESP_LOGD(TAG, "server->on %s moduleDef %s", request->url().c_str(), buffer);
    
        return response.send();
    });

    onConfigUpdated(); //triggers all onUpdates
}

void Module::onConfigUpdated()
{
    ESP_LOGD(TAG, "onConfigUpdated");
    //if update, for all updated items, run onUpdate
    // for (UpdatedItem updatedItem : _state.updatedItems) {
    //     onUpdate(updatedItem);
    // }
}

void Module::setupDefinition(JsonArray root) { //virtual so it can be overriden in derived classes
    ESP_LOGW(TAG, "not implemented");
    JsonObject property; // state.data has one or more properties
    JsonArray details; // if a property is an array, this is the details of the array
    JsonArray values; // if a property is a select, this is the values of the select

    property = root.add<JsonObject>(); property["name"] = "text"; property["type"] = "text"; property["default"] = "MoonBase";
}

void Module::onUpdate(UpdatedItem &updatedItem) {
    ESP_LOGW(TAG, "not implemented %s = %s", updatedItem.name, updatedItem.value.as<String>().c_str());
}


#endif