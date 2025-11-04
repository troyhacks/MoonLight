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
  for (JsonObject property : definition) {
    // if (property["type"] == "coord3Dxx") {
    //     EXT_LOGD(ML_TAG, "coord3D %d %d %d",  property["default"]["x"].as<int>(),  property["default"]["y"].as<int>(),  property["default"]["z"].as<int>());
    //     JsonObject object = root[property["name"]].to<JsonObject>();
    //     root[property["name"]]["x"] = property["default"]["x"];
    //     root[property["name"]]["y"] = property["default"]["y"];
    //     root[property["name"]]["z"] = property["default"]["z"];
    // } else
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
  // only if no file ...
  if (data.size() == 0) {
    EXT_LOGV(MB_TAG, "size %d", data.size());
    JsonDocument definition;
    if (setupDefinition)
      setupDefinition(definition.to<JsonArray>());
    else
      EXT_LOGW(MB_TAG, "no definition");

    // create a new json for the defaults
    JsonDocument doc;
    setDefaults(doc.to<JsonObject>(), definition.as<JsonArray>());

    // assign the new defaults to state and run onUpdate
    data.clear();  //->to<JsonObject>(); //clear data
    UpdatedItem updatedItem;
    compareRecursive("", data, doc.as<JsonObject>(), updatedItem);  // fill data with doc
  }

  // to do: check if the file matches the definition
}

void ModuleState::read(ModuleState& state, JsonObject& root) {
  if (state.readHook) state.readHook(state.data);

  root.set(state.data);  // copy
}

void ModuleState::execOnReOrderSwap(uint8_t stateIndex, uint8_t newIndex) {
  if (onUpdateRunInTask == 1) {  // if set to 0, run in main loopTask
    std::lock_guard<std::mutex> lock(runInTask_mutex);
    runInTask1.push_back([&, stateIndex, newIndex]() { onReOrderSwap(stateIndex, newIndex); });
  } else if (onUpdateRunInTask == 2) {  // if set to 0, run in main loopTask
    std::lock_guard<std::mutex> lock(runInTask_mutex);
    runInTask2.push_back([&, stateIndex, newIndex]() { onReOrderSwap(stateIndex, newIndex); });
  } else
    onReOrderSwap(stateIndex, newIndex);  // this code will swap the values in its array from stateIndex to newIndex, so the value at stateIndex is storage for the value which was on newIndex
}

bool ModuleState::checkReOrderSwap(JsonString parent, JsonVariant stateData, JsonVariant newData, UpdatedItem& updatedItem, uint8_t depth, uint8_t index) {
  bool changed = false;
  // check if root is a reordering of state
  // if so reorder state, no comparison with updates needed
  for (JsonPair newProperty : newData.as<JsonObject>()) {
    if (newProperty.value().is<JsonArray>()) {
      // bool reorderedRows = false;
      JsonArray newArray = newProperty.value();
      JsonArray stateArray = stateData[newProperty.key()];
      // check each row with each row of stateArray and check if same row found

      // only pure swaps, no add or remove of rows
      if (newArray.size() == stateArray.size()) {
        uint8_t parkedAtIndex;
        uint8_t parkedFromIndex = UINT8_MAX;

        // size_t minSize = min(stateArray.size(), newArray.size());
        for (uint8_t stateIndex = 0; stateIndex < stateArray.size(); stateIndex++) {  //} JsonObject stateObject : stateArray) {
          for (uint8_t newIndex = 0; newIndex < stateArray.size(); newIndex++) {      //} JsonObject newObject : newArray) {
            if (stateIndex != newIndex && stateArray[stateIndex] == newArray[newIndex]) {
              // if the old value found somewhere else, it has been moved so we can overwrite the current value
              stateArray[stateIndex].set(newArray[stateIndex]);  // copies the value to the state array
              EXT_LOGD(ML_TAG, "(%d @ %d) %d -> %d", parkedFromIndex, parkedAtIndex, stateIndex, newIndex);
              changed = true;

              // calculate swap indexes
              // eg 0 to 1 take 0 and store in 1. old 1 is parked in 0
              //.  1 to 2 take the 1 parked in 0 store old 2 in 0
              //.  2 to 0 take the 2 parked in 0 . it is already in 0 so we are done!!!

              // temp = newindex; newindex = oldindex; oldindex = temp (storage)!

              // for each map which maps to another row, call onReOrderSwap

              // check if stateindex is stored somewhere else
              // if stateIndex refers to an index which is parked, use the parking spot.

              uint8_t newStateIndex = stateIndex;
              if (stateIndex == parkedFromIndex)  // e.g. parkedFromIndex ==1
                newStateIndex = parkedAtIndex;    // e.g. 1 is stored in 0

              if (newStateIndex != newIndex) execOnReOrderSwap(newStateIndex, newIndex);

              if (parkedFromIndex == UINT8_MAX) parkedFromIndex = newIndex;  // the index of value in the array stored in the parking spot
              parkedAtIndex = newStateIndex;                                 // the parking spot created
            }
          }
        }
      }  // equal size
    }
  }
  return changed;
}

void ModuleState::execOnUpdate(UpdatedItem& updatedItem) {
  if (updatedItem.oldValue != "null" && updatedItem.name != "channel") {  // todo: fix the problem at channel, not here...
    // EXT_LOGD(MB_TAG, "%s = %s -> %s", updatedItem.name.c_str(), updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
    saveNeeded = true;
  }

  if (onUpdate) {
    if (onUpdateRunInTask == 1) {  // if set to 0, run in main loopTask
      std::lock_guard<std::mutex> lock(runInTask_mutex);
      runInTask1.push_back([&, updatedItem]() mutable {  // mutable as updatedItem is called by reference (&)
        onUpdate(updatedItem);
      });
    } else if (onUpdateRunInTask == 2) {  // if set to 0, run in main loopTask
      std::lock_guard<std::mutex> lock(runInTask_mutex);
      runInTask2.push_back([&, updatedItem]() mutable {  // mutable as updatedItem is called by reference (&)
        onUpdate(updatedItem);
      });
    } else
      onUpdate(updatedItem);
  }
}

bool ModuleState::compareRecursive(JsonString parent, JsonVariant stateData, JsonVariant newData, UpdatedItem& updatedItem, uint8_t depth, uint8_t index) {
  bool changed = false;
  for (JsonPair newProperty : newData.as<JsonObject>()) {
    if (stateData[newProperty.key()].isNull()) {
      stateData[newProperty.key()] = nullptr;  // Initialize the key in stateData if it doesn't exist todo: run in loopTask ?
    }
  }

  // loop over all properties in stateData
  for (JsonPair stateProperty : stateData.as<JsonObject>()) {
    JsonString key = stateProperty.key();
    JsonVariant stateValue = stateData[key.c_str()];
    JsonVariant newValue = newData[key.c_str()];
    if (!newValue.isNull() && stateValue != newValue) {  // if value changed, don't update if not defined in newValue

      if (depth != UINT8_MAX) {  // depth starts with '-1' (no depth)
        updatedItem.parent[depth] = parent.c_str();
        updatedItem.index[depth] = index;
      }
      for (uint8_t i = depth + 1; i < 2; i++) {  // reset deeper levels when coming back from recursion
        updatedItem.parent[i] = "";
        updatedItem.index[i] = UINT8_MAX;
      }

      if (stateValue.is<JsonArray>() || newValue.is<JsonArray>()) {  // if the property is an array
        if (stateValue.isNull()) {
          stateData[key.c_str()].to<JsonArray>();
          stateValue = stateData[key.c_str()];
        }  // if old value is null, set to empty array
        JsonArray stateArray = stateValue.as<JsonArray>();
        JsonArray newArray = newValue.as<JsonArray>();

        // EXT_LOGD(MB_TAG, "compare %s[%d] %s = %s -> %s", parent.c_str(), index, key.c_str(), stateValue.as<String>().c_str(), newValue.as<String>().c_str());

        for (int i = 0; i < max(stateArray.size(), newArray.size()); i++) {  // compare each item in the array
          // EXT_LOGD(MB_TAG, "compare %s[%d] %s = %s -> %s", parent.c_str(), index, key.c_str(), stateArray[i].as<String>().c_str(), newArray[i].as<String>().c_str());
          if (i >= stateArray.size()) {  // newArray has added a row
            // EXT_LOGD(MB_TAG, "add %s.%s[%d] d: %d", parent.c_str(), key.c_str(), i, depth);
            stateArray.add<JsonObject>();  // add new row
            changed = compareRecursive(key, stateArray[i], newArray[i], updatedItem, depth + 1, i) || changed;
          } else if (i >= newArray.size()) {  // newArray has deleted a row
            // EXT_LOGD(MB_TAG, "remove %s.%s[%d] d: %d", parent.c_str(), key.c_str(), i, depth);
            // newArray.add<JsonObject>(); //add dummy row
            changed = true;  // compareRecursive(key, stateArray[i], newArray[i], updatedItem, depth+1, i) || changed;

            // set all the values to null
            //  UpdatedItem updatedItem; //create local updatedItem
            updatedItem.parent[1] = "";  // reset deeper levels when coming back from recursion (repeat in loop)
            updatedItem.index[1] = UINT8_MAX;
            updatedItem.parent[(uint8_t)(depth + 1)] = key.c_str();
            updatedItem.index[(uint8_t)(depth + 1)] = i;
            for (JsonPair property : stateArray[i].as<JsonObject>()) {
              // EXT_LOGD(MB_TAG, "     remove %s[%d] %s %s", key.c_str(), i, property.key().c_str(), property.value().as<String>().c_str());
              // newArray[i][property.key()] = nullptr; // Initialize the keys in newArray so comparerecusive can compare them
              updatedItem.name = property.key().c_str();
              updatedItem.oldValue = property.value().as<String>();
              updatedItem.value = JsonVariant();     // Assign an empty JsonVariant
              stateArray[i].remove(property.key());  // remove the property from the state row so onUpdate see it as empty
              execOnUpdate(updatedItem);
            }

            stateArray.remove(i);  // remove the state row entirely
          } else                   // row already exists
            changed = compareRecursive(key, stateArray[i], newArray[i], updatedItem, depth + 1, i) || changed;
        }
      } else {  // if property is key/value
        if (key != "p") {
          changed = true;
          updatedItem.name = key.c_str();
          updatedItem.oldValue = stateValue.as<String>();
          stateData[updatedItem.name] = newValue;           // update the value in stateData, should not be done in runLoopTask as FS update then misses the change!!
          updatedItem.value = stateData[updatedItem.name];  // store the stateData item (convenience)

          execOnUpdate(updatedItem);
        }
        // else {
        //     EXT_LOGD(MB_TAG, "do not update %s", key.c_str());
        // }
      }
    }
  }
  return changed;
}

StateUpdateResult ModuleState::update(JsonObject& root, ModuleState& state) {
  // if (state.data.isNull()) EXT_LOGD(ML_TAG, "state data is null %d %d", root.size(), root != state.data); // state.data never null here

  if (root.size() != 0) {  // in case of empty file

    // check which propertys have updated
    if (root != state.data) {
      UpdatedItem updatedItem;

      // bool isNew = state.data.isNull();  // device is starting , not useful as state.data never null here

      bool changed = state.checkReOrderSwap("", state.data, root, updatedItem);

      // EXT_LOGD(ML_TAG, "update isNew %d changed %d", isNew, changed);
      // serializeJson(state.data, Serial);Serial.println();
      // serializeJson(root, Serial);Serial.println();

      if (state.compareRecursive("", state.data, root, updatedItem)) {
        if (changed) EXT_LOGW(ML_TAG, "checkReOrderSwap changed, compareRecursive also changed?");
        changed = true;
      }

      return (changed) ? StateUpdateResult::CHANGED : StateUpdateResult::UNCHANGED;  //! isNew &&
    } else
      return StateUpdateResult::UNCHANGED;
  } else {
    EXT_LOGD(MB_TAG, "empty root");
    return StateUpdateResult::UNCHANGED;
  }
}

Module::Module(String moduleName, PsychicHttpServer* server, ESP32SvelteKit* sveltekit)
    : _httpEndpoint(ModuleState::read, ModuleState::update, this, server, String("/rest/" + moduleName).c_str(), sveltekit->getSecurityManager(), AuthenticationPredicates::IS_AUTHENTICATED),
      _eventEndpoint(ModuleState::read, ModuleState::update, this, sveltekit->getSocket(), moduleName.c_str()),
      _webSocketServer(ModuleState::read, ModuleState::update, this, server, String("/ws/" + moduleName).c_str(), sveltekit->getSecurityManager(), AuthenticationPredicates::IS_AUTHENTICATED),
      _socket(sveltekit->getSocket()),
      _fsPersistence(ModuleState::read, ModuleState::update, this, sveltekit->getFS(), String("/.config/" + moduleName + ".json").c_str(), true)  // 🌙 true: delayedWrites
{
  _moduleName = moduleName;

  EXT_LOGV(MB_TAG, "constructor %s", moduleName.c_str());
  _server = server;

  _state.onUpdate = [&](UpdatedItem& updatedItem) {
    onUpdate(updatedItem);  // Ensure updatedItem is of type UpdatedItem&
  };

  _state.onReOrderSwap = [&](uint8_t stateIndex, uint8_t newIndex) {
    onReOrderSwap(stateIndex, newIndex);  // Ensure updatedItem is of type UpdatedItem&
  };

  addUpdateHandler([&](const String& originId) { updateHandler(); }, false);
}

void Module::begin() {
  EXT_LOGV(MB_TAG, "");
  _httpEndpoint.begin();
  _eventEndpoint.begin();
  _fsPersistence.readFromFS();  // overwrites the default settings in state

  // no virtual functions in constructor so this is in begin()
  _state.setupDefinition = [&](JsonArray root) {
    this->setupDefinition(root);  // using here means setupDefinition must be virtual ...
  };
  // _state.setupDefinition = this->setupDefinition;
  _state.setupData();  // if no data readFromFS, using overridden virtual function setupDefinition

  _server->on(String("/rest/" + _moduleName + "Def").c_str(), HTTP_GET, [&](PsychicRequest* request) {
    PsychicJsonResponse response = PsychicJsonResponse(request, false);
    JsonArray root = response.getRoot().to<JsonArray>();

    setupDefinition(root);  // virtual function

    // char buffer[2048];
    // serializeJson(root, buffer, sizeof(buffer));
    // EXT_LOGD(MB_TAG, "server->on %s moduleDef %s", request->url().c_str(), buffer);

    return response.send();
  });

  updateHandler();  // triggers all onUpdates, needed? as setupData already called compareRecursive
}

void Module::updateHandler() {
  // EXT_LOGD(MB_TAG, "");
  // if update, for all updated items, run onUpdate
  //  for (UpdatedItem updatedItem : _state.updatedItems) {
  //      onUpdate(updatedItem);
  //  }
}

void Module::setupDefinition(JsonArray root) {  // virtual so it can be overriden in derived classes
  EXT_LOGW(MB_TAG, "not implemented");
  JsonObject property;  // state.data has one or more properties
  JsonArray details;    // if a property is an array, this is the details of the array
  JsonArray values;     // if a property is a select, this is the values of the select

  property = root.add<JsonObject>();
  property["name"] = "text";
  property["type"] = "text";
  property["default"] = "MoonLight";
}

void Module::onUpdate(UpdatedItem& updatedItem) { EXT_LOGD(MB_TAG, "%s = %s -> %s", updatedItem.name.c_str(), updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str()); }

void Module::onReOrderSwap(uint8_t stateIndex, uint8_t newIndex) { EXT_LOGD(MB_TAG, "s:%d n:%d", stateIndex, newIndex); }

#endif