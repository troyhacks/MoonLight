/**
    @title     MoonLight
    @file      ModuleEffects.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/moonbase/module/editor/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact us for more information.
**/

#ifndef ModuleEffects_h
#define ModuleEffects_h

#if FT_MOONLIGHT

#include "FastLED.h"
// #include "../MoonBase/Module.h"
#include "Nodes/NodeManager.h"

// #include "Nodes.h" //Nodes.h will include VirtualLayer.h which will include PhysicalLayer.h

class ModuleEffects : public NodeManager
{
public:

    FileManager *_fileManager;

    ModuleEffects(PsychicHttpServer *server,
        ESP32SvelteKit *sveltekit,
        FileManager *fileManager
    ) : NodeManager("effects", server, sveltekit) {
        MB_LOGV(ML_TAG, "constructor");
        _fileManager = fileManager;
    }

    void begin() {
        _state.onUpdateRunInTask = 1;
        defaultNodeName = RandomEffect::name();
        NodeManager::begin();

        nodes = &(layerP.layerV[0]->nodes);

        // if (false)
        //create a handler which recompiles the live script when the file of a current running live script changes in the File Manager
        _fileManager->addUpdateHandler([&](const String &originId)
        { 
            MB_LOGV(ML_TAG, "FileManager::updateHandler %s", originId.c_str());
            //read the file state (read all files and folders on FS and collect changes)
            _fileManager->read([&](FilesState &filesState) {
                // loop over all changed files (normally only one)
                for (auto updatedItem : filesState.updatedItems) {
                    //if file is the current live script, recompile it (to do: multiple live effects)
                    MB_LOGV(ML_TAG, "updateHandler updatedItem %s", updatedItem.c_str());
                    if (equal(updatedItem.c_str(), "/.config/effects.json")) {
                        MB_LOGV(ML_TAG, " effects.json updated -> call update %s", updatedItem.c_str());
                        readFromFS(); //repopulates the state, processing file changes
                    }
                    // uint8_t index = 0;
                    // for (JsonObject nodeState: _state.data["nodes"].as<JsonArray>()) {
                    //     String name = nodeState["name"];

                    //     if (updatedItem == name) {
                    //         MB_LOGV(ML_TAG, "updateHandler equals current item -> livescript compile %s", updatedItem.c_str());
                    //         LiveScriptNode *liveScriptNode = (LiveScriptNode *)layerP.layerV[0]->findLiveScriptNode(nodeState["name"]);
                    //         if (liveScriptNode) {
                    //             liveScriptNode->compileAndRun();

                    //             //wait until setup has been executed?

                    //             _moduleEffects->requestUIUpdate = true; //update the Effects UI
                    //         }

                    //         MB_LOGV(ML_TAG, "update due to new node %s done", name.c_str());
                    //     }
                    //     index++;
                    // }
                }
            });
        });

        #if FT_ENABLED(FT_MONITOR)
            _socket->registerEvent("monitor");
            _server->on("/rest/monitorLayout", HTTP_GET, [&](PsychicRequest *request) {
                MB_LOGV(ML_TAG, "rest monitor triggered");

                //trigger pass 1 mapping of layout
                layerP.pass = 1; //(requestMapPhysical=1 physical rerun)
                layerP.monitorPass = true;
                layerP.mapLayout();
                layerP.monitorPass = false;

                PsychicJsonResponse response = PsychicJsonResponse(request, false);
                return response.send();
            });
        #endif

        _state.readHook = [&](JsonObject data) {
            data["start"]["x"] = 0;
            data["start"]["y"] = 0;
            data["start"]["z"] = 0;
            data["end"]["x"] = layerP.lights.header.size.x;
            data["end"]["y"] = layerP.lights.header.size.y;
            data["end"]["z"] = layerP.lights.header.size.z;
            data["brightness"] = layerP.lights.header.brightness;
        };
    }

    void setupDefinition(JsonArray root) override {
        MB_LOGV(ML_TAG, "");
        JsonObject property; // state.data has one or more properties
        JsonArray values; // if a property is a select, this is the values of the select
        property = root.add<JsonObject>(); property["name"] = "layer"; property["type"] = "select"; property["default"] = 0; values = property["values"].to<JsonArray>();
        uint8_t i = 0;
        for (VirtualLayer * layer: layerP.layerV) {
            values.add(i);
            i++;
        }
        property = root.add<JsonObject>(); property["name"] = "start"; property["type"] = "coord3D"; property["ro"] = true;
        property = root.add<JsonObject>(); property["name"] = "end"; property["type"] = "coord3D"; property["ro"] = true;
        property = root.add<JsonObject>(); property["name"] = "brightness"; property["type"] = "range"; property["ro"] = true;

        NodeManager::setupDefinition(root);
    }

    void addNodes(JsonArray values) override {
        values.add(SolidEffect::name());
        //alphabetically from here
        values.add(BouncingBallsEffect::name());
        values.add(BlurzEffect::name());
        values.add(DistortionWavesEffect::name());
        values.add(FreqMatrixEffect::name());
        values.add(GameOfLifeEffect::name());
        values.add(GEQEffect::name());
        values.add(GEQ3DEffect::name());
        values.add(FreqSawsEffect::name());
        values.add(LinesEffect::name());
        values.add(LissajousEffect::name());
        #if USE_M5UNIFIED
            values.add(MoonManEffect::name());
        #endif
        values.add(Noise2DEffect::name());
        values.add(NoiseMeterEffect::name());
        values.add(PaintBrushEffect::name());
        values.add(ParticlesEffect::name());
        values.add(PopCornEffect::name());
        values.add(RainbowEffect::name());
        values.add(RandomEffect::name());
        values.add(RipplesEffect::name());
        values.add(RubiksCubeEffect::name());
        values.add(ScrollingTextEffect::name());
        values.add(SinusEffect::name());
        values.add(SphereMoveEffect::name());
        values.add(StarFieldEffect::name());
        values.add(PraxisEffect::name());
        values.add(WaverlyEffect::name());
        values.add(WaveEffect::name());
        values.add(FixedRectangleEffect::name());
        values.add(Troy1ColorEffect::name());
        values.add(Troy1MoveEffect::name());
        values.add(Troy2ColorEffect::name());
        values.add(Troy2MoveEffect::name());
        values.add(FreqColorsEffect::name());
        values.add(WowiMoveEffect::name());
        values.add(AmbientMoveEffect::name());

        values.add(CheckerboardModifier::name());
        values.add(CircleModifier::name());
        values.add(MirrorModifier::name());
        values.add(MultiplyModifier::name());
        values.add(RippleYZModifier::name());
        values.add(PinwheelModifier::name());
        values.add(RotateNodifier::name());
        values.add(TransposeModifier::name());

        //find all the .sc files on FS
        File rootFolder = ESPFS.open("/");
        walkThroughFiles(rootFolder, [&](File folder, File file) {
            if (strstr(file.name(), ".sc")) {
                // MB_LOGV(ML_TAG, "found file %s", file.path());
                values.add((char *)file.path());
            }
        });
        rootFolder.close();
    }

    Node* addNode(const uint8_t index, const char * name, const JsonArray controls) override {
        Node *node = nullptr;
        if (equal(name, SolidEffect::name())) node = allocMBObject<SolidEffect>();
        //alphabetically from here
        else if (equal(name, BouncingBallsEffect::name())) node = allocMBObject<BouncingBallsEffect>();
        else if (equal(name, BlurzEffect::name())) node = allocMBObject<BlurzEffect>();
        else if (equal(name, DistortionWavesEffect::name())) node = allocMBObject<DistortionWavesEffect>();
        else if (equal(name, FreqMatrixEffect::name())) node = allocMBObject<FreqMatrixEffect>();
        else if (equal(name, GameOfLifeEffect::name())) node = allocMBObject<GameOfLifeEffect>();
        else if (equal(name, GEQEffect::name())) node = allocMBObject<GEQEffect>();
        else if (equal(name, GEQ3DEffect::name())) node = allocMBObject<GEQ3DEffect>();
        else if (equal(name, FreqSawsEffect::name())) node = allocMBObject<FreqSawsEffect>();
        else if (equal(name, LinesEffect::name())) node = allocMBObject<LinesEffect>();
        else if (equal(name, LissajousEffect::name())) node = allocMBObject<LissajousEffect>();
        #if USE_M5UNIFIED
            else if (equal(name, MoonManEffect::name())) node = allocMBObject<MoonManEffect>();
        #endif
        else if (equal(name, Noise2DEffect::name())) node = allocMBObject<Noise2DEffect>();
        else if (equal(name, NoiseMeterEffect::name())) node = allocMBObject<NoiseMeterEffect>();
        else if (equal(name, PaintBrushEffect::name())) node = allocMBObject<PaintBrushEffect>();
        else if (equal(name, ParticlesEffect::name())) node = allocMBObject<ParticlesEffect>();
        else if (equal(name, PopCornEffect::name())) node = allocMBObject<PopCornEffect>();
        else if (equal(name, RainbowEffect::name())) node = allocMBObject<RainbowEffect>();
        else if (equal(name, RandomEffect::name())) node = allocMBObject<RandomEffect>();
        else if (equal(name, RipplesEffect::name())) node = allocMBObject<RipplesEffect>();
        else if (equal(name, RubiksCubeEffect::name())) node = allocMBObject<RubiksCubeEffect>();
        else if (equal(name, ScrollingTextEffect::name())) node = allocMBObject<ScrollingTextEffect>();
        else if (equal(name, SinusEffect::name())) node = allocMBObject<SinusEffect>();
        else if (equal(name, SphereMoveEffect::name())) node = allocMBObject<SphereMoveEffect>();
        else if (equal(name, StarFieldEffect::name())) node = allocMBObject<StarFieldEffect>();
        else if (equal(name, PraxisEffect::name())) node = allocMBObject<PraxisEffect>();
        else if (equal(name, WaverlyEffect::name())) node = allocMBObject<WaverlyEffect>();
        else if (equal(name, FixedRectangleEffect::name())) node = allocMBObject<FixedRectangleEffect>();
        else if (equal(name, WaveEffect::name())) node = allocMBObject<WaveEffect>();
        else if (equal(name, Troy1ColorEffect::name())) node = allocMBObject<Troy1ColorEffect>();
        else if (equal(name, Troy1MoveEffect::name())) node = allocMBObject<Troy1MoveEffect>();
        else if (equal(name, Troy2ColorEffect::name())) node = allocMBObject<Troy2ColorEffect>();
        else if (equal(name, Troy2MoveEffect::name())) node = allocMBObject<Troy2MoveEffect>();
        else if (equal(name, FreqColorsEffect::name())) node = allocMBObject<FreqColorsEffect>();
        else if (equal(name, WowiMoveEffect::name())) node = allocMBObject<WowiMoveEffect>();
        else if (equal(name, AmbientMoveEffect::name())) node = allocMBObject<AmbientMoveEffect>();

        else if (equal(name, CheckerboardModifier::name())) node = allocMBObject<CheckerboardModifier>();
        else if (equal(name, CircleModifier::name())) node = allocMBObject<CircleModifier>();
        else if (equal(name, MirrorModifier::name())) node = allocMBObject<MirrorModifier>();
        else if (equal(name, MultiplyModifier::name())) node = allocMBObject<MultiplyModifier>();
        else if (equal(name, RippleYZModifier::name())) node = allocMBObject<RippleYZModifier>();
        else if (equal(name, PinwheelModifier::name())) node = allocMBObject<PinwheelModifier>();
        else if (equal(name, RotateNodifier::name())) node = allocMBObject<RotateNodifier>();
        else if (equal(name, TransposeModifier::name())) node = allocMBObject<TransposeModifier>();
        
        #if FT_LIVESCRIPT
            else {
                LiveScriptNode *liveScriptNode = allocMBObject<LiveScriptNode>();
                liveScriptNode->animation = name; //set the (file)name of the script
                node = liveScriptNode;
            }
        #endif

        if (node) {
            MB_LOGD(ML_TAG, "%s (p:%p pr:%d)", name, node, isInPSRAM(node));

            node->constructor(layerP.layerV[0], controls); //pass the layer to the node
            node->setup(); //run the setup of the effect
            node->onSizeChanged(Coord3D());
            // layerV[0]->nodes.reserve(index+1);
            if (index >= layerP.layerV[0]->nodes.size())
                layerP.layerV[0]->nodes.push_back(node);
            else
                layerP.layerV[0]->nodes[index] = node; //add the node to the layer
        }

        return node;
    }

    //run effects
    void loop() {

        NodeManager::loop();

    }
  
}; // class ModuleEffects

#endif
#endif