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
#include "NodeManager.h"

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
        NodeManager::begin();

        nodes = &layerP.nodes;
    }

    void addNodes(JsonArray values) override {

        values.add(PanelLayout::name());
        values.add(PanelsLayout::name());
        values.add(CubeLayout::name());
        values.add(RingsLayout::name());
        values.add(WheelLayout::name());
        values.add(HumanSizedCubeLayout::name());
        values.add(SingleLineLayout::name());
        values.add(SingleRowLayout::name());

        //custom
        values.add(SE16Layout::name());

        values.add(ArtNetDriverMod::name());
        values.add(FastLEDDriverMod::name());
        values.add(HUB75DriverMod::name());
        values.add(PhysicalDriverMod::name());
        values.add(VirtualDriverMod::name());
        values.add(AudioSyncMod::name());
    }

    Node* addNode(const uint8_t index, const char * name, const JsonArray controls) override {
        Node *node = nullptr;
        if (equal(name, SolidEffect::name())) node = new SolidEffect();
        //alphabetically from here

        else if (equal(name, PanelLayout::name())) node = new PanelLayout();
        else if (equal(name, PanelsLayout::name())) node = new PanelsLayout();
        else if (equal(name, CubeLayout::name())) node = new CubeLayout();
        else if (equal(name, RingsLayout::name())) node = new RingsLayout();
        else if (equal(name, WheelLayout::name())) node = new WheelLayout();
        else if (equal(name, HumanSizedCubeLayout::name())) node = new HumanSizedCubeLayout();
        else if (equal(name, SingleLineLayout::name())) node = new SingleLineLayout();
        else if (equal(name, SingleRowLayout::name())) node = new SingleRowLayout();

        //custom
        else if (equal(name, SE16Layout::name())) node = new SE16Layout();

        else if (equal(name, ArtNetDriverMod::name())) node = new ArtNetDriverMod();
        else if (equal(name, FastLEDDriverMod::name())) node = new FastLEDDriverMod();
        else if (equal(name, HUB75DriverMod::name())) node = new HUB75DriverMod();
        else if (equal(name, PhysicalDriverMod::name())) node = new PhysicalDriverMod();
        else if (equal(name, VirtualDriverMod::name())) node = new VirtualDriverMod();
        else if (equal(name, AudioSyncMod::name())) node = new AudioSyncMod();
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