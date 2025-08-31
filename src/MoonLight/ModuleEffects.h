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
#include "NodeManager.h"

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
                    //     String nodeName = nodeState["nodeName"];

                    //     if (updatedItem == nodeName) {
                    //         MB_LOGV(ML_TAG, "updateHandler equals current item -> livescript compile %s", updatedItem.c_str());
                    //         LiveScriptNode *liveScriptNode = (LiveScriptNode *)layerP.layerV[0]->findLiveScriptNode(nodeState["nodeName"]);
                    //         if (liveScriptNode) {
                    //             liveScriptNode->compileAndRun();

                    //             //wait until setup has been executed?

                    //             _moduleEffects->requestUIUpdate = true; //update the Effects UI
                    //         }

                    //         MB_LOGV(ML_TAG, "update due to new node %s done", nodeName.c_str());
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
        values.add(BlurzEffect::name());
        values.add(DistortionWavesEffect::name());
        values.add(FreqMatrixEffect::name());
        values.add(GameOfLifeEffect::name());
        values.add(GEQEffect::name());
        values.add(GEQ3DEffect::name());
        values.add(GEQSawEffect::name());
        values.add(LinesEffect::name());
        values.add(LissajousEffect::name());
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
        values.add(WowiColorEffect::name());
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
        if (equal(name, SolidEffect::name())) node = allocateInPSRAM<SolidEffect>();
        //alphabetically from here
        else if (equal(name, BouncingBallsEffect::name())) node = allocateInPSRAM<BouncingBallsEffect>();
        else if (equal(name, BlurzEffect::name())) node = allocateInPSRAM<BlurzEffect>();
        else if (equal(name, DistortionWavesEffect::name())) node = allocateInPSRAM<DistortionWavesEffect>();
        else if (equal(name, FreqMatrixEffect::name())) node = allocateInPSRAM<FreqMatrixEffect>();
        else if (equal(name, GameOfLifeEffect::name())) node = allocateInPSRAM<GameOfLifeEffect>();
        else if (equal(name, GEQEffect::name())) node = allocateInPSRAM<GEQEffect>();
        else if (equal(name, GEQ3DEffect::name())) node = allocateInPSRAM<GEQ3DEffect>();
        else if (equal(name, GEQSawEffect::name())) node = allocateInPSRAM<GEQSawEffect>();
        else if (equal(name, LinesEffect::name())) node = allocateInPSRAM<LinesEffect>();
        else if (equal(name, LissajousEffect::name())) node = allocateInPSRAM<LissajousEffect>();
        else if (equal(name, Noise2DEffect::name())) node = allocateInPSRAM<Noise2DEffect>();
        else if (equal(name, NoiseMeterEffect::name())) node = allocateInPSRAM<NoiseMeterEffect>();
        else if (equal(name, PaintBrushEffect::name())) node = allocateInPSRAM<PaintBrushEffect>();
        else if (equal(name, ParticlesEffect::name())) node = allocateInPSRAM<ParticlesEffect>();
        else if (equal(name, PopCornEffect::name())) node = allocateInPSRAM<PopCornEffect>();
        else if (equal(name, RainbowEffect::name())) node = allocateInPSRAM<RainbowEffect>();
        else if (equal(name, RandomEffect::name())) node = allocateInPSRAM<RandomEffect>();
        else if (equal(name, RipplesEffect::name())) node = allocateInPSRAM<RipplesEffect>();
        else if (equal(name, RubiksCubeEffect::name())) node = allocateInPSRAM<RubiksCubeEffect>();
        else if (equal(name, ScrollingTextEffect::name())) node = allocateInPSRAM<ScrollingTextEffect>();
        else if (equal(name, SinusEffect::name())) node = allocateInPSRAM<SinusEffect>();
        else if (equal(name, SphereMoveEffect::name())) node = allocateInPSRAM<SphereMoveEffect>();
        else if (equal(name, StarFieldEffect::name())) node = allocateInPSRAM<StarFieldEffect>();
        else if (equal(name, PraxisEffect::name())) node = allocateInPSRAM<PraxisEffect>();
        else if (equal(name, WaverlyEffect::name())) node = allocateInPSRAM<WaverlyEffect>();
        else if (equal(name, FixedRectangleEffect::name())) node = allocateInPSRAM<FixedRectangleEffect>();
        else if (equal(name, WaveEffect::name())) node = allocateInPSRAM<WaveEffect>();
        else if (equal(name, Troy1ColorEffect::name())) node = allocateInPSRAM<Troy1ColorEffect>();
        else if (equal(name, Troy1MoveEffect::name())) node = allocateInPSRAM<Troy1MoveEffect>();
        else if (equal(name, Troy2ColorEffect::name())) node = allocateInPSRAM<Troy2ColorEffect>();
        else if (equal(name, Troy2MoveEffect::name())) node = allocateInPSRAM<Troy2MoveEffect>();
        else if (equal(name, WowiColorEffect::name())) node = allocateInPSRAM<WowiColorEffect>();
        else if (equal(name, WowiMoveEffect::name())) node = allocateInPSRAM<WowiMoveEffect>();
        else if (equal(name, AmbientMoveEffect::name())) node = allocateInPSRAM<AmbientMoveEffect>();

        else if (equal(name, CheckerboardModifier::name())) node = allocateInPSRAM<CheckerboardModifier>();
        else if (equal(name, CircleModifier::name())) node = allocateInPSRAM<CircleModifier>();
        else if (equal(name, MirrorModifier::name())) node = allocateInPSRAM<MirrorModifier>();
        else if (equal(name, MultiplyModifier::name())) node = allocateInPSRAM<MultiplyModifier>();
        else if (equal(name, RippleYZModifier::name())) node = allocateInPSRAM<RippleYZModifier>();
        else if (equal(name, PinwheelModifier::name())) node = allocateInPSRAM<PinwheelModifier>();
        else if (equal(name, RotateNodifier::name())) node = allocateInPSRAM<RotateNodifier>();
        else if (equal(name, TransposeModifier::name())) node = allocateInPSRAM<TransposeModifier>();
        
        #if FT_LIVESCRIPT
            else {
                LiveScriptNode *liveScriptNode = allocateInPSRAM<LiveScriptNode>();
                liveScriptNode->animation = name; //set the (file)name of the script
                node = liveScriptNode;
            }
        #endif

        if (node) {
            MB_LOGD(ML_TAG, "%s (p:%p pr:%d)", name, node, isInPSRAM(node));

            node->constructor(layerP.layerV[0], controls); //pass the layer to the node
            node->setup(); //run the setup of the effect
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