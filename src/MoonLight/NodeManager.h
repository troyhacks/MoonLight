/**
    @title     MoonLight
    @file      NodeManager.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/moonbase/module/editor/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact us for more information.
**/

#ifndef NodeManager_h
#define NodeManager_h

#if FT_MOONLIGHT

#undef TAG
#define TAG "💫"

#include "../MoonBase/Module.h"

#include "Nodes.h" //Nodes.h will include VirtualLayer.h which will include PhysicalLayer.h

PhysicalLayer layerP; //global declaration of the physical layer

class NodeManager : public Module {
public: 
    bool requestUIUpdate = false;

protected:
    PsychicHttpServer *_server;

    std::vector<Node *> *nodes;

    NodeManager(String moduleName,
        PsychicHttpServer *server,
        ESP32SvelteKit *sveltekit
    ) : Module(moduleName, server, sveltekit) {
        ESP_LOGD(TAG, "constructor");
        _server = server;
    }

    void begin() {
        Module::begin();
    }

    virtual void addNodes(JsonArray values) {}

    virtual Node* addNode(const uint8_t index, const char * name, const JsonArray controls) {return nullptr;}

    //define the data model
    void setupDefinition(JsonArray root) override {
        ESP_LOGD(TAG, "");
        JsonObject property; // state.data has one or more properties
        JsonArray details = root; // if a property is an array, this is the details of the array
        JsonArray values; // if a property is a select, this is the values of the select

        property = root.add<JsonObject>(); property["name"] = "nodes"; property["type"] = "array"; details = property["n"].to<JsonArray>();
        {
            property = details.add<JsonObject>(); property["name"] = "nodeName"; property["type"] = "selectFile"; values = property["values"].to<JsonArray>(); //property["default"] = RandomEffect::name(); 

            addNodes(values);

            property = details.add<JsonObject>(); property["name"] = "on"; property["type"] = "checkbox"; property["default"] = true;
            property = details.add<JsonObject>(); property["name"] = "controls"; property["type"] = "controls"; details = property["n"].to<JsonArray>();
            {
                property = details.add<JsonObject>(); property["name"] = "name"; property["type"] = "text"; property["default"] = "speed";
                property = details.add<JsonObject>(); property["name"] = "type"; property["type"] = "select"; property["default"] = "Number"; values = property["values"].to<JsonArray>();
                values.add("number");
                values.add("range");
                values.add("text");
                values.add("coordinate");
                property = details.add<JsonObject>(); property["name"] = "value"; property["type"] = "text"; property["default"] = "128";
            }
        }
    }

    //implement business logic
    void onUpdate(UpdatedItem &updatedItem) override
    {
        TaskHandle_t currentTask = xTaskGetCurrentTaskHandle();
        // handle nodes
        if (updatedItem.parent[0] == "nodes") { // onNodes
            JsonVariant nodeState = _state.data["nodes"][updatedItem.index[0]];
            // serializeJson(nodeState, Serial); Serial.println();

            if (updatedItem.name == "nodeName") { //onName

                Node *oldNode = updatedItem.index[0] < nodes->size() ? (*nodes)[updatedItem.index[0]] : nullptr; //find the node in the nodes list
                bool newNode = false;

                // remove or add Nodes (incl controls)
                if (!updatedItem.value.isNull()) { // if name changed // == updatedItem.value

                    //if old node exists then remove it's controls
                    if (updatedItem.oldValue != "null") {
                        ESP_LOGD(TAG, "remove controls %s[%d]%s[%d].%s = %s -> %s", updatedItem.parent[0].c_str(), updatedItem.index[0], updatedItem.parent[1].c_str(), updatedItem.index[1], updatedItem.name.c_str(), updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
                        nodeState.remove("controls"); //remove the controls from the nodeState
                    }

                    ESP_LOGD(TAG, "add %s[%d]%s[%d].%s = %s -> %s task %s %d", updatedItem.parent[0].c_str(), updatedItem.index[0], updatedItem.parent[1].c_str(), updatedItem.index[1], updatedItem.name.c_str(), updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str(), pcTaskGetName(currentTask), uxTaskGetStackHighWaterMark(currentTask));
    
                    if (nodeState["controls"].isNull()) { //if controls are not set, create empty array
                        nodeState["controls"].to<JsonArray>(); //clear the controls
                    }

                    Node *nodeClass = addNode(updatedItem.index[0], updatedItem.value, nodeState["controls"]);

                    if (nodeClass != nullptr) {
                        nodeClass->on = nodeState["on"];
                        newNode = true;

                        //wait until setup has been executed?

                        requestUIUpdate = true;

                        ESP_LOGD(TAG, "update due to new node %s done", updatedItem.value.as<String>().c_str());

                        //make sure "p" is also updated

                        nodeClass->requestMappings();
                    }
                    else
                        ESP_LOGW(TAG, "Nodeclass %s not found", updatedItem.value.as<String>().c_str());

                    if (updatedItem.oldValue.indexOf("Driver") != -1 && updatedItem.value.as<String>().indexOf("Driver") != -1) {
                        ESP_LOGW(TAG, "Restart needed");
                        restartNeeded = true;
                    }
                }

                //if a node existed and no new node in place, remove 
                if (updatedItem.oldValue != "null" && oldNode) {
                    ESP_LOGD(TAG, "remove %s[%d]%s[%d].%s = %s -> %s", updatedItem.parent[0].c_str(), updatedItem.index[0], updatedItem.parent[1].c_str(), updatedItem.index[1], updatedItem.name.c_str(), updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
                    if (!newNode) {
                        //remove oldNode from the nodes list 
                        for (uint8_t i = 0; i < nodes->size(); i++) {
                            if ((*nodes)[i] == oldNode) {
                                ESP_LOGD(TAG, "remove node %d", i);
                                nodes->erase(nodes->begin() + i);
                                break;
                            }
                        }
                        ESP_LOGD(TAG, "No newnode - remove! %d s:%d", updatedItem.index[0], nodes->size());
                    }

                    oldNode->requestMappings();

                    layerP.removeNode(oldNode);
                }
                    
                #if FT_ENABLED(FT_LIVESCRIPT)
                    // if (updatedItem.oldValue.length()) {
                    //     ESP_LOGD(TAG, "delete %s %s ...", updatedItem.name.c_str(), updatedItem.oldValue.c_str());
                    //     LiveScriptNode *liveScriptNode = findLiveScriptNode(node["nodeName"]);
                    //     if (liveScriptNode) liveScriptNode->kill(); 
                    //     else ESP_LOGW(TAG, "liveScriptNode not found %s", node["nodeName"].as<String>().c_str());
                    // }
                    // if (!node["nodeName"].isNull() && !node["type"].isNull()) {
                    //     LiveScriptNode *liveScriptNode = findLiveScriptNode(node["nodeName"]); //todo: can be 2 nodes with the same name ...
                    //     if (liveScriptNode) liveScriptNode->compileAndRun();
                    //     // not needed as creating the node is already running it ...
                    // }
                #endif
            }

            if (updatedItem.name == "on") {
                if (updatedItem.index[0] < nodes->size()) {
                    ESP_LOGD(TAG, "on %s %s (%d)", updatedItem.name.c_str(), updatedItem.value.as<String>().c_str(), nodes->size());
                    Node *nodeClass = (*nodes)[updatedItem.index[0]];
                    if (nodeClass != nullptr) {
                        nodeClass->on = updatedItem.value.as<bool>(); //set nodeclass on/off
                        ESP_LOGD(TAG, "  nodeclass 🔘:%d 🚥:%d 💎:%d", nodeClass->on, nodeClass->hasLayout, nodeClass->hasModifier);

                        nodeClass->requestMappings();
                    }
                    else
                        ESP_LOGW(TAG, "Nodeclass %s not found", nodeState["nodeName"].as<String>().c_str());
                }
            }

            if (updatedItem.parent[1] == "controls" && updatedItem.name == "value") {    //process controls values 
                ESP_LOGD(TAG, "handle %s[%d]%s[%d].%s = %s -> %s", updatedItem.parent[0].c_str(), updatedItem.index[0], updatedItem.parent[1].c_str(), updatedItem.index[1], updatedItem.name.c_str(), updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
                if (updatedItem.index[0] < nodes->size()) {
                    Node *nodeClass = (*nodes)[updatedItem.index[0]];
                    if (nodeClass != nullptr) {
                        nodeClass->updateControl(nodeState["controls"][updatedItem.index[1]]);

                        nodeClass->requestMappings();
                    }
                    else ESP_LOGW(TAG, "nodeClass not found %s", nodeState["nodeName"].as<String>().c_str());
                }
            }
            // end Nodes
        }
        // else
        // ESP_LOGD(TAG, "no handle for %s[%d]%s[%d].%s = %s -> %s", updatedItem.parent[0].c_str(), updatedItem.index[0], updatedItem.parent[1].c_str(), updatedItem.index[1], updatedItem.name.c_str(), updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
    }

    void loop() {
        if (requestUIUpdate) {
            requestUIUpdate = false; //reset the flag
            ESP_LOGD(TAG, "requestUIUpdate");

            // update state to UI
            update([&](ModuleState &state) {
                return StateUpdateResult::CHANGED; // notify StatefulService by returning CHANGED
            }, "server");
        }
    }
};

#endif
#endif