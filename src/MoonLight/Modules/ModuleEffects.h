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

  void setupDefinition(const JsonArray& root) override {
    EXT_LOGV(ML_TAG, "");
    JsonObject property;  // state.data has one or more properties
    JsonArray values;     // if a property is a select, this is the values of the select
    property = root.add<JsonObject>();
    property["name"] = "layer";
    property["type"] = "select";
    property["default"] = 0;
    values = property["values"].to<JsonArray>();
    uint8_t i = 0;
    for (VirtualLayer* layer : layerP.layers) {
      values.add(i);
      i++;
    }
    property = root.add<JsonObject>();
    property["name"] = "start";
    property["type"] = "coord3D";
    property["ro"] = true;
    property = root.add<JsonObject>();
    property["name"] = "end";
    property["type"] = "coord3D";
    property["ro"] = true;
    property = root.add<JsonObject>();
    property["name"] = "brightness";
    property["type"] = "slider";
    property["ro"] = true;

    NodeManager::setupDefinition(root);
  }

  void addNodes(const JsonArray& values) const override {
    // keep the order the same as in https://moonmodules.org/MoonLight/moonlight/effects

    // MoonLight effects, Solid first then alphabetically
    values.add(getNameAndTags<SolidEffect>());
    values.add(getNameAndTags<LinesEffect>());
    values.add(getNameAndTags<FixedRectangleEffect>());
    values.add(getNameAndTags<ParticlesEffect>());
    values.add(getNameAndTags<PraxisEffect>());
  #if USE_M5UNIFIED
    values.add(getNameAndTags<MoonManEffect>());
  #endif
    values.add(getNameAndTags<FreqSawsEffect>());
    values.add(getNameAndTags<RainbowEffect>());
    values.add(getNameAndTags<RandomEffect>());
    values.add(getNameAndTags<RipplesEffect>());
    values.add(getNameAndTags<RubiksCubeEffect>());
    values.add(getNameAndTags<ScrollingTextEffect>());
    values.add(getNameAndTags<SinusEffect>());
    values.add(getNameAndTags<SphereMoveEffect>());
    values.add(getNameAndTags<StarFieldEffect>());
    values.add(getNameAndTags<WaveEffect>());
    values.add(getNameAndTags<SpiralFireEffect>());

    // MoonModules effects, alphabetically
    values.add(getNameAndTags<GameOfLifeEffect>());
    values.add(getNameAndTags<GEQ3DEffect>());
    values.add(getNameAndTags<PaintBrushEffect>());

    // WLED effects, alphabetically
    values.add(getNameAndTags<BlackholeEffect>());
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

    // Moving head effects, alphabetically
    values.add(getNameAndTags<AmbientMoveEffect>());
    values.add(getNameAndTags<FreqColorsEffect>());
    values.add(getNameAndTags<Troy1ColorEffect>());
    values.add(getNameAndTags<Troy1MoveEffect>());
    values.add(getNameAndTags<Troy2ColorEffect>());
    values.add(getNameAndTags<Troy2MoveEffect>());
    values.add(getNameAndTags<WowiMoveEffect>());

    // Modifiers, most used first
    values.add(getNameAndTags<MultiplyModifier>());
    values.add(getNameAndTags<MirrorModifier>());
    values.add(getNameAndTags<TransposeModifier>());
    values.add(getNameAndTags<CircleModifier>());
    values.add(getNameAndTags<RotateModifier>());
    values.add(getNameAndTags<CheckerboardModifier>());
    values.add(getNameAndTags<PinwheelModifier>());
    values.add(getNameAndTags<RippleYZModifier>());

    // find all the .sc files on FS
    File rootFolder = ESPFS.open("/");
    walkThroughFiles(rootFolder, [&](File folder, File file) {
      if (strstr(file.name(), ".sc")) {
        // EXT_LOGV(ML_TAG, "found file %s", file.path());
        values.add((char*)file.path());
      }
    });
    rootFolder.close();
  }

  Node* addNode(const uint8_t index, const char* name, const JsonArray& controls) const override {
    Node* node = nullptr;

    // MoonLight effects, Solid first then alphabetically
    if (equalAZaz09(name, SolidEffect::name()))
      node = allocMBObject<SolidEffect>();
    else if (equalAZaz09(name, FixedRectangleEffect::name()))
      node = allocMBObject<FixedRectangleEffect>();
    else if (equalAZaz09(name, FreqSawsEffect::name()))
      node = allocMBObject<FreqSawsEffect>();
    else if (equalAZaz09(name, LinesEffect::name()))
      node = allocMBObject<LinesEffect>();
  #if USE_M5UNIFIED
    else if (equalAZaz09(name, MoonManEffect::name()))
      node = allocMBObject<MoonManEffect>();
  #endif
    else if (equalAZaz09(name, PraxisEffect::name()))
      node = allocMBObject<PraxisEffect>();
    else if (equalAZaz09(name, RandomEffect::name()))
      node = allocMBObject<RandomEffect>();
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
    else if (equalAZaz09(name, FreqMatrixEffect::name()))
      node = allocMBObject<FreqMatrixEffect>();
    else if (equalAZaz09(name, GEQEffect::name()))
      node = allocMBObject<GEQEffect>();
    else if (equalAZaz09(name, LissajousEffect::name()))
      node = allocMBObject<LissajousEffect>();
    else if (equalAZaz09(name, Noise2DEffect::name()))
      node = allocMBObject<Noise2DEffect>();
    else if (equalAZaz09(name, NoiseMeterEffect::name()))
      node = allocMBObject<NoiseMeterEffect>();
    else if (equalAZaz09(name, ParticlesEffect::name()))
      node = allocMBObject<ParticlesEffect>();
    else if (equalAZaz09(name, PopCornEffect::name()))
      node = allocMBObject<PopCornEffect>();
    else if (equalAZaz09(name, RainbowEffect::name()))
      node = allocMBObject<RainbowEffect>();
    else if (equalAZaz09(name, WaverlyEffect::name()))
      node = allocMBObject<WaverlyEffect>();

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
      node->setup();                                  // run the setup of the effect
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