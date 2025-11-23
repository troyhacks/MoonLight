/**
    @title     MoonLight
    @file      moduleLiveScripts.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/moonlight/livescripts/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact us for more information.
**/

#ifndef moduleLiveScripts_h
#define moduleLiveScripts_h

#if FT_MOONLIGHT
  #if FT_LIVESCRIPT

    #include "ModuleEffects.h"
    #include "MoonBase/Module.h"

class ModuleLiveScripts : public Module {
 public:
  PsychicHttpServer* _server;
  FileManager* _fileManager;
  ModuleEffects* _moduleEffects;
  ModuleDrivers* _moduleDrivers;

  ModuleLiveScripts(PsychicHttpServer* server, ESP32SvelteKit* sveltekit, FileManager* fileManager, ModuleEffects* moduleEffects, ModuleDrivers* moduleDrivers) : Module("livescripts", server, sveltekit) {
    EXT_LOGV(ML_TAG, "constructor");
    _server = server;
    _fileManager = fileManager;
    _moduleEffects = moduleEffects;
    _moduleDrivers = moduleDrivers;
  }

  void begin() {
    Module::begin();
    #if FT_ENABLED(FT_LIVESCRIPT)
    // create a handler which recompiles the live script when the file of a current running live script changes in the File Manager
    _fileManager->addUpdateHandler([&](const String& originId) {
      EXT_LOGV(ML_TAG, "FileManager::updateHandler %s", originId.c_str());
      // read the file state (read all files and folders on FS and collect changes)
      _fileManager->read([&](FilesState& filesState) {
        // loop over all changed files (normally only one)
        for (auto updatedItem : filesState.updatedItems) {
          // if file is the current live script, recompile it (to do: multiple live effects)
          uint8_t index = 0;
          _moduleEffects->read([&](ModuleState& effectsState) {
            for (JsonObject nodeState : effectsState.data["nodes"].as<JsonArray>()) {

              if (updatedItem == nodeState["name"]) {
                EXT_LOGD(ML_TAG, "updateHandler equals current item -> livescript compile %s", updatedItem.c_str());
                LiveScriptNode* liveScriptNode = (LiveScriptNode*)_moduleEffects->findLiveScriptNode(nodeState["name"]);
                if (liveScriptNode) {
                  liveScriptNode->compileAndRun();

                  // wait until setup has been executed?

                  _moduleEffects->requestUIUpdate = true;  // update the Effects UI
                }

                EXT_LOGD(ML_TAG, "update due to new node %s done", nodeState["name"].as<const char *>());
              }
              index++;
            }
          });
          _moduleDrivers->read([&](ModuleState& driversState) {
            for (JsonObject nodeState : driversState.data["nodes"].as<JsonArray>()) {

              if (updatedItem == nodeState["name"]) {
                EXT_LOGD(ML_TAG, "updateHandler equals current item -> livescript compile %s", updatedItem.c_str());
                LiveScriptNode* liveScriptNode = (LiveScriptNode*)_moduleDrivers->findLiveScriptNode(nodeState["name"]);
                if (liveScriptNode) {
                  liveScriptNode->compileAndRun();

                  // wait until setup has been executed?

                  _moduleDrivers->requestUIUpdate = true;  // update the Effects UI
                }

                EXT_LOGD(ML_TAG, "update due to new node %s done", nodeState["name"].as<const char *>());
              }
              index++;
            }
          });
        }
      });
    });
    #endif
  }

  // define the data model
  void setupDefinition(JsonArray root) override {
    EXT_LOGV(ML_TAG, "");
    JsonObject property;       // state.data has one or more properties
    JsonArray details = root;  // if a property is an array, this is the details of the array
    JsonArray values;          // if a property is a select, this is the values of the select

    property = root.add<JsonObject>();
    property["name"] = "scripts";
    property["type"] = "rows";
    details = property["n"].to<JsonArray>();
    {
      property = details.add<JsonObject>();
      property["name"] = "name";
      property["type"] = "text";
      property["ro"] = true;
      property = details.add<JsonObject>();
      property["name"] = "isRunning";
      property["type"] = "checkbox";
      property["ro"] = true;
      property = details.add<JsonObject>();
      property["name"] = "isHalted";
      property["type"] = "checkbox";
      property["ro"] = true;
      property = details.add<JsonObject>();
      property["name"] = "exeExist";
      property["type"] = "checkbox";
      property["ro"] = true;
      property = details.add<JsonObject>();
      property["name"] = "handle";
      property["type"] = "number";
      property["ro"] = true;
      property["max"] = 65536;
      property = details.add<JsonObject>();
      property["name"] = "binary_size";
      property["type"] = "number";
      property["ro"] = true;
      property["max"] = 65536;
      property = details.add<JsonObject>();
      property["name"] = "data_size";
      property["type"] = "number";
      property["ro"] = true;
      property["max"] = 65536;
      property = details.add<JsonObject>();
      property["name"] = "error";
      property["type"] = "text";
      property["ro"] = true;
      property = details.add<JsonObject>();
      property["name"] = "stop";
      property["type"] = "button";
      property = details.add<JsonObject>();
      property["name"] = "start";
      property["type"] = "button";
      // property = details.add<JsonObject>(); property["name"] = "free"; property["type"] = "button";
      property = details.add<JsonObject>();
      property["name"] = "delete";
      property["type"] = "button";
    }
  }

  // implement business logic
  void onUpdate(const UpdatedItem& updatedItem) override {
    // scripts
    if (updatedItem.parent[0] == "scripts") {
      JsonVariant scriptState = _state.data["scripts"][updatedItem.index[0]];
      EXT_LOGV(ML_TAG, "handle %s[%d]%s[%d].%s = %s -> %s", updatedItem.parent[0].c_str(), updatedItem.index[0], updatedItem.parent[1].c_str(), updatedItem.index[1], updatedItem.name.c_str(), updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
      if (updatedItem.oldValue != "") {  // do not run at boot!
        LiveScriptNode* liveScriptNode = (LiveScriptNode*)_moduleEffects->findLiveScriptNode(scriptState["name"]);
        if (!liveScriptNode) {
          // try drivers
          liveScriptNode = (LiveScriptNode*)_moduleDrivers->findLiveScriptNode(scriptState["name"]);
        }
        if (liveScriptNode) {
          if (updatedItem.name == "stop") liveScriptNode->kill();
          if (updatedItem.name == "start") liveScriptNode->execute();
          // if (equal(updatedItem.name, "free"))
          //     liveScriptNode->free();
          if (updatedItem.name == "delete") liveScriptNode->killAndDelete();
          // updatedItem.value = 0;
        } else
          EXT_LOGW(ML_TAG, "liveScriptNode not found %s", scriptState["name"].as<const char*>());
      }
    }
    // else
    // EXT_LOGV(ML_TAG, "no handle for %s[%d]%s[%d].%s = %s -> %s", updatedItem.parent[0].c_str(), updatedItem.index[0], updatedItem.parent[1].c_str(), updatedItem.index[1], updatedItem.name.c_str(),
    // updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
  }

  // update scripts / read only values in the UI
  void loop1s() {
    if (!_socket->getConnectedClients()) return;
    if (!WiFi.localIP()) return;

    JsonDocument newData;                                    // to only send updatedData
    JsonArray scripts = newData["scripts"].to<JsonArray>();  // to: remove old array
    LiveScriptNode::getScriptsJson(scripts);

    // only if changed
    if (_state.data["scripts"] != newData["scripts"]) {
      // UpdatedItem updatedItem;
      // _state.compareRecursive("scripts", _state.data["scripts"], newData["scripts"], updatedItem); //compare and update
      _state.data["scripts"] = newData["scripts"];  // update without compareRecursive -> without handles
      // JsonObject newDataObject = newData.as<JsonObject>();
      // _socket->emitEvent("editor", newDataObject);

      // requestUIUpdate ...
      update(
          [&](ModuleState& state) {
            return StateUpdateResult::CHANGED;  // notify StatefulService by returning CHANGED
          },
          _moduleName);
    }

    // if (_state.data["scripts"] != newData["scripts"]) {
    //     update([&](ModuleState &state) {

    //         EXT_LOGV(ML_TAG, "update scripts");

    //         UpdatedItem updatedItem;
    //         ; //compare and update
    //         state.data["scripts"] = newData["scripts"]; //update without compareRecursive -> without handles
    //         // return state.compareRecursive("scripts", state.data["scripts"], newData["scripts"], updatedItem)?StateUpdateResult::CHANGED:StateUpdateResult::UNCHANGED;
    //         return StateUpdateResult::CHANGED; // notify StatefulService by returning CHANGED
    //     }, _moduleName);
    // }

    // char buffer[256];
    // serializeJson(doc, buffer, sizeof(buffer));
    // EXT_LOGV(ML_TAG, "livescripts %s", buffer);
  }

};  // class ModuleLiveScripts

  #endif
#endif
#endif