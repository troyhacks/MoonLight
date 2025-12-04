/**
    @title     MoonLight
    @file      ModuleDrivers.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/moonlight/drivers/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact us for more information.
**/

#ifndef ModuleDrivers_h
#define ModuleDrivers_h

#if FT_MOONLIGHT

  #include "FastLED.h"
  // #include "MoonBase/Module.h"
  #include "MoonBase/NodeManager.h"
  #include "MoonLight/Modules/ModuleLightsControl.h"

// #include "Nodes.h" //Nodes.h will include VirtualLayer.h which will include PhysicalLayer.h

class ModuleDrivers : public NodeManager {
 public:
  ModuleLightsControl* _moduleLightsControl;
  ModuleIO* _moduleIO;
  ModuleDrivers(PsychicHttpServer* server, ESP32SvelteKit* sveltekit, FileManager* fileManager, ModuleLightsControl* moduleLightsControl, ModuleIO* moduleIO) : NodeManager("drivers", server, sveltekit, fileManager) {
    _moduleLightsControl = moduleLightsControl;
    _moduleIO = moduleIO;
    EXT_LOGV(ML_TAG, "constructor");
  }

  void readPins() {
    _moduleIO->read([&](ModuleState& state) {
      // find the pins in board definitions

      memset(layerP.ledPins, UINT8_MAX, sizeof(layerP.ledPins));

      layerP.maxPower = state.data["maxPower"];
      EXT_LOGD(ML_TAG, "maxPower %d", layerP.maxPower);

      // assign pins (valid only)
      for (JsonObject pinObject : state.data["pins"].as<JsonArray>()) {
        uint8_t usage = pinObject["usage"];
        if (usage >= pin_LED_01 && usage <= pin_LED_20 && GPIO_IS_VALID_OUTPUT_GPIO(pinObject["GPIO"].as<uint8_t>())) {
          layerP.ledPins[usage - 1] = pinObject["GPIO"];
        }
      }

      // Remove all UINT8_MAX values by compacting the array
      layerP.nrOfLedPins = 0;
      for (int readPos = 0; readPos < sizeof(layerP.ledPins); readPos++) {
        if (layerP.ledPins[readPos] != UINT8_MAX) {  // only pins which have a nrOfLedPins // && layerP.ledsPerPin[layerP.nrOfLedPins] != UINT16_MAX && layerP.ledsPerPin[layerP.nrOfLedPins] != 0
          layerP.ledPins[layerP.nrOfLedPins++] = layerP.ledPins[readPos];
        }
      }

      // log pins
      for (int i = 0; i < layerP.nrOfLedPins; i++) {
        EXT_LOGD(ML_TAG, "ledPins[%d-%d] = %d (#%d)", i, layerP.nrOfLedPins, layerP.ledPins[i], layerP.ledsPerPin[i]);
      }

      layerP.requestMapPhysical = true;
      layerP.requestMapVirtual = true;
    });
  }

  void begin() override {
    defaultNodeName = getNameAndTags<PanelLayout>();

    nodes = &layerP.nodes;
    NodeManager::begin();

    _moduleIO->addUpdateHandler([&](const String& originId) { readPins(); }, false);
  }

  void addNodes(const JsonObject& control) override {
    // Layouts, Most used first
    addControlValue(control, getNameAndTags<PanelLayout>());
    addControlValue(control, getNameAndTags<PanelsLayout>());
    addControlValue(control, getNameAndTags<CubeLayout>());
    addControlValue(control, getNameAndTags<HumanSizedCubeLayout>());
    addControlValue(control, getNameAndTags<RingsLayout>());
    addControlValue(control, getNameAndTags<WheelLayout>());
    addControlValue(control, getNameAndTags<SpiralLayout>());
    addControlValue(control, getNameAndTags<SingleLineLayout>());
    addControlValue(control, getNameAndTags<SingleRowLayout>());

    // Drivers, Most used first
    addControlValue(control, getNameAndTags<ParallelLEDDriver>());
    addControlValue(control, getNameAndTags<FastLEDDriver>());
    addControlValue(control, getNameAndTags<ArtNetInDriver>());
    addControlValue(control, getNameAndTags<ArtNetOutDriver>());
    addControlValue(control, getNameAndTags<AudioSyncDriver>());
    addControlValue(control, getNameAndTags<IRDriver>());
    addControlValue(control, getNameAndTags<HUB75Driver>());

    // custom
    addControlValue(control, getNameAndTags<SE16Layout>());
  }

  Node* addNode(const uint8_t index, const char* name, const JsonArray& controls) const override {
    Node* node = nullptr;

    // Layouts, most used first
    if (equalAZaz09(name, PanelLayout::name()))
      node = allocMBObject<PanelLayout>();
    else if (equalAZaz09(name, PanelsLayout::name()))
      node = allocMBObject<PanelsLayout>();
    else if (equalAZaz09(name, CubeLayout::name()))
      node = allocMBObject<CubeLayout>();
    else if (equalAZaz09(name, RingsLayout::name()))
      node = allocMBObject<RingsLayout>();
    else if (equalAZaz09(name, WheelLayout::name()))
      node = allocMBObject<WheelLayout>();
    else if (equalAZaz09(name, SpiralLayout::name()))
      node = allocMBObject<SpiralLayout>();
    else if (equalAZaz09(name, HumanSizedCubeLayout::name()))
      node = allocMBObject<HumanSizedCubeLayout>();
    else if (equalAZaz09(name, SingleLineLayout::name()))
      node = allocMBObject<SingleLineLayout>();
    else if (equalAZaz09(name, SingleRowLayout::name()))
      node = allocMBObject<SingleRowLayout>();

    // Drivers most used first
    else if (equalAZaz09(name, ParallelLEDDriver::name()))
      node = allocMBObject<ParallelLEDDriver>();
    else if (equalAZaz09(name, FastLEDDriver::name()))
      node = allocMBObject<FastLEDDriver>();
    else if (equalAZaz09(name, ArtNetInDriver::name()))
      node = allocMBObject<ArtNetInDriver>();
    else if (equalAZaz09(name, ArtNetOutDriver::name()))
      node = allocMBObject<ArtNetOutDriver>();
    else if (equalAZaz09(name, AudioSyncDriver::name()))
      node = allocMBObject<AudioSyncDriver>();
    else if (equalAZaz09(name, IRDriver::name()))
      node = allocMBObject<IRDriver>();
    else if (equalAZaz09(name, HUB75Driver::name()))
      node = allocMBObject<HUB75Driver>();

    // custom
    else if (equalAZaz09(name, SE16Layout::name()))
      node = allocMBObject<SE16Layout>();

  #if FT_LIVESCRIPT
    else {
      LiveScriptNode* liveScriptNode = allocMBObject<LiveScriptNode>();
      liveScriptNode->animation = name;  // set the (file)name of the script
      node = liveScriptNode;
    }
  #endif

    if (node) {
      EXT_LOGD(ML_TAG, "%s (p:%p pr:%d)", name, node, isInPSRAM(node));

      node->constructor(layerP.layers[0], controls);  // pass the layer to the node (C++ constructors are not inherited, so declare it as normal functions)
      node->moduleControl = _moduleLightsControl;     // to access global lights control functions if needed
      node->moduleIO = _moduleIO;                     // to get pin allocations
      node->moduleNodes = (Module*)this;              // to request UI update
      node->setup();                                  // run the setup of the effect
      node->onSizeChanged(Coord3D());
      // layers[0]->nodes.reserve(index+1);
      if (index >= layerP.nodes.size())
        layerP.nodes.push_back(node);
      else
        layerP.nodes[index] = node;  // add the node to the layer
    }

    EXT_LOGV(ML_TAG, "%s (s:%d p:%p)", name, layerP.nodes.size(), node);

    return node;
  }

  bool initPins = false;

  void loop() override {
    // if (layerP.lights.header.isPositions == 0) //otherwise lights is used for positions etc.
    //     layerP.loop(); //run all the effects of all virtual layers (currently only one)

    NodeManager::loop();

    if (!initPins) {
      readPins();  // initially
      initPins = true;
    }
  }

};  // class ModuleDrivers

#endif
#endif