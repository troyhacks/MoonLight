/**
    @title     MoonLight
    @file      ModuleDrivers.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/moonbase/module/editor/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact us for more information.
**/

#ifndef ModuleDrivers_h
#define ModuleDrivers_h

#if FT_MOONLIGHT

#include "FastLED.h"
// #include "../MoonBase/Module.h"
#include "Nodes/NodeManager.h"

// #include "Nodes.h" //Nodes.h will include VirtualLayer.h which will include PhysicalLayer.h

class ModuleDrivers : public NodeManager
{
public:

    ModuleDrivers(PsychicHttpServer *server,
        ESP32SvelteKit *sveltekit
    ) : NodeManager("drivers", server, sveltekit) {
        MB_LOGV(ML_TAG, "constructor");
    }

    void begin() {
        _state.onUpdateRunInTask = 1; //also in effects class!, the driver only drives !!!
        defaultNodeName = PanelLayout::name();

        NodeManager::begin();

        nodes = &layerP.nodes;
    }

    void addNodes(JsonArray values) override {

        values.add(getNameAndTags<PanelLayout>());
        values.add(getNameAndTags<PanelsLayout>());
        values.add(getNameAndTags<CubeLayout>());
        values.add(getNameAndTags<HumanSizedCubeLayout>());
        values.add(getNameAndTags<RingsLayout>());
        values.add(getNameAndTags<WheelLayout>());
        values.add(getNameAndTags<SingleLineLayout>());
        values.add(getNameAndTags<SingleRowLayout>());

        //custom
        #ifdef BUILD_TARGET_ESP32_S3_STEPHANELEC_16P
            values.add(getNameAndTags<SE16Layout>());
        #endif

        values.add(getNameAndTags<PhysicalDriver>());
        values.add(getNameAndTags<FastLEDDriver>());
        values.add(getNameAndTags<ArtNetDriver>());
        values.add(getNameAndTags<AudioSyncDriver>());
        values.add(getNameAndTags<VirtualDriver>());
        values.add(getNameAndTags<HUB75Driver>());
    }

    Node* addNode(const uint8_t index, const char * name, const JsonArray controls) override {
        Node *node = nullptr;

        if (contains(name, PanelLayout::name())) node = allocMBObject<PanelLayout>();
        else if (contains(name, PanelsLayout::name())) node = allocMBObject<PanelsLayout>();
        else if (contains(name, CubeLayout::name())) node = allocMBObject<CubeLayout>();
        else if (contains(name, RingsLayout::name())) node = allocMBObject<RingsLayout>();
        else if (contains(name, WheelLayout::name())) node = allocMBObject<WheelLayout>();
        else if (contains(name, HumanSizedCubeLayout::name())) node = allocMBObject<HumanSizedCubeLayout>();
        else if (contains(name, SingleLineLayout::name())) node = allocMBObject<SingleLineLayout>();
        else if (contains(name, SingleRowLayout::name())) node = allocMBObject<SingleRowLayout>();

        //custom
        #ifdef BUILD_TARGET_ESP32_S3_STEPHANELEC_16P
            else if (contains(name, SE16Layout::name())) node = allocMBObject<SE16Layout>();
        #endif

        else if (contains(name, ArtNetDriver::name())) node = allocMBObject<ArtNetDriver>();
        else if (contains(name, FastLEDDriver::name())) node = allocMBObject<FastLEDDriver>();
        else if (contains(name, PhysicalDriver::name())) node = allocMBObject<PhysicalDriver>();
        else if (contains(name, AudioSyncDriver::name())) node = allocMBObject<AudioSyncDriver>();
        else if (contains(name, HUB75Driver::name())) node = allocMBObject<HUB75Driver>();
        else if (contains(name, VirtualDriver::name())) node = allocMBObject<VirtualDriver>();
        #if FT_LIVESCRIPT
            else {
                LiveScriptNode *liveScriptNode = allocMBObject<LiveScriptNode>();
                liveScriptNode->animation = name; //set the (file)name of the script
                node = liveScriptNode;
            }
        #endif

        if (node) {
            node->constructor(layerP.layerV[0], controls); //pass the layer to the node (C++ constructors are not inherited, so declare it as normal functions)
            node->setup(); //run the setup of the effect
            node->onSizeChanged(Coord3D());
            // layerV[0]->nodes.reserve(index+1);
            if (index >= layerP.nodes.size())
                layerP.nodes.push_back(node);
            else
                layerP.nodes[index] = node; //add the node to the layer
        }

        MB_LOGV(ML_TAG, "%s (s:%d p:%p)", name, layerP.nodes.size(), node);

        return node;
    }

    //run effects
    void loop() {
        // if (layerP.lights.header.isPositions == 0) //otherwise lights is used for positions etc.
        //     layerP.loop(); //run all the effects of all virtual layers (currently only one)

        NodeManager::loop();

    }
  
}; // class ModuleDrivers

#endif
#endif