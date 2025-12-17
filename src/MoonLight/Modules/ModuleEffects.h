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
  // #include "MoonBase/Module.h"
  #include "MoonBase/NodeManager.h"

// #include "MoonBase/Nodes.h" //Nodes.h will include VirtualLayer.h which will include PhysicalLayer.h

class ModuleEffects : public NodeManager {
 public:
  ModuleEffects(PsychicHttpServer* server, ESP32SvelteKit* sveltekit, FileManager* fileManager) : NodeManager("effects", server, sveltekit, fileManager) { EXT_LOGV(ML_TAG, "constructor"); }

  void begin() override {
    defaultNodeName = getNameAndTags<RandomEffect>();
    nodes = &(layerP.layers[0]->nodes);  // to do add nodes from all layers...
    NodeManager::begin();

  #if FT_ENABLED(FT_MONITOR)
    _socket->registerEvent("monitor");
    _server->on("/rest/monitorLayout", HTTP_GET, [&](PsychicRequest* request) {
      EXT_LOGV(ML_TAG, "rest monitor triggered");

      // trigger pass 1 mapping of layout
      layerP.pass = 1;  //(requestMapPhysical=1 physical rerun)
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

  void setupDefinition(const JsonArray& controls) override {
    EXT_LOGV(ML_TAG, "");
    JsonObject control;  // state.data has one or more properties
    control = addControl(controls, "layer", "select");
    control["default"] = 0;  // the first entry has index 0 and refers to Layer 1 (layer counting starts with 1)
    uint8_t i = 1;           // start with 1
    for (VirtualLayer* layer : layerP.layers) {
      addControlValue(control, i);
      i++;
    }
    addControl(controls, "start", "coord3D", 0, UINT16_MAX, true);
    addControl(controls, "end", "coord3D", 0, UINT16_MAX, true);
    addControl(controls, "brightness", "slider", 0, UINT8_MAX, true);

    NodeManager::setupDefinition(controls);
  }

  void addNodes(const JsonObject& control) override {
    // keep the order the same as in https://moonmodules.org/MoonLight/moonlight/effects

    // MoonLight effects, Solid first then alphabetically
    addControlValue(control, getNameAndTags<SolidEffect>());
    addControlValue(control, getNameAndTags<AudioRingsEffect>());
    addControlValue(control, getNameAndTags<LinesEffect>());
    addControlValue(control, getNameAndTags<FireEffect>());
    addControlValue(control, getNameAndTags<FixedRectangleEffect>());
    addControlValue(control, getNameAndTags<ParticlesEffect>());
    addControlValue(control, getNameAndTags<PraxisEffect>());
  #if USE_M5UNIFIED
    addControlValue(control, getNameAndTags<MoonManEffect>());
  #endif
    addControlValue(control, getNameAndTags<FreqSawsEffect>());
    addControlValue(control, getNameAndTags<MarioTestEffect>());
    addControlValue(control, getNameAndTags<PixelMapEffect>());
    addControlValue(control, getNameAndTags<RandomEffect>());
    addControlValue(control, getNameAndTags<RingRandomFlowEffect>());
    addControlValue(control, getNameAndTags<RipplesEffect>());
    addControlValue(control, getNameAndTags<RubiksCubeEffect>());
    addControlValue(control, getNameAndTags<ScrollingTextEffect>());
    addControlValue(control, getNameAndTags<SinusEffect>());
    addControlValue(control, getNameAndTags<SphereMoveEffect>());
    addControlValue(control, getNameAndTags<SpiralFireEffect>());
    addControlValue(control, getNameAndTags<StarFieldEffect>());
    addControlValue(control, getNameAndTags<VUMeterEffect>());
    addControlValue(control, getNameAndTags<WaveEffect>());

    // MoonModules effects, alphabetically
    addControlValue(control, getNameAndTags<GameOfLifeEffect>());
    addControlValue(control, getNameAndTags<GEQ3DEffect>());
    addControlValue(control, getNameAndTags<PaintBrushEffect>());

    // WLED effects, alphabetically
    addControlValue(control, getNameAndTags<BlackholeEffect>());
    addControlValue(control, getNameAndTags<BouncingBallsEffect>());
    addControlValue(control, getNameAndTags<BlurzEffect>());
    addControlValue(control, getNameAndTags<DistortionWavesEffect>());
    addControlValue(control, getNameAndTags<DJLightEffect>());
    addControlValue(control, getNameAndTags<DNAEffect>());
    addControlValue(control, getNameAndTags<DripEffect>());
    addControlValue(control, getNameAndTags<FreqMatrixEffect>());
    addControlValue(control, getNameAndTags<FireworksEffect>());
    addControlValue(control, getNameAndTags<FlowEffect>());
    addControlValue(control, getNameAndTags<FrizzlesEffect>());
    addControlValue(control, getNameAndTags<FunkyPlankEffect>());
    addControlValue(control, getNameAndTags<GEQEffect>());
    addControlValue(control, getNameAndTags<HeartBeatEffect>());
    addControlValue(control, getNameAndTags<LissajousEffect>());
    addControlValue(control, getNameAndTags<Noise2DEffect>());
    addControlValue(control, getNameAndTags<NoiseMeterEffect>());
    addControlValue(control, getNameAndTags<OctopusEffect>());
    addControlValue(control, getNameAndTags<PopCornEffect>());
    addControlValue(control, getNameAndTags<RainEffect>());
    addControlValue(control, getNameAndTags<WaverlyEffect>());

    // FastLED effects
    addControlValue(control, getNameAndTags<RainbowEffect>());

    // Moving head effects, alphabetically
    addControlValue(control, getNameAndTags<AmbientMoveEffect>());
    addControlValue(control, getNameAndTags<FreqColorsEffect>());
    addControlValue(control, getNameAndTags<Troy1ColorEffect>());
    addControlValue(control, getNameAndTags<Troy1MoveEffect>());
    addControlValue(control, getNameAndTags<Troy2ColorEffect>());
    addControlValue(control, getNameAndTags<Troy2MoveEffect>());
    addControlValue(control, getNameAndTags<WowiMoveEffect>());

    // Modifiers, most used first
    addControlValue(control, getNameAndTags<MultiplyModifier>());
    addControlValue(control, getNameAndTags<MirrorModifier>());
    addControlValue(control, getNameAndTags<TransposeModifier>());
    addControlValue(control, getNameAndTags<CircleModifier>());
    addControlValue(control, getNameAndTags<RotateModifier>());
    addControlValue(control, getNameAndTags<CheckerboardModifier>());
    addControlValue(control, getNameAndTags<PinwheelModifier>());
    addControlValue(control, getNameAndTags<RippleYZModifier>());

    // find all the .sc files on FS
    File rootFolder = ESPFS.open("/");
    walkThroughFiles(rootFolder, [&](File folder, File file) {
      if (strstr(file.name(), ".sc")) {
        // EXT_LOGV(ML_TAG, "found file %s", file.path());
        addControlValue(control, (char*)file.path());
      }
    });
    rootFolder.close();
  }

  Node* addNode(const uint8_t index, const char* name, const JsonArray& controls) const override {
    Node* node = nullptr;

    // MoonLight effects, Solid first then alphabetically
    if (equalAZaz09(name, SolidEffect::name()))
      node = allocMBObject<SolidEffect>();
    else if (equalAZaz09(name, AudioRingsEffect::name()))
      node = allocMBObject<AudioRingsEffect>();
    else if (equalAZaz09(name, FireEffect::name()))
      node = allocMBObject<FireEffect>();
    else if (equalAZaz09(name, FixedRectangleEffect::name()))
      node = allocMBObject<FixedRectangleEffect>();
    else if (equalAZaz09(name, FreqSawsEffect::name()))
      node = allocMBObject<FreqSawsEffect>();
    else if (equalAZaz09(name, LinesEffect::name()))
      node = allocMBObject<LinesEffect>();
    else if (equalAZaz09(name, MarioTestEffect::name()))
      node = allocMBObject<MarioTestEffect>();
  #if USE_M5UNIFIED
    else if (equalAZaz09(name, MoonManEffect::name()))
      node = allocMBObject<MoonManEffect>();
  #endif
    else if (equalAZaz09(name, ParticlesEffect::name()))
      node = allocMBObject<ParticlesEffect>();
    else if (equalAZaz09(name, PraxisEffect::name()))
      node = allocMBObject<PraxisEffect>();
    else if (equalAZaz09(name, PixelMapEffect::name()))
      node = allocMBObject<PixelMapEffect>();
    else if (equalAZaz09(name, RandomEffect::name()))
      node = allocMBObject<RandomEffect>();
    else if (equalAZaz09(name, RingRandomFlowEffect::name()))
      node = allocMBObject<RingRandomFlowEffect>();
    else if (equalAZaz09(name, RipplesEffect::name()))
      node = allocMBObject<RipplesEffect>();
    else if (equalAZaz09(name, RubiksCubeEffect::name()))
      node = allocMBObject<RubiksCubeEffect>();
    else if (equalAZaz09(name, ScrollingTextEffect::name()))
      node = allocMBObject<ScrollingTextEffect>();
    else if (equalAZaz09(name, SinusEffect::name()))
      node = allocMBObject<SinusEffect>();
    else if (equalAZaz09(name, SphereMoveEffect::name()))
      node = allocMBObject<SphereMoveEffect>();
    else if (equalAZaz09(name, StarFieldEffect::name()))
      node = allocMBObject<StarFieldEffect>();
    else if (equalAZaz09(name, WaveEffect::name()))
      node = allocMBObject<WaveEffect>();
    else if (equalAZaz09(name, SpiralFireEffect::name()))
      node = allocMBObject<SpiralFireEffect>();
    else if (equalAZaz09(name, VUMeterEffect::name()))
      node = allocMBObject<VUMeterEffect>();

    // MoonModules effects, alphabetically
    else if (equalAZaz09(name, GameOfLifeEffect::name()))
      node = allocMBObject<GameOfLifeEffect>();
    else if (equalAZaz09(name, GEQ3DEffect::name()))
      node = allocMBObject<GEQ3DEffect>();
    else if (equalAZaz09(name, PaintBrushEffect::name()))
      node = allocMBObject<PaintBrushEffect>();

    // WLED effects, alphabetically
    else if (equalAZaz09(name, BlackholeEffect::name()))
      node = allocMBObject<BlackholeEffect>();
    else if (equalAZaz09(name, BouncingBallsEffect::name()))
      node = allocMBObject<BouncingBallsEffect>();
    else if (equalAZaz09(name, BlurzEffect::name()))
      node = allocMBObject<BlurzEffect>();
    else if (equalAZaz09(name, DistortionWavesEffect::name()))
      node = allocMBObject<DistortionWavesEffect>();
    else if (equalAZaz09(name, DJLightEffect::name()))
      node = allocMBObject<DJLightEffect>();
    else if (equalAZaz09(name, DNAEffect::name()))
      node = allocMBObject<DNAEffect>();
    else if (equalAZaz09(name, DripEffect::name()))
      node = allocMBObject<DripEffect>();
    else if (equalAZaz09(name, FireworksEffect::name()))
      node = allocMBObject<FireworksEffect>();
    else if (equalAZaz09(name, FlowEffect::name()))
      node = allocMBObject<FlowEffect>();
    else if (equalAZaz09(name, FreqMatrixEffect::name()))
      node = allocMBObject<FreqMatrixEffect>();
    else if (equalAZaz09(name, FrizzlesEffect::name()))
      node = allocMBObject<FrizzlesEffect>();
    else if (equalAZaz09(name, FunkyPlankEffect::name()))
      node = allocMBObject<FunkyPlankEffect>();
    else if (equalAZaz09(name, GEQEffect::name()))
      node = allocMBObject<GEQEffect>();
    else if (equalAZaz09(name, HeartBeatEffect::name()))
      node = allocMBObject<HeartBeatEffect>();
    else if (equalAZaz09(name, LissajousEffect::name()))
      node = allocMBObject<LissajousEffect>();
    else if (equalAZaz09(name, Noise2DEffect::name()))
      node = allocMBObject<Noise2DEffect>();
    else if (equalAZaz09(name, NoiseMeterEffect::name()))
      node = allocMBObject<NoiseMeterEffect>();
    else if (equalAZaz09(name, OctopusEffect::name()))
      node = allocMBObject<OctopusEffect>();
    else if (equalAZaz09(name, PopCornEffect::name()))
      node = allocMBObject<PopCornEffect>();
    else if (equalAZaz09(name, RainEffect::name()))
      node = allocMBObject<RainEffect>();
    else if (equalAZaz09(name, WaverlyEffect::name()))
      node = allocMBObject<WaverlyEffect>();

    // FastLED
    else if (equalAZaz09(name, RainbowEffect::name()))
      node = allocMBObject<RainbowEffect>();

    // Moving head effects, alphabetically

    else if (equalAZaz09(name, AmbientMoveEffect::name()))
      node = allocMBObject<AmbientMoveEffect>();
    else if (equalAZaz09(name, FreqColorsEffect::name()))
      node = allocMBObject<FreqColorsEffect>();
    else if (equalAZaz09(name, Troy1ColorEffect::name()))
      node = allocMBObject<Troy1ColorEffect>();
    else if (equalAZaz09(name, Troy1MoveEffect::name()))
      node = allocMBObject<Troy1MoveEffect>();
    else if (equalAZaz09(name, Troy2ColorEffect::name()))
      node = allocMBObject<Troy2ColorEffect>();
    else if (equalAZaz09(name, Troy2MoveEffect::name()))
      node = allocMBObject<Troy2MoveEffect>();
    else if (equalAZaz09(name, WowiMoveEffect::name()))
      node = allocMBObject<WowiMoveEffect>();

    // Modifiers, most used first

    else if (equalAZaz09(name, MultiplyModifier::name()))
      node = allocMBObject<MultiplyModifier>();
    else if (equalAZaz09(name, MirrorModifier::name()))
      node = allocMBObject<MirrorModifier>();
    else if (equalAZaz09(name, TransposeModifier::name()))
      node = allocMBObject<TransposeModifier>();
    else if (equalAZaz09(name, CircleModifier::name()))
      node = allocMBObject<CircleModifier>();
    else if (equalAZaz09(name, RotateModifier::name()))
      node = allocMBObject<RotateModifier>();
    else if (equalAZaz09(name, CheckerboardModifier::name()))
      node = allocMBObject<CheckerboardModifier>();
    else if (equalAZaz09(name, PinwheelModifier::name()))
      node = allocMBObject<PinwheelModifier>();
    else if (equalAZaz09(name, RippleYZModifier::name()))
      node = allocMBObject<RippleYZModifier>();

  #if FT_LIVESCRIPT
    else {
      LiveScriptNode* liveScriptNode = allocMBObject<LiveScriptNode>();
      liveScriptNode->animation = name;  // set the (file)name of the script
      node = liveScriptNode;
    }
  #endif

    if (node) {
      EXT_LOGD(ML_TAG, "%s (p:%p pr:%d)", name, node, isInPSRAM(node));

      node->constructor(layerP.layers[0], controls);  // pass the layer to the node
      // node->moduleControl = _moduleLightsControl;     // to access global lights control functions if needed
      // node->moduleIO = _moduleIO;                     // to get pin allocations
      node->moduleNodes = (Module*)this;  // to request UI update
      node->setup();                      // run the setup of the effect
      node->onSizeChanged(Coord3D());
      // layers[0]->nodes.reserve(index+1);
      if (index >= layerP.layers[0]->nodes.size())
        layerP.layers[0]->nodes.push_back(node);
      else
        layerP.layers[0]->nodes[index] = node;  // add the node to the layer
    }

    return node;
  }

  // run effects
  void loop() override { NodeManager::loop(); }

};  // class ModuleEffects

#endif
#endif