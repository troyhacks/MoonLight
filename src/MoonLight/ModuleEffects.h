/**
    @title     MoonLight
    @file      ModuleEffects.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/moonlight/effects/
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
        values.add(getNameAndTags<SolidEffect>());
        //alphabetically per category, keep the order the same as in https://moonmodules.org/MoonLight/moonlight/effects

        // MoonLight effects
        values.add(getNameAndTags<LinesEffect>());
        values.add(getNameAndTags<FreqSawsEffect>());
        #if USE_M5UNIFIED
            values.add(getNameAndTags<MoonManEffect>());
        #endif
        values.add(getNameAndTags<ParticlesEffect>());
        values.add(getNameAndTags<RainbowEffect>());
        values.add(getNameAndTags<RandomEffect>());
        values.add(getNameAndTags<RipplesEffect>());
        values.add(getNameAndTags<RubiksCubeEffect>());
        values.add(getNameAndTags<ScrollingTextEffect>());
        values.add(getNameAndTags<SinusEffect>());
        values.add(getNameAndTags<SphereMoveEffect>());
        values.add(getNameAndTags<StarFieldEffect>());
        values.add(getNameAndTags<PraxisEffect>());
        values.add(getNameAndTags<WaveEffect>());
        values.add(getNameAndTags<FixedRectangleEffect>());

        //MoonModules effects
        values.add(getNameAndTags<GameOfLifeEffect>());
        values.add(getNameAndTags<GEQ3DEffect>());
        values.add(getNameAndTags<PaintBrushEffect>());

        //WLED effects
        values.add(getNameAndTags<BouncingBallsEffect>());
        values.add(getNameAndTags<BlurzEffect>());
        values.add(getNameAndTags<DistortionWavesEffect>());
        values.add(getNameAndTags<FreqMatrixEffect>());
        values.add(getNameAndTags<GEQEffect>());
        values.add(getNameAndTags<LissajousEffect>());
        values.add(getNameAndTags<Noise2DEffect>());
        values.add(getNameAndTags<NoiseMeterEffect>());
        values.add(getNameAndTags<PopCornEffect>());
        values.add(getNameAndTags<WaverlyEffect>());

        // Moving head effects
        values.add(getNameAndTags<Troy1ColorEffect>());
        values.add(getNameAndTags<Troy1MoveEffect>());
        values.add(getNameAndTags<Troy2ColorEffect>());
        values.add(getNameAndTags<Troy2MoveEffect>());
        values.add(getNameAndTags<FreqColorsEffect>());
        values.add(getNameAndTags<WowiMoveEffect>());
        values.add(getNameAndTags<AmbientMoveEffect>());

        //Modifiers
        values.add(getNameAndTags<MultiplyModifier>());
        values.add(getNameAndTags<MirrorModifier>());
        values.add(getNameAndTags<TransposeModifier>());
        values.add(getNameAndTags<CircleModifier>());
        values.add(getNameAndTags<RotateModifier>());
        values.add(getNameAndTags<CheckerboardModifier>());
        values.add(getNameAndTags<PinwheelModifier>());
        values.add(getNameAndTags<RippleYZModifier>());

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
        if (contains(name, SolidEffect::name())) node = allocMBObject<SolidEffect>();
        else if (contains(name, BouncingBallsEffect::name())) node = allocMBObject<BouncingBallsEffect>();
        else if (contains(name, BlurzEffect::name())) node = allocMBObject<BlurzEffect>();
        else if (contains(name, DistortionWavesEffect::name())) node = allocMBObject<DistortionWavesEffect>();
        else if (contains(name, FreqMatrixEffect::name())) node = allocMBObject<FreqMatrixEffect>();
        else if (contains(name, GameOfLifeEffect::name())) node = allocMBObject<GameOfLifeEffect>();
        else if (contains(name, GEQEffect::name())) node = allocMBObject<GEQEffect>();
        else if (contains(name, GEQ3DEffect::name())) node = allocMBObject<GEQ3DEffect>();
        else if (contains(name, FreqSawsEffect::name())) node = allocMBObject<FreqSawsEffect>();
        else if (contains(name, LinesEffect::name())) node = allocMBObject<LinesEffect>();
        else if (contains(name, LissajousEffect::name())) node = allocMBObject<LissajousEffect>();
        #if USE_M5UNIFIED
            else if (contains(name, MoonManEffect::name())) node = allocMBObject<MoonManEffect>();
        #endif
        else if (contains(name, Noise2DEffect::name())) node = allocMBObject<Noise2DEffect>();
        else if (contains(name, NoiseMeterEffect::name())) node = allocMBObject<NoiseMeterEffect>();
        else if (contains(name, PaintBrushEffect::name())) node = allocMBObject<PaintBrushEffect>();
        else if (contains(name, ParticlesEffect::name())) node = allocMBObject<ParticlesEffect>();
        else if (contains(name, PopCornEffect::name())) node = allocMBObject<PopCornEffect>();
        else if (contains(name, RainbowEffect::name())) node = allocMBObject<RainbowEffect>();
        else if (contains(name, RandomEffect::name())) node = allocMBObject<RandomEffect>();
        else if (contains(name, RipplesEffect::name())) node = allocMBObject<RipplesEffect>();
        else if (contains(name, RubiksCubeEffect::name())) node = allocMBObject<RubiksCubeEffect>();
        else if (contains(name, ScrollingTextEffect::name())) node = allocMBObject<ScrollingTextEffect>();
        else if (contains(name, SinusEffect::name())) node = allocMBObject<SinusEffect>();
        else if (contains(name, SphereMoveEffect::name())) node = allocMBObject<SphereMoveEffect>();
        else if (contains(name, StarFieldEffect::name())) node = allocMBObject<StarFieldEffect>();
        else if (contains(name, PraxisEffect::name())) node = allocMBObject<PraxisEffect>();
        else if (contains(name, WaverlyEffect::name())) node = allocMBObject<WaverlyEffect>();
        else if (contains(name, FixedRectangleEffect::name())) node = allocMBObject<FixedRectangleEffect>();
        else if (contains(name, WaveEffect::name())) node = allocMBObject<WaveEffect>();
        else if (contains(name, Troy1ColorEffect::name())) node = allocMBObject<Troy1ColorEffect>();
        else if (contains(name, Troy1MoveEffect::name())) node = allocMBObject<Troy1MoveEffect>();
        else if (contains(name, Troy2ColorEffect::name())) node = allocMBObject<Troy2ColorEffect>();
        else if (contains(name, Troy2MoveEffect::name())) node = allocMBObject<Troy2MoveEffect>();
        else if (contains(name, FreqColorsEffect::name())) node = allocMBObject<FreqColorsEffect>();
        else if (contains(name, WowiMoveEffect::name())) node = allocMBObject<WowiMoveEffect>();
        else if (contains(name, AmbientMoveEffect::name())) node = allocMBObject<AmbientMoveEffect>();

        else if (contains(name, CheckerboardModifier::name())) node = allocMBObject<CheckerboardModifier>();
        else if (contains(name, CircleModifier::name())) node = allocMBObject<CircleModifier>();
        else if (contains(name, MirrorModifier::name())) node = allocMBObject<MirrorModifier>();
        else if (contains(name, MultiplyModifier::name())) node = allocMBObject<MultiplyModifier>();
        else if (contains(name, RippleYZModifier::name())) node = allocMBObject<RippleYZModifier>();
        else if (contains(name, PinwheelModifier::name())) node = allocMBObject<PinwheelModifier>();
        else if (contains(name, RotateModifier::name())) node = allocMBObject<RotateModifier>();
        else if (contains(name, TransposeModifier::name())) node = allocMBObject<TransposeModifier>();
        
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