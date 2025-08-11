/**
    @title     MoonBase
    @file      Module.cpp
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/develop/modules/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact us for more information.
**/

#if FT_MOONBASE == 1

#include "Module.h"

void setDefaults(JsonObject root, JsonArray definition) {
    for (JsonObject property: definition) {
        if (property["type"] != "array") {
            root[property["name"]] = property["default"];
        } else {
            // JsonArray array = root[property["name"]].to<JsonArray>();
            // //loop over detail propertys (recursive)
            // JsonObject object = array.add<JsonObject>(); // add one row
            // setDefaults(object, property["n"].as<JsonArray>());
        }
    }
}

void ModuleState::setupData() {

    //only if no file ...
    if (data.size() == 0) {
        ESP_LOGV(TAG, "size %d", data.size());
        JsonDocument definition;
        if (setupDefinition) 
            setupDefinition(definition.to<JsonArray>());
        else
            ESP_LOGW(TAG, "no definition");

        //create a new json for the defaults
        JsonDocument doc;
        setDefaults(doc.to<JsonObject>(), definition.as<JsonArray>());
            
        //assign the new defaults to state and run onUpdate
        data.to<JsonObject>(); //clear data
        UpdatedItem updatedItem;
        compareRecursive("", data, doc.as<JsonObject>(), updatedItem); //fill data with doc
}

    //to do: check if the file matches the definition
}

void ModuleState::read(ModuleState &state, JsonObject &root)
{
    if (state.readHook)
        state.readHook(state.data.as<JsonObject>());

    root.set(state.data.as<JsonObject>()); //copy
}

bool ModuleState::compareRecursive(JsonString parent, JsonVariant stateData, JsonVariant newData, UpdatedItem &updatedItem, uint8_t depth, uint8_t index) {
    bool changed = false;
    for (JsonPair newProperty : newData.as<JsonObject>()) {
        if (stateData[newProperty.key()].isNull()) { 
            stateData[newProperty.key()] = nullptr; // Initialize the key in stateData if it doesn't exist todo: run in loopTask ?
        }
    }

    for (JsonPair stateProperty : stateData.as<JsonObject>()) {
        JsonString key = stateProperty.key();
        JsonVariant stateValue = stateData[key.c_str()];
        JsonVariant newValue = newData[key.c_str()];
        if (!newValue.isNull() && stateValue != newValue) { //if value changed, don't update if not defined in newValue

            if (depth != UINT8_MAX) { //depth starts with '-1' (no depth)
                updatedItem.parent[depth] = parent.c_str();
                updatedItem.index[depth] = index;
            }
            for (int i = uint8_t(depth + 1); i < 2; i++) { // reset deeper levels when coming back from recursion
                updatedItem.parent[i] = "";
                updatedItem.index[i] = UINT8_MAX;
            }

            if (stateValue.is<JsonArray>() || newValue.is<JsonArray>()) { // if the property is an array
                if (stateValue.isNull()) {stateData[key.c_str()].to<JsonArray>(); stateValue = stateData[key.c_str()];} // if old value is null, set to empty array
                JsonArray stateArray = stateValue.as<JsonArray>();
                JsonArray newArray = newValue.as<JsonArray>();

                // ESP_LOGV(TAG, "compare %s[%d] %s = %s -> %s", parent.c_str(), index, key.c_str(), stateValue.as<String>().c_str(), newValue.as<String>().c_str());
                
                for (int i = 0; i < max(stateArray.size(), newArray.size()); i++) { //compare each item in the array
                    if (i >= stateArray.size()) { //newArray has added a row
                        ESP_LOGV(TAG, "add %s.%s[%d] d: %d", parent.c_str(), key.c_str(), i, depth);
                        stateArray.add<JsonObject>(); //add new row
                        changed = compareRecursive(key, stateArray[i], newArray[i], updatedItem, depth+1, i) || changed;
                    } else if (i >= newArray.size()) { //newArray has deleted a row
                        ESP_LOGV(TAG, "remove %s.%s[%d] d: %d", parent.c_str(), key.c_str(), i, depth);
                        // newArray.add<JsonObject>(); //add dummy row
                        changed = true; //compareRecursive(key, stateArray[i], newArray[i], updatedItem, depth+1, i) || changed;
                        //set all the values to null
                        // UpdatedItem updatedItem; //create local updatedItem
                        updatedItem.parent[(uint8_t)(depth+1)] = key.c_str();
                        updatedItem.index[(uint8_t)(depth+1)] = i;
                        for (JsonPair property : stateArray[i].as<JsonObject>()) {
                            // ESP_LOGV(TAG, "     remove %s[%d] %s %s", key.c_str(), i, property.key().c_str(), property.value().as<String>().c_str());
                            // newArray[i][property.key()] = nullptr; // Initialize the keys in newArray so comparerecusive can compare them
                            updatedItem.name = property.key().c_str();
                            updatedItem.oldValue = property.value().as<String>();
                            updatedItem.value = JsonVariant(); // Assign an empty JsonVariant
                            stateArray[i].remove(property.key()); //remove the property from the state row so onUpdate see it as empty
                            if (onUpdate) onUpdate(updatedItem);

                        }
                        stateArray.remove(i); //remove the state row entirely
                    } else //row already exists
                        changed = compareRecursive(key, stateArray[i], newArray[i], updatedItem, depth+1, i) || changed;
                }
            } else { // if property is key/value
                if (key != "p") {
                    changed = true;
                    updatedItem.name = key.c_str();
                    updatedItem.oldValue = stateValue.as<String>();
                    stateData[updatedItem.name] = newValue; //update the value in stateData, should not be done in runLoopTask as FS update then misses the change!!
                    updatedItem.value = stateData[updatedItem.name]; //store the stateData item (convenience)
                    
                    if (onUpdateRunInTask == 1) { //if set to 0, run in main loopTask
                        runInTask1.push_back([&, updatedItem, stateData]() mutable { //mutable as updatedItem is called by reference (&)
                            ESP_LOGV(TAG, "update %s[%d].%s[%d].%s = %s", updatedItem.parent[0].c_str(), updatedItem.index[0], updatedItem.parent[1].c_str(), updatedItem.index[1], updatedItem.name.c_str(), updatedItem.value.as<String>().c_str());
                            if (onUpdate) onUpdate(updatedItem);
                            // TaskHandle_t currentTask = xTaskGetCurrentTaskHandle();
                            // ESP_LOGV(TAG, "changed %s = %s -> %s (%s %d)", updatedItem.name, updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str(), pcTaskGetName(currentTask), uxTaskGetStackHighWaterMark(currentTask));
                        });
                    }
                    else if (onUpdateRunInTask == 2) { //if set to 0, run in main loopTask
                        runInTask2.push_back([&, updatedItem, stateData]() mutable { //mutable as updatedItem is called by reference (&)
                            ESP_LOGV(TAG, "update %s[%d].%s[%d].%s = %s", updatedItem.parent[0].c_str(), updatedItem.index[0], updatedItem.parent[1].c_str(), updatedItem.index[1], updatedItem.name.c_str(), updatedItem.value.as<String>().c_str());
                            if (onUpdate) onUpdate(updatedItem);
                            // TaskHandle_t currentTask = xTaskGetCurrentTaskHandle();
                            // ESP_LOGV(TAG, "changed %s = %s -> %s (%s %d)", updatedItem.name, updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str(), pcTaskGetName(currentTask), uxTaskGetStackHighWaterMark(currentTask));
                        });
                    }
                    else
                        if (onUpdate) onUpdate(updatedItem);
                } 
                // else {
                //     ESP_LOGV(TAG, "do not update %s", key.c_str());
                // }
            }
        }
    }
    return changed;
}

StateUpdateResult ModuleState::update(JsonObject &root, ModuleState &state)
{
    
    if (root.size() != 0) { // in case of empty file

        //check which propertys have updated
        if (root != state.data) {
            UpdatedItem updatedItem;

            bool isNew = state.data.isNull(); //device is starting
            
            bool changed = state.compareRecursive("", state.data, root, updatedItem);

            if (!isNew && changed) saveNeeded = true;

            return (!isNew && changed)?StateUpdateResult::CHANGED:StateUpdateResult::UNCHANGED;
        } else
            return StateUpdateResult::UNCHANGED;
    } else
        return StateUpdateResult::UNCHANGED;
}

Module::Module(String moduleName, PsychicHttpServer *server,
    ESP32SvelteKit *sveltekit
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
                  String("/.config/" + moduleName + ".json").c_str(), true) //🌙 true: delayedWrites
{
    _moduleName = moduleName;

    ESP_LOGV(TAG, "constructor %s", moduleName.c_str());
    _server = server;

    _state.onUpdate = [&](UpdatedItem &updatedItem) {
        onUpdate(updatedItem); // Ensure updatedItem is of type UpdatedItem&
    };

    addUpdateHandler([&](const String &originId)
                     { onConfigUpdated(); },
                     false);
}

void Module::begin()
{
    ESP_LOGV(TAG, "");
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
        // ESP_LOGV(TAG, "server->on %s moduleDef %s", request->url().c_str(), buffer);
    
        return response.send();
    });

    onConfigUpdated(); //triggers all onUpdates
}

void Module::onConfigUpdated()
{
    ESP_LOGV(TAG, "onConfigUpdated");
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

    property = root.add<JsonObject>(); property["name"] = "text"; property["type"] = "text"; property["default"] = "MoonLight";
}

void Module::onUpdate(UpdatedItem &updatedItem) {
    // ESP_LOGW(TAG, "not implemented %s = %s", updatedItem.name.c_str(), updatedItem.value.as<String>().c_str());
}


#endif