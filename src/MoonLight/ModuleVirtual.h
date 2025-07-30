/**
    @title     MoonLight
    @file      ModuleVirtual.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/moonbase/module/editor/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact us for more information.
**/

#ifndef ModuleVirtual_h
#define ModuleVirtual_h

#if FT_MOONLIGHT

#undef TAG
#define TAG "💫"

#include "FastLED.h"
// #include "../MoonBase/Module.h"
#include "NodeManager.h"

// #include "Nodes.h" //Nodes.h will include VirtualLayer.h which will include PhysicalLayer.h

class ModuleVirtual : public NodeManager
{
public:

    FileManager *_fileManager;

    ModuleVirtual(PsychicHttpServer *server,
        ESP32SvelteKit *sveltekit,
        FileManager *fileManager
    ) : NodeManager("virtual", server, sveltekit) {
        ESP_LOGD(TAG, "constructor");
        _fileManager = fileManager;
    }

    void begin() {
        _state.onUpdateRunInTask = 1;
        NodeManager::begin();

        nodes = &(layerP.layerV[0]->nodes);

        // if (false)
        //create a handler which recompiles the live script when the file of a current running live script changes in the File Manager
        _fileManager->addUpdateHandler([&](const String &originId)
        { 
            ESP_LOGD(TAG, "FileManager::updateHandler %s", originId.c_str());
            //read the file state (read all files and folders on FS and collect changes)
            _fileManager->read([&](FilesState &filesState) {
                // loop over all changed files (normally only one)
                for (auto updatedItem : filesState.updatedItems) {
                    //if file is the current live script, recompile it (to do: multiple live effects)
                    ESP_LOGD(TAG, "updateHandler updatedItem %s", updatedItem.c_str());
                    if (equal(updatedItem.c_str(), "/.config/virtual.json")) {
                        ESP_LOGD(TAG, " virtual.json updated -> call update %s", updatedItem.c_str());
                        readFromFS(); //repopulates the state, processing file changes
                    }
                    // uint8_t index = 0;
                    // for (JsonObject nodeState: _state.data["nodes"].as<JsonArray>()) {
                    //     String nodeName = nodeState["nodeName"];

                    //     if (updatedItem == nodeName) {
                    //         ESP_LOGD(TAG, "updateHandler equals current item -> livescript compile %s", updatedItem.c_str());
                    //         LiveScriptNode *liveScriptNode = (LiveScriptNode *)layerP.layerV[0]->findLiveScriptNode(nodeState["nodeName"]);
                    //         if (liveScriptNode) {
                    //             liveScriptNode->compileAndRun();

                    //             //wait until setup has been executed?

                    //             _moduleVirtual->requestUIUpdate = true; //update the virtual layers UI
                    //         }

                    //         ESP_LOGD(TAG, "update due to new node %s done", nodeName.c_str());
                    //     }
                    //     index++;
                    // }
                }
            });
        });

        #if FT_ENABLED(FT_MONITOR)
            _socket->registerEvent("monitor");
            _server->on("/rest/monitorLayout", HTTP_GET, [&](PsychicRequest *request) {
                ESP_LOGD(TAG, "rest monitor triggered");

                //trigger pass 1 mapping of layout
                layerP.pass = 1; //(requestMapPhysical=1 physical rerun)
                layerP.mapLayout();

                PsychicJsonResponse response = PsychicJsonResponse(request, false);
                return response.send();
            });
        #endif
    }

    void addNodes(JsonArray values) override {
        values.add(SolidEffect::name());
        //alphabetically from here
        values.add(BouncingBallsEffect::name());
        values.add(DistortionWavesEffect::name());
        values.add(FreqMatrixEffect::name());
        values.add(GEQEffect::name());
        values.add(LinesEffect::name());
        values.add(LissajousEffect::name());
        values.add(PaintBrushEffect::name());
        values.add(RainbowEffect::name());
        values.add(RandomEffect::name());
        values.add(RipplesEffect::name());
        values.add(RGBWParEffect::name());
        values.add(ScrollingTextEffect::name());
        values.add(SinusEffect::name());
        values.add(SphereMoveEffect::name());
        values.add(WaveEffect::name());
        values.add(FixedRectangleEffect::name());
        values.add(MHTroy1Effect::name());
        values.add(MHTroy2Effect::name());
        values.add(MHWowiEffect::name());

        values.add(CircleModifier::name());
        values.add(MirrorModifier::name());
        values.add(MultiplyModifier::name());
        values.add(RippleYZModifier::name());
        values.add(PinwheelModifier::name());
        values.add(RotateNodifier::name());

        //find all the .sc files on FS
        File rootFolder = ESPFS.open("/");
        walkThroughFiles(rootFolder, [&](File folder, File file) {
            if (strstr(file.name(), ".sc")) {
                // ESP_LOGD(TAG, "found file %s", file.path());
                values.add((char *)file.path());
            }
        });
        rootFolder.close();
    }

    Node* addNode(const uint8_t index, const char * name, const JsonArray controls) override {
        Node *node = nullptr;
        if (equal(name, SolidEffect::name())) node = new SolidEffect();
        //alphabetically from here
        else if (equal(name, BouncingBallsEffect::name())) node = new BouncingBallsEffect();
        else if (equal(name, DistortionWavesEffect::name())) node = new DistortionWavesEffect();
        else if (equal(name, FreqMatrixEffect::name())) node = new FreqMatrixEffect();
        else if (equal(name, GEQEffect::name())) node = new GEQEffect();
        else if (equal(name, LinesEffect::name())) node = new LinesEffect();
        else if (equal(name, LissajousEffect::name())) node = new LissajousEffect();
        else if (equal(name, PaintBrushEffect::name())) node = new PaintBrushEffect();
        else if (equal(name, RainbowEffect::name())) node = new RainbowEffect();
        else if (equal(name, RandomEffect::name())) node = new RandomEffect();
        else if (equal(name, RipplesEffect::name())) node = new RipplesEffect();
        else if (equal(name, RGBWParEffect::name())) node = new RGBWParEffect();
        else if (equal(name, ScrollingTextEffect::name())) node = new ScrollingTextEffect();
        else if (equal(name, SinusEffect::name())) node = new SinusEffect();
        else if (equal(name, SphereMoveEffect::name())) node = new SphereMoveEffect();
        else if (equal(name, FixedRectangleEffect::name())) node = new FixedRectangleEffect();
        else if (equal(name, WaveEffect::name())) node = new WaveEffect();
        else if (equal(name, MHTroy1Effect::name())) node = new MHTroy1Effect();
        else if (equal(name, MHTroy2Effect::name())) node = new MHTroy2Effect();
        else if (equal(name, MHWowiEffect::name())) node = new MHWowiEffect();

        else if (equal(name, CircleModifier::name())) node = new CircleModifier();
        else if (equal(name, MirrorModifier::name())) node = new MirrorModifier();
        else if (equal(name, MultiplyModifier::name())) node = new MultiplyModifier();
        else if (equal(name, RippleYZModifier::name())) node = new RippleYZModifier();
        else if (equal(name, PinwheelModifier::name())) node = new PinwheelModifier();
        else if (equal(name, RotateNodifier::name())) node = new RotateNodifier();
        
        #if FT_LIVESCRIPT
            else {
                LiveScriptNode *liveScriptNode = new LiveScriptNode();
                liveScriptNode->animation = name; //set the (file)name of the script
                node = liveScriptNode;
            }
        #endif

        if (node) {
            node->constructor(layerP.layerV[0], controls); //pass the layer to the node
            node->setup(); //run the setup of the effect
            // layerV[0]->nodes.reserve(index+1);
            if (index >= layerP.layerV[0]->nodes.size())
                layerP.layerV[0]->nodes.push_back(node);
            else
                layerP.layerV[0]->nodes[index] = node; //add the node to the layer
        }

        ESP_LOGD(TAG, "%s (s:%d p:%p)", name, layerP.layerV[0]->nodes.size(), node);

        return node;
    }

    //run effects
    void loop() {

        NodeManager::loop();

    }
  
}; // class ModuleVirtual

#endif
#endif