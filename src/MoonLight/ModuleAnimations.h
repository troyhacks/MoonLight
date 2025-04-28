/**
    @title     MoonLight
    @file      ModuleAnimations.h
    @repo      https://github.com/ewowi/MoonBase, submit changes to this file as PRs
    @Authors   https://github.com/ewowi/MoonBase/commits/main
    @Doc       https://ewowi.github.io/MoonBase/modules/module/animations/
    @Copyright © 2025 Github MoonBase Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
**/

#ifndef ModuleAnimations_h
#define ModuleAnimations_h

#if FT_MOONLIGHT == 1

#undef TAG
#define TAG "💫"

#include "FastLED.h"
#include "../MoonBase/Module.h"

#include "Nodes.h" //Nodes.h will include VirtualLayer.h which will include PhysicalLayer.h

PhysicalLayer layerP; //global declaration of the physical layer

class ModuleAnimations : public Module
{
public:

    ModuleAnimations(PsychicHttpServer *server,
        ESP32SvelteKit *sveltekit,
        FilesService *filesService
    ) : Module("animations", server, sveltekit, filesService) {
        ESP_LOGD(TAG, "constructor");
    }

    void begin() {
        Module::begin();

        ESP_LOGD(TAG, "");

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
                                LiveScriptNode *liveScriptNode = (LiveScriptNode *)findNode(node["animation"]);
                                if (liveScriptNode) liveScriptNode->compileAndRun();
                            }
                        }
                    }
                });
            });
        #endif

        #if FT_ENABLED(FT_MONITOR)
            _socket->registerEvent("monitor");
        #endif
    }

    //define the data model
    void setupDefinition(JsonArray root) override {
        ESP_LOGD(TAG, "");
        JsonObject property; // state.data has one or more properties
        JsonArray details = root; // if a property is an array, this is the details of the array
        JsonArray values; // if a property is a select, this is the values of the select

        property = root.add<JsonObject>(); property["name"] = "lightsOn"; property["type"] = "checkbox"; property["default"] = true;
        property = root.add<JsonObject>(); property["name"] = "brightness"; property["type"] = "range"; property["default"] = 10;
        property = root.add<JsonObject>(); property["name"] = "red"; property["type"] = "range"; property["default"] = 128; property["color"] = "Red";
        property = root.add<JsonObject>(); property["name"] = "green"; property["type"] = "range"; property["default"] = 128; property["color"] = "Green";
        property = root.add<JsonObject>(); property["name"] = "blue"; property["type"] = "range"; property["default"] = 128; property["color"] = "Blue";
        property = root.add<JsonObject>(); property["name"] = "preset"; property["type"] = "select"; property["default"] = "Preset1"; values = property["values"].to<JsonArray>();
        values.add("Preset1");
        values.add("Preset2");
        property = root.add<JsonObject>(); property["name"] = "driverOn"; property["type"] = "checkbox"; property["default"] = true;
        property = root.add<JsonObject>(); property["name"] = "pin"; property["type"] = "select"; property["default"] = 2; values = property["values"].to<JsonArray>();
        values.add("2");
        values.add("16");

        #if FT_ENABLED(FT_MONITOR)
            property = root.add<JsonObject>(); property["name"] = "monitorOn"; property["type"] = "checkbox"; property["default"] = true;
        #endif

        property = root.add<JsonObject>(); property["name"] = "nodes"; property["type"] = "array"; details = property["n"].to<JsonArray>();
        {
            property = details.add<JsonObject>(); property["name"] = "animation"; property["type"] = "selectFile"; property["default"] = "Random"; values = property["values"].to<JsonArray>();
            values.add("Solid🔥");
            values.add("Random🔥");
            values.add("Sinelon🔥");
            values.add("Rainbow🔥");
            values.add("Sinus🔥");
            values.add("Lissajous🔥");
            values.add("Lines🔥");
            values.add("Panel🚥");
            values.add("Multiply💎");
            values.add("Mirror💎");
            values.add("Pinwheel💎");
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

        #if FT_ENABLED(FT_LIVESCRIPT)
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
        #endif
    }

    //implement business logic
    void onUpdate(UpdatedItem &updatedItem) override
    {
        if (equal(updatedItem.name, "pin") || equal(updatedItem.name, "red") || equal(updatedItem.name, "green") || equal(updatedItem.name, "blue")) {
            ESP_LOGD(TAG, "handle %s[%d]%s[%d].%s = %s -> %s", updatedItem.parent[0], updatedItem.index[0], updatedItem.parent[1], updatedItem.index[1], updatedItem.name, updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());

            //addLeds twice is temp hack to make rgb sliders work
            switch (_state.data["pin"].as<int>()) {
                case 2:
                    FastLED.addLeds<WS2812B, 16, GRB>(layerP.leds, 0, layerP.nrOfLeds).setCorrection(CRGB(_state.data["red"],_state.data["green"],_state.data["blue"]));
                    FastLED.addLeds<WS2812B, 2, GRB>(layerP.leds, 0, layerP.nrOfLeds).setCorrection(CRGB(_state.data["red"],_state.data["green"],_state.data["blue"]));
                    break;
                case 16:
                    FastLED.addLeds<WS2812B, 2, GRB>(layerP.leds, 0, layerP.nrOfLeds).setCorrection(CRGB(_state.data["red"],_state.data["green"],_state.data["blue"]));
                    FastLED.addLeds<WS2812B, 16, GRB>(layerP.leds, 0, layerP.nrOfLeds).setCorrection(CRGB(_state.data["red"],_state.data["green"],_state.data["blue"]));
                    break;
                default:
                    ESP_LOGD(TAG, "unknown pin %d", _state.data["pin"].as<int>());
            }
            FastLED.setMaxPowerInMilliWatts(10000); // 5v, 2000mA, to protect usb while developing
            FastLED.setBrightness(_state.data["lightsOn"]?_state.data["brightness"]:0);
            ESP_LOGD(TAG, "FastLED.addLeds n:%d", layerP.nrOfLeds);
        } else if (equal(updatedItem.name, "lightsOn") || equal(updatedItem.name, "brightness")) {
            ESP_LOGD(TAG, "handle %s[%d]%s[%d].%s = %s -> %s", updatedItem.parent[0], updatedItem.index[0], updatedItem.parent[1], updatedItem.index[1], updatedItem.name, updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
            FastLED.setBrightness(_state.data["lightsOn"]?_state.data["brightness"]:0);

        // handle nodes
        } else if (equal(updatedItem.parent[0], "nodes")) { // onNodes
            JsonVariant nodeState = _state.data["nodes"][updatedItem.index[0]];

            if (equal(updatedItem.name, "animation")) { //onAnimation

                if (updatedItem.oldValue != "null") {
                    ESP_LOGD(TAG, "handle %s[%d]%s[%d].%s = %s -> %s", updatedItem.parent[0], updatedItem.index[0], updatedItem.parent[1], updatedItem.index[1], updatedItem.name, updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
                    nodeState.remove("controls"); //remove the controls
                    layerP.removeNode(updatedItem.oldValue.c_str());
                }
                
                // remove or add Nodes (incl controls)
                if (!nodeState["animation"].isNull()) { // if animation changed // == updatedItem.value
                    ESP_LOGD(TAG, "handle %s[%d]%s[%d].%s = %s -> %s", updatedItem.parent[0], updatedItem.index[0], updatedItem.parent[1], updatedItem.index[1], updatedItem.name, updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
                    // delete the old animation, create a new one
                    // if (updatedItem.oldValue != "null") {
                    //     layerP.removeNode(updatedItem.oldValue.c_str());
                    // }
    
                    Node *nodeClass = layerP.addNode(nodeState["animation"]); // fill the layers and effects ...
                    
                    nodeState["controls"].to<JsonArray>(); //clear the controls
                    nodeClass->getControls(nodeState["controls"]); //create the controls
                    //show these controls in the UI
                    JsonObject object = _state.data.as<JsonObject>();
                    _socket->emitEvent("animations", object);

                    //if node is modifier, rerun fixture definition
                    if (nodeClass->hasModifier) {
                        for (Node *node : layerP.layerV[0]->nodes) {
                            if (node->hasFixDef && node->on) {
                                ESP_LOGD(TAG, "Modifier control changed -> setup %s", node->animation);
                                node->setup();
                            }
                        }
                    }
                }

                // if (nodeState["animation"].isNull()) {
                //     ESP_LOGD(TAG, "handle %s[%d]%s[%d].%s = %s -> %s", updatedItem.parent[0], updatedItem.index[0], updatedItem.parent[1], updatedItem.index[1], updatedItem.name, updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
                //     // delete the old animation
                //     if (updatedItem.oldValue != "null") {
                //         layerP.removeNode(updatedItem.oldValue.c_str());
                //     }
                // }
    
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
                Node * nodeClass = findNode(nodeState["animation"]);
                if (nodeClass) {
                    nodeClass->on = updatedItem.value.as<bool>();
                    if (nodeClass->hasModifier) {
                        for (Node *node : layerP.layerV[0]->nodes) {
                            if (node->hasFixDef && node->on) {
                                ESP_LOGD(TAG, "Modifier control changed -> setup %s", node->animation);
                                node->setup();
                            }
                        }
                    }
                    if (nodeClass->hasFixDef && !nodeClass->on) {
                        //if fixdef has been set to off, remove the mapping
                    }
                }
            }

            if (equal(updatedItem.parent[1], "controls") && equal(updatedItem.name, "value")) {    //process controls values 
                ESP_LOGD(TAG, "handle %s[%d]%s[%d].%s = %s -> %s", updatedItem.parent[0], updatedItem.index[0], updatedItem.parent[1], updatedItem.index[1], updatedItem.name, updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
    
                Node * nodeClass = findNode(nodeState["animation"]);
                if (nodeClass) {
                    nodeClass->setControl(nodeState["controls"][updatedItem.index[1]]); //to do only send the changed control
                    // if Modfier control changed, rerun the fixture definition
                    // find nodes which implement the Modifier interface
                    // find nodes which implement ModifyPixelsPre and ModifyPixel
                    // if this nodes implements ModifyPixelsPre and ModifyPixel then rerun fixdef setup

                    // ESP_LOGD(TAG, "nodeClass type %s", nodeClass->scriptType);
                    if (nodeClass->hasModifier) {
                        for (Node *node : layerP.layerV[0]->nodes) {
                            if (node->hasFixDef && node->on) {
                                ESP_LOGD(TAG, "Modifier control changed -> setup %s", node->animation);
                                node->setup();
                            }
                        }
                    }
                }
                else ESP_LOGW(TAG, "nodeClass not found %s", nodeState["animation"].as<String>().c_str());
    
            }
            // end Nodes

        //scripts
        } else if (equal(updatedItem.parent[0], "scripts")) {    
            JsonVariant scriptState = _state.data["scripts"][updatedItem.index[0]];
            ESP_LOGD(TAG, "handle %s[%d]%s[%d].%s = %s -> %s", updatedItem.parent[0], updatedItem.index[0], updatedItem.parent[1], updatedItem.index[1], updatedItem.name, updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
            #if FT_ENABLED(FT_LIVESCRIPT)
                if (updatedItem.oldValue != "null") {//do not run at boot!
                    LiveScriptNode *liveScriptNode = (LiveScriptNode *)findNode(scriptState["name"]);
                    if (liveScriptNode) {
                        if (equal(updatedItem.name, "stop"))
                            liveScriptNode->kill();
                        if (equal(updatedItem.name, "start"))
                            liveScriptNode->execute();
                        // if (equal(updatedItem.name, "free"))
                        //     liveScriptNode->free();
                        if (equal(updatedItem.name, "delete"))
                            liveScriptNode->killAndDelete();
                        // updatedItem.value = 0;
                    } else ESP_LOGW(TAG, "liveScriptNode not found %s", scriptState["name"].as<String>().c_str());
                }
            #endif
        } 
        // else
        // ESP_LOGD(TAG, "no handle for %s[%d]%s[%d].%s = %s -> %s", updatedItem.parent[0], updatedItem.index[0], updatedItem.parent[1], updatedItem.index[1], updatedItem.name, updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
    }

    //run effects
    void loop()
    {
        layerP.loop(); //run all the effects of all virtual layers (currently only one)

        //show connected clients on the led display
        // static uint8_t lastConnectedClients = 0;
        // if (_socket->getConnectedClients() == 0) lastConnectedClients++;
        for (int i = 0; i < _socket->getConnectedClients(); i++) {
            // ESP_LOGD(TAG, "socket %d", i);
            layerP.leds[i] = CRGB(0, 0, 128);
        }

        driverShow();
    }

    void loop50ms() {
        #if FT_ENABLED(FT_MONITOR)
            if (!_socket->getConnectedClients()) return; 
            static int monitorMillis = 0;
            if (millis() - monitorMillis >= layerP.nrOfLeds / 12 && _state.data["monitorOn"]) { //max 12000 leds per second -> 1 second for 12000 leds
                monitorMillis = millis();
                _socket->emitEvent("monitor", (char *)layerP.leds, MIN(layerP.nrOfLeds, NUM_LEDS) * sizeof(CRGB));// + 3); //3 bytes for type and factor and ...
            }
        #endif
    }

    //update scripts / read only values in the UI
    void loop1s() {

        if (!_socket->getConnectedClients()) return; 

        #if FT_ENABLED(FT_LIVESCRIPT)
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

        #endif
            
        // char buffer[256];
        // serializeJson(doc, buffer, sizeof(buffer));
        // ESP_LOGD(TAG, "livescripts %s", buffer);
    }

    void driverShow()
    {
        if (_state.data["driverOn"] && FastLED.count()) {
            FastLED.show();
        }
    }

    Node *findNode(const char *animation) {
        for (Node *node : layerP.layerV[0]->nodes) {
            // Check if the node is of type LiveScriptNode

                if (equal(node->animation, animation)) {
                    ESP_LOGD(TAG, "found %s", animation);
                    return node;
                }
        }
        return nullptr;
    }
  
}; // class ModuleAnimations

#endif
#endif