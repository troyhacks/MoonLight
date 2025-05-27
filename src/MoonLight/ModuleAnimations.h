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

    void begin() {
        Module::begin();

        ESP_LOGD(TAG, "L:%d(%d) LH:%d N:%d PL:%d(%d) VL:%d MH:%d", sizeof(Lights), sizeof(LightsHeader), sizeof(Lights) - sizeof(LightsHeader), sizeof(Node), sizeof(PhysicalLayer), sizeof(PhysicalLayer)-sizeof(Lights), sizeof(VirtualLayer), sizeof(MovingHead));

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
                        for (JsonObject node: _state.data["nodes"].as<JsonArray>()) {
                            String animation = node["animation"];

                            if (updatedItem == animation) {
                                ESP_LOGD(TAG, "updateHandler updatedItem %s", updatedItem.c_str());
                                LiveScriptNode *liveScriptNode = findLiveScriptNode(node["animation"]);
                                if (liveScriptNode) liveScriptNode->compileAndRun();
                            }
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
                        ESP_LOGD(TAG, "Modifier control changed -> setup %s", node->name());
                        for (layerP.pass = 1; layerP.pass <=1; layerP.pass++) //only virtual mapping
                            node->map();
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
            values.add(LinesEffect::name());
            values.add(LissajousEffect::name());
            values.add(MovingHeadEffect::name());
            values.add(RainbowEffect::name());
            values.add(RandomEffect::name());
            values.add(RipplesEffect::name());
            values.add(RGBWParEffect::name());
            values.add(SinelonEffect::name());
            values.add(SinusEffect::name());
            values.add(SphereMoveEffect::name());
            values.add(DMXLayout::name());
            values.add(PanelLayout::name());
            values.add(RingsLayout::name());
            values.add(MirrorModifier::name());
            values.add(MultiplyModifier::name());
            values.add(PinwheelModifier::name());
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
        // handle nodes
        if (equal(updatedItem.parent[0], "nodes")) { // onNodes
            JsonVariant nodeState = _state.data["nodes"][updatedItem.index[0]];

            if (equal(updatedItem.name, "animation")) { //onAnimation

                Node *oldNode = layerP.layerV[0]->nodes.size() > updatedItem.index[0]?layerP.layerV[0]->nodes[updatedItem.index[0]]:nullptr; //find the node in the nodes list
                bool newNode = false;

                if (updatedItem.oldValue != "null") {
                    if (nodeState.isNull()) {
                        ESP_LOGW(TAG, "nodeState is null!");
                    } else {
                        // ESP_LOGD(TAG, "nodeState: %s", nodeState.as<String>().c_str());
                        nodeState.remove("controls"); //remove the controls, node itself removed after new node is placed ... assert failed: multi_heap_free multi_heap_poisoning.c:259 (head != NULL)
                        // nodeState["controls"].to<JsonArray>(); //clear the controls
                    }
                }

                // remove or add Nodes (incl controls)
                if (!nodeState["animation"].isNull()) { // if animation changed // == updatedItem.value
                    ESP_LOGD(TAG, "add %s[%d]%s[%d].%s = %s -> %s", updatedItem.parent[0], updatedItem.index[0], updatedItem.parent[1], updatedItem.index[1], updatedItem.name, updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
    
                    Node *nodeClass = layerP.addNode(nodeState["animation"], updatedItem.index[0]);
                    nodeClass->on = nodeState["on"];
                    newNode = true;

                    // nodeState.remove("controls"); //clear the controls
                    nodeState["controls"].to<JsonArray>(); //clear the controls
                    nodeClass->addControls(nodeState["controls"]); //create the controls

                    //show these controls in the UI, send notifiers to listeners...
                    //save ??
                    // JsonObject object = _state.data.as<JsonObject>();
                    // _socket->emitEvent(_moduleName, object);
                    update([&](ModuleState &state) {
                        ESP_LOGD(TAG, "update due to new node %s", updatedItem.value.as<String>().c_str());

                        // UpdatedItem updatedItem;
                        ; //compare and update
                        // state.data["scripts"] = newData["scripts"]; //update without compareRecursive -> without handles
                        // return state.compareRecursive("scripts", state.data["scripts"], newData["scripts"], updatedItem)?StateUpdateResult::CHANGED:StateUpdateResult::UNCHANGED;
                        return StateUpdateResult::CHANGED; // notify StatefulService by returning CHANGED
                    }, "server");
                    ESP_LOGD(TAG, "update due to new node %s done", updatedItem.value.as<String>().c_str());

                    //make sure "p" is also updated

                    //if node is a modifier, run the layout definition
                    if (nodeClass->hasModifier) {
                        for (Node *node : layerP.layerV[0]->nodes) {
                            if (node->hasLayout && node->on) {
                                ESP_LOGD(TAG, "Modifier control changed -> setup %s", node->name());
                                node->setup();
                            }
                        }
                    }
                }

                //if a node existed and no new node in place, remove 
                if (updatedItem.oldValue != "null" && oldNode) {
                    ESP_LOGD(TAG, "remove %s[%d]%s[%d].%s = %s -> %s", updatedItem.parent[0], updatedItem.index[0], updatedItem.parent[1], updatedItem.index[1], updatedItem.name, updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
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

            if (equal(updatedItem.name, "on")) {
                if (layerP.layerV[0]->nodes.size() > updatedItem.index[0]) { //could be remoced by onAnimation
                    Node *nodeClass = layerP.layerV[0]->nodes[updatedItem.index[0]];
                    if (nodeClass) {
                        ESP_LOGD(TAG, "on %s %d", updatedItem.name, updatedItem.value.as<bool>());
                        nodeClass->on = updatedItem.value.as<bool>(); //set nodeclass on/off
                        if (nodeClass->hasModifier) { //nodeClass->on && //if class has modifier, run the layout (if on) - which uses all the modifiers ...
                            for (Node *node : layerP.layerV[0]->nodes) {
                                if (node->hasLayout && node->on) {
                                    ESP_LOGD(TAG, "Modifier control changed -> setup %s", node->name());
                                    node->setup();
                                }
                            }
                        }
                        if (nodeClass->hasLayout) {
                            //if layout has been set to off, remove the mapping
                            nodeClass->setup(); // rerun setup (which checks for on)
                            if (!nodeClass->on)
                                nodeClass->loop(); // run the loop once to update the layout (off cancels the loop)
                        }
                    }
                }
            }

            if (equal(updatedItem.parent[1], "controls") && equal(updatedItem.name, "value")) {    //process controls values 
                ESP_LOGD(TAG, "handle %s[%d]%s[%d].%s = %s -> %s", updatedItem.parent[0], updatedItem.index[0], updatedItem.parent[1], updatedItem.index[1], updatedItem.name, updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
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
                                if (node->hasLayout && node->on) {
                                    ESP_LOGD(TAG, "Modifier control changed -> setup layout %s", node->name());
                                    node->setup();
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
        if (layerP.lights.header.type == ct_Leds) //otherwise lights is used for positions etc.
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