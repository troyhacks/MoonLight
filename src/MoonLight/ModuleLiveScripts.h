/**
    @title     MoonLight
    @file      moduleLiveScripts.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/modules/module/liveScripts/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
**/

#ifndef moduleLiveScripts_h
#define moduleLiveScripts_h

#if FT_MOONLIGHT
#if FT_LIVESCRIPT

#undef TAG
#define TAG "💫"

#include "../MoonBase/Module.h"

#include "ModuleAnimations.h"

class ModuleLiveScripts : public Module
{
public:

    PsychicHttpServer *_server;

    ModuleLiveScripts(PsychicHttpServer *server,
        ESP32SvelteKit *sveltekit,
        FilesService *filesService
    ) : Module("liveScripts", server, sveltekit, filesService) {
        ESP_LOGD(TAG, "constructor");
        _server = server;
    }

    void begin() {
        Module::begin();
    }

    //define the data model
    void setupDefinition(JsonArray root) override {
        ESP_LOGD(TAG, "");
        JsonObject property; // state.data has one or more properties
        JsonArray details = root; // if a property is an array, this is the details of the array
        JsonArray values; // if a property is a select, this is the values of the select

        property = root.add<JsonObject>(); property["name"] = "scripts"; property["type"] = "array"; details = property["n"].to<JsonArray>();
        {
            property = details.add<JsonObject>(); property["name"] = "name"; property["type"] = "text"; property["ro"] = true;
            property = details.add<JsonObject>(); property["name"] = "isRunning"; property["type"] = "checkbox"; property["ro"] = true;
            property = details.add<JsonObject>(); property["name"] = "isHalted"; property["type"] = "checkbox"; property["ro"] = true;
            property = details.add<JsonObject>(); property["name"] = "exeExist"; property["type"] = "checkbox"; property["ro"] = true;
            property = details.add<JsonObject>(); property["name"] = "handle"; property["type"] = "number"; property["ro"] = true;
            property = details.add<JsonObject>(); property["name"] = "binary_size"; property["type"] = "number"; property["ro"] = true;
            property = details.add<JsonObject>(); property["name"] = "data_size"; property["type"] = "number"; property["ro"] = true;
            property = details.add<JsonObject>(); property["name"] = "error"; property["type"] = "text"; property["ro"] = true;
            property = details.add<JsonObject>(); property["name"] = "stop"; property["type"] = "button";
            property = details.add<JsonObject>(); property["name"] = "start"; property["type"] = "button";
            // property = details.add<JsonObject>(); property["name"] = "free"; property["type"] = "button";
            property = details.add<JsonObject>(); property["name"] = "delete"; property["type"] = "button";
        }
    }

    //implement business logic
    void onUpdate(UpdatedItem &updatedItem) override
    {

        //scripts
        if (updatedItem.parent[0] == "scripts") {    
            JsonVariant scriptState = _state.data["scripts"][updatedItem.index[0]];
            ESP_LOGD(TAG, "handle %s[%d]%s[%d].%s = %s -> %s", updatedItem.parent[0], updatedItem.index[0], updatedItem.parent[1], updatedItem.index[1], updatedItem.name, updatedItem.oldValue.c_str(), updatedItem.value.c_str());
            if (updatedItem.oldValue != "null") {//do not run at boot!
                LiveScriptNode *liveScriptNode = findLiveScriptNode(scriptState["name"]);
                if (liveScriptNode) {
                    if (updatedItem.name == "stop")
                        liveScriptNode->kill();
                    if (updatedItem.name == "start")
                        liveScriptNode->execute();
                    // if (equal(updatedItem.name, "free"))
                    //     liveScriptNode->free();
                    if (updatedItem.name == "delete")
                        liveScriptNode->killAndDelete();
                    // updatedItem.value = 0;
                } else ESP_LOGW(TAG, "liveScriptNode not found %s", scriptState["name"].as<String>().c_str());
            }
        } 
        // else
        // ESP_LOGD(TAG, "no handle for %s[%d]%s[%d].%s = %s -> %s", updatedItem.parent[0], updatedItem.index[0], updatedItem.parent[1], updatedItem.index[1], updatedItem.name, updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
    }

    //update scripts / read only values in the UI
    void loop1s() {

        if (!_socket->getConnectedClients()) return; 

        JsonDocument newData; //to only send updatedData
        JsonArray scripts = newData["scripts"].to<JsonArray>(); //to: remove old array
        LiveScriptNode node;
        node.getScriptsJson(scripts);

        //only if changed
        if (_state.data["scripts"] != newData["scripts"]) {
            // UpdatedItem updatedItem;
            // _state.compareRecursive("scripts", _state.data["scripts"], newData["scripts"], updatedItem); //compare and update
            _state.data["scripts"] = newData["scripts"]; //update without compareRecursive -> without handles
            JsonObject newDataObject = newData.as<JsonObject>();
            _socket->emitEvent("animations", newDataObject);
        }

        // if (_state.data["scripts"] != newData["scripts"]) {
        //     update([&](ModuleState &state) {

        //         ESP_LOGD(TAG, "update scripts");

        //         UpdatedItem updatedItem;
        //         ; //compare and update
        //         state.data["scripts"] = newData["scripts"]; //update without compareRecursive -> without handles
        //         // return state.compareRecursive("scripts", state.data["scripts"], newData["scripts"], updatedItem)?StateUpdateResult::CHANGED:StateUpdateResult::UNCHANGED;
        //         return StateUpdateResult::CHANGED; // notify StatefulService by returning CHANGED
        //     }, "server");
        // }

        // char buffer[256];
        // serializeJson(doc, buffer, sizeof(buffer));
        // ESP_LOGD(TAG, "livescripts %s", buffer);
    }

    LiveScriptNode *findLiveScriptNode(const char *animation) {
        for (Node *node : layerP.layerV[0]->nodes) {
            // Check if the node is of type LiveScriptNode
            LiveScriptNode *liveScriptNode = (LiveScriptNode *)node;
            if (liveScriptNode) {

                if (equal(liveScriptNode->animation, animation)) {
                    ESP_LOGD(TAG, "found %s", animation);
                    return liveScriptNode;
                }
            }
        }
        return nullptr;
    }
  
}; // class ModuleLiveScripts

#endif
#endif
#endif