/**
    @title     MoonLight
    @file      ModuleAnimations.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/modules/module/animations/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
**/

#ifndef ModuleAnimations_h
#define ModuleAnimations_h

#if FT_MOONLIGHT

#undef TAG
#define TAG "💫"

#include "FastLED.h"
#include "../MoonBase/Module.h"

#include "Nodes.h" //Nodes.h will include VirtualLayer.h which will include PhysicalLayer.h

PhysicalLayer layerP; //global declaration of the physical layer

class ModuleAnimations : public Module
{
public:

    PsychicHttpServer *_server;

    ModuleAnimations(PsychicHttpServer *server,
        ESP32SvelteKit *sveltekit,
        FilesService *filesService
    ) : Module("animations", server, sveltekit, filesService) {
        ESP_LOGD(TAG, "constructor");
        _server = server;
    }

    void updateControls(uint8_t index) {
        ESP_LOGD(TAG, "updateControls %d < %d ?", index, _state.data["nodes"].size());
        if (index < _state.data["nodes"].size()) {
            JsonVariant nodeState = _state.data["nodes"][index];
            ESP_LOGD(TAG, "updateControls found nodeState %d", index);
            nodeState["controls"].to<JsonArray>(); //clear the controls
            if (index < layerP.layerV[0]->nodes.size()) {
                Node *node = layerP.layerV[0]->nodes[index];
                ESP_LOGD(TAG, "updateControls found node class %d", index);
                if (node) {
                    node->addControls(nodeState["controls"].as<JsonArray>()); //update controls

                    // for (JsonVariant control : nodeState["controls"].as<JsonArray>()) {
                    //     ESP_LOGD(TAG, "updateControls control %s", control["name"].as<String>().c_str());
                    // }

                    // serializeJson(nodeState, Serial); Serial.println();

                    // JsonObject object = _state.data.as<JsonObject>();
                    // _socket->emitEvent(_moduleName, object);

                    update([&](ModuleState &state) {
                        // ESP_LOGD(TAG, "update due to new node %s", animation.c_str());

                        // UpdatedItem updatedItem;
                        ; //compare and update
                        // state.data["scripts"] = newData["scripts"]; //update without compareRecursive -> without handles
                        // return state.compareRecursive("scripts", state.data["scripts"], newData["scripts"], updatedItem)?StateUpdateResult::CHANGED:StateUpdateResult::UNCHANGED;
                        return StateUpdateResult::CHANGED; // notify StatefulService by returning CHANGED
                    }, "server");
                }
            }
        }
    }

    void begin() {
        Module::begin();

        #if FT_ENABLED(FT_LIVESCRIPT)
            //create a handler which recompiles the animation when the file of the current animation changes in the File Manager
            _filesService->addUpdateHandler([&](const String &originId)
            { 
                ESP_LOGD(TAG, "FilesService::updateHandler %s", originId.c_str());
                //read the file state
                _filesService->read([&](FilesState &filesState) {
                    // loop over all changed files (normally only one)
                    for (auto updatedItem : filesState.updatedItems) {
                        //if file is the current animation, recompile it (to do: multiple animations)
                        uint8_t index = 0;
                        for (JsonObject nodeState: _state.data["nodes"].as<JsonArray>()) {
                            String animation = nodeState["animation"];

                            if (updatedItem == animation) {
                                ESP_LOGD(TAG, "updateHandler updatedItem %s", updatedItem.c_str());
                                LiveScriptNode *liveScriptNode = findLiveScriptNode(nodeState["animation"]);
                                if (liveScriptNode) liveScriptNode->compileAndRun();

                                updateControls(index);

                                ESP_LOGD(TAG, "update due to new node %s done", animation.c_str());
                            }
                            index++;
                        }
                    }
                });
            });
        #endif

        #if FT_ENABLED(FT_MONITOR)
            _socket->registerEvent("monitor");
            _server->on("/rest/monitorLayout", HTTP_GET, [&](PsychicRequest *request) {
                ESP_LOGD(TAG, "rest monitor triggered");

                //trigger pass 1 mapping of layout
                for (Node *node : layerP.layerV[0]->nodes) {
                    if (node->hasLayout && node->on) {
                        ESP_LOGD(TAG, "Monitor request -> layout map 1 %s", node->name());
                        for (layerP.pass = 1; layerP.pass <=1; layerP.pass++) //only pass1: virtual mapping
                            node->mapLayout();
                    }
                }

                PsychicJsonResponse response = PsychicJsonResponse(request, false);
                return response.send();
            });
        #endif
    }

    //define the data model
    void setupDefinition(JsonArray root) override {
        ESP_LOGD(TAG, "");
        JsonObject property; // state.data has one or more properties
        JsonArray details = root; // if a property is an array, this is the details of the array
        JsonArray values; // if a property is a select, this is the values of the select

        property = root.add<JsonObject>(); property["name"] = "nodes"; property["type"] = "array"; details = property["n"].to<JsonArray>();
        {
            property = details.add<JsonObject>(); property["name"] = "animation"; property["type"] = "selectFile"; property["default"] = "Random🔥"; values = property["values"].to<JsonArray>();
            values.add(SolidEffect::name());
            //alphabetically from here
            values.add(BouncingBallsEffect::name());
            values.add(GEQEffect::name());
            values.add(LinesEffect::name());
            values.add(LissajousEffect::name());
            values.add(MovingHeadEffect::name());
            values.add(PaintBrushEffect::name());
            values.add(RainbowEffect::name());
            values.add(RandomEffect::name());
            values.add(RipplesEffect::name());
            values.add(RGBWParEffect::name());
            values.add(SinelonEffect::name());
            values.add(SinusEffect::name());
            values.add(SphereMoveEffect::name());
            values.add(MovingHeadLayout::name());
            values.add(PanelLayout::name());
            values.add(RingsLayout::name());
            values.add(MirrorModifier::name());
            values.add(MultiplyModifier::name());
            values.add(PinwheelModifier::name());
            values.add(AudioSyncMod::name());
            //find all the .sc files on FS
            File rootFolder = ESPFS.open("/");
            walkThroughFiles(rootFolder, [&](File folder, File file) {
                if (strstr(file.name(), ".sc")) {
                    // ESP_LOGD(TAG, "found file %s", file.path());
                    values.add((char *)file.path());
                }
            });
            rootFolder.close();
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
            const char *animation = nodeState["animation"];
            // serializeJson(nodeState, Serial); Serial.println();

            if (updatedItem.name == "animation") { //onAnimation

                Node *oldNode = layerP.layerV[0]->nodes.size() > updatedItem.index[0]?layerP.layerV[0]->nodes[updatedItem.index[0]]:nullptr; //find the node in the nodes list
                bool newNode = false;

                // remove or add Nodes (incl controls)
                if (!nodeState["animation"].isNull()) { // if animation changed // == updatedItem.value
                    ESP_LOGD(TAG, "add %s[%d]%s[%d].%s = %s -> %s task %s %d", updatedItem.parent[0], updatedItem.index[0], updatedItem.parent[1], updatedItem.index[1], updatedItem.name, updatedItem.oldValue.c_str(), updatedItem.value.c_str(), pcTaskGetName(currentTask), uxTaskGetStackHighWaterMark(currentTask));
    
                    Node *nodeClass = layerP.addNode(nodeState["animation"], updatedItem.index[0]);
                    nodeClass->on = nodeState["on"];
                    newNode = true;

                    updateControls(updatedItem.index[0]); //update controls of the new node

                    ESP_LOGD(TAG, "update due to new node %s done", updatedItem.value.c_str());

                    //make sure "p" is also updated

                    //if node is a modifier, run the layout definition
                    if (nodeClass->hasModifier) {
                        for (Node *node : layerP.layerV[0]->nodes) {
                            if (node->hasLayout) {
                                ESP_LOGD(TAG, "Modifier created -> remap layout %s", node->name());
                                node->requestMap = true; //request mapping
                            }
                        }
                    }
                }

                //if a node existed and no new node in place, remove 
                if (updatedItem.oldValue != "null" && oldNode) {
                    ESP_LOGD(TAG, "remove %s[%d]%s[%d].%s = %s -> %s", updatedItem.parent[0], updatedItem.index[0], updatedItem.parent[1], updatedItem.index[1], updatedItem.name, updatedItem.oldValue.c_str(), updatedItem.value.c_str());
                    if (!newNode) {
                        layerP.layerV[0]->nodes.pop_back();
                        ESP_LOGD(TAG, "No newnode - remove! %d s:%d", updatedItem.index[0], layerP.layerV[0]->nodes.size());
                    }
                    layerP.removeNode(oldNode);
                }
                    
                #if FT_ENABLED(FT_LIVESCRIPT)
                    // if (updatedItem.oldValue.length()) {
                    //     ESP_LOGD(TAG, "delete %s %s ...", updatedItem.name, updatedItem.oldValue.c_str());
                    //     LiveScriptNode *liveScriptNode = findLiveScriptNode(node["animation"]);
                    //     if (liveScriptNode) liveScriptNode->kill(); 
                    //     else ESP_LOGW(TAG, "liveScriptNode not found %s", node["animation"].as<String>().c_str());
                    // }
                    // if (!node["animation"].isNull() && !node["type"].isNull()) {
                    //     LiveScriptNode *liveScriptNode = findLiveScriptNode(node["animation"]); //todo: can be 2 nodes with the same name ...
                    //     if (liveScriptNode) liveScriptNode->compileAndRun();
                    //     // not needed as creating the node is already running it ...
                    // }
                #endif
            }

            if (updatedItem.name == "on") {
                if (layerP.layerV[0]->nodes.size() > updatedItem.index[0]) { //could be remoced by onAnimation
                    ESP_LOGD(TAG, "on %s %s (%d)", updatedItem.name, updatedItem.value.c_str(), layerP.layerV[0]->nodes.size());
                    Node *nodeClass = layerP.layerV[0]->nodes[updatedItem.index[0]];
                    if (nodeClass) {
                        nodeClass->on = updatedItem.value == "true"; //set nodeclass on/off
                        if (nodeClass->hasModifier) { //nodeClass->on && //if class has modifier, run the layout (if on) - which uses all the modifiers ...
                            for (Node *node : layerP.layerV[0]->nodes) {
                                if (node->hasLayout) {
                                    ESP_LOGD(TAG, "Modifier on/off -> remap layout %s", node->name());
                                    node->requestMap = true; //request mapping
                                }
                            }
                        }
                        if (nodeClass->hasLayout) {
                            ESP_LOGD(TAG, "Layout on/off -> remap layout %s", animation);
                            nodeClass->requestMap = true; // rerun setup (which checks for on)
                            if (!nodeClass->on)
                                nodeClass->loop(); // run the loop once to update the layout (off cancels the loop)
                        }
                    }
                }
            }

            if (updatedItem.parent[1] == "controls" && updatedItem.name == "value") {    //process controls values 
                ESP_LOGD(TAG, "handle %s[%d]%s[%d].%s = %s -> %s", updatedItem.parent[0], updatedItem.index[0], updatedItem.parent[1], updatedItem.index[1], updatedItem.name, updatedItem.oldValue.c_str(), updatedItem.value.c_str());
                if (layerP.layerV[0]->nodes.size() > updatedItem.index[0]) { //could be removed by onAnimation
                    Node *nodeClass = layerP.layerV[0]->nodes[updatedItem.index[0]];
                    if (nodeClass) {
                        nodeClass->updateControl(nodeState["controls"][updatedItem.index[1]]);
                        // if Modfier control changed, run the layout
                        // find nodes which implement the Modifier interface
                        // find nodes which implement modifyLayout and modifyLight
                        // if this nodes implements modifyLayout and modifyLight then run lights lyayout map

                        // ESP_LOGD(TAG, "nodeClass type %s", nodeClass->scriptType);
                        if (nodeClass->on && nodeClass->hasModifier) {
                            for (Node *node : layerP.layerV[0]->nodes) {
                                if (node->hasLayout) {
                                    ESP_LOGD(TAG, "Modifier control changed -> remap layout %s", node->name());
                                    node->requestMap = true; //request mapping
                                }
                            }
                        }
                    }
                    else ESP_LOGW(TAG, "nodeClass not found %s", nodeState["animation"].as<String>().c_str());
                }
            }
            // end Nodes
        }
        // else
        // ESP_LOGD(TAG, "no handle for %s[%d]%s[%d].%s = %s -> %s", updatedItem.parent[0], updatedItem.index[0], updatedItem.parent[1], updatedItem.index[1], updatedItem.name, updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
    }

    //run effects
    void loop() {
        if (layerP.lights.header.isPositions == 0) //otherwise lights is used for positions etc.
            layerP.loop(); //run all the effects of all virtual layers (currently only one)

        //show connected clients on the led display
        // for (int i = 0; i < _socket->getConnectedClients(); i++) {
        //     // ESP_LOGD(TAG, "socket %d", i);
        //     layerP.lights.leds[i] = CRGB(0, 0, 128);
        // }
    }

    #if FT_ENABLED(FT_LIVESCRIPT)
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
    #endif
  
}; // class ModuleAnimations

#endif
#endif