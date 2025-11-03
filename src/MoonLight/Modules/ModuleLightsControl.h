/**
    @title     MoonLight
    @file      ModuleLightsControl.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/moonlight/lightscontrol/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact us for more information.
**/

#ifndef ModuleLightsControl_h
#define ModuleLightsControl_h

#if FT_MOONLIGHT

  #include "FastLED.h"
  #include "ModuleEffects.h"
  #include "MoonBase/Module.h"
  #include "MoonBase/Utilities.h"  //for isInPSRAM
  #include "MoonBase/Modules/FileManager.h"

class ModuleLightsControl : public Module {
 public:
  PsychicHttpServer* _server;
  FileManager* _fileManager;

  ModuleLightsControl(PsychicHttpServer* server, ESP32SvelteKit* sveltekit, FileManager* fileManager) : Module("lightscontrol", server, sveltekit) {
    EXT_LOGV(ML_TAG, "constructor");
    _server = server;
    _fileManager = fileManager;
  }

  void begin() {
    Module::begin();

    EXT_LOGI(ML_TAG, "Lights:%d(Header:%d) L-H:%d Node:%d PL:%d(PL-L:%d) VL:%d PM:%d C3D:%d", sizeof(Lights), sizeof(LightsHeader), sizeof(Lights) - sizeof(LightsHeader), sizeof(Node),
            sizeof(PhysicalLayer), sizeof(PhysicalLayer) - sizeof(Lights), sizeof(VirtualLayer), sizeof(PhysMap), sizeof(Coord3D));

    EXT_LOGI(ML_TAG, "isInPSRAM: mt:%d mti:%d ch:%d", isInPSRAM(&layerP.layers[0]->mappingTable), isInPSRAM(&layerP.layers[0]->mappingTableIndexes), isInPSRAM(layerP.lights.channels));

    setPresetsFromFolder();  // set the right values during boot

    // update presets if files changed in presets folder
    _fileManager->addUpdateHandler([&](const String& originId) {
      EXT_LOGV(ML_TAG, "FileManager::updateHandler %s", originId.c_str());
      // read the file state (read all files and folders on FS and collect changes)
      _fileManager->read([&](FilesState& filesState) {
        // loop over all changed files (normally only one)
        bool presetChanged = false;
        for (auto updatedItem : filesState.updatedItems) {
          // if file is the current live script, recompile it (to do: multiple live effects)
          EXT_LOGV(ML_TAG, "updateHandler updatedItem %s", updatedItem.c_str());
          if (strstr(updatedItem.c_str(), "/.config/presets")) {
            EXT_LOGV(ML_TAG, " preset.json updated -> call update %s", updatedItem.c_str());
            presetChanged = true;
          }
        }
        if (presetChanged) {
          EXT_LOGV(ML_TAG, "setPresetsFromFolder");
          setPresetsFromFolder();  // update the presets from the folder
        }
      });
    });
  }

  // define the data model
  void setupDefinition(JsonArray root) override {
    EXT_LOGV(ML_TAG, "");
    JsonObject property;       // state.data has one or more properties
    JsonArray details = root;  // if a property is an array, this is the details of the array
    JsonArray values;          // if a property is a select, this is the values of the select

    property = root.add<JsonObject>();
    property["name"] = "lightsOn";
    property["type"] = "checkbox";
    property["default"] = true;
    property = root.add<JsonObject>();
    property["name"] = "brightness";
    property["type"] = "slider";
    property["default"] = 10;
    property = root.add<JsonObject>();
    property["name"] = "red";
    property["type"] = "slider";
    property["default"] = 255;
    property["color"] = "Red";
    property = root.add<JsonObject>();
    property["name"] = "green";
    property["type"] = "slider";
    property["default"] = 255;
    property["color"] = "Green";
    property = root.add<JsonObject>();
    property["name"] = "blue";
    property["type"] = "slider";
    property["default"] = 255;
    property["color"] = "Blue";
    property = root.add<JsonObject>();
    property["name"] = "palette";
    property["type"] = "select";
    property["default"] = 6;
    values = property["values"].to<JsonArray>();
    values.add("CloudColors");
    values.add("LavaColors");
    values.add("OceanColors");
    values.add("ForestColors");
    values.add("RainbowColors");
    values.add("RainbowStripeColors");
    values.add("PartyColors");
    values.add("HeatColors");
    values.add("RandomColors");
    property = root.add<JsonObject>();
    property["name"] = "preset";
    property["type"] = "pad";
    property["width"] = 8;
    property["size"] = 18;
    property["default"].to<JsonObject>();  // clear the preset array before adding new presets
    property["default"]["list"].to<JsonArray>();
    property["default"]["count"] = 64;

    property = root.add<JsonObject>();
    property["name"] = "presetLoop";
    property["type"] = "slider";
    property["default"] = 0;
    property = root.add<JsonObject>();
    property["name"] = "firstPreset";
    property["type"] = "slider";
    property["min"] = 0;
    property["max"] = 63;
    property["default"] = 0;
    property = root.add<JsonObject>();
    property["name"] = "lastPreset";
    property["type"] = "slider";
    property["min"] = 0;
    property["max"] = 63;
    property["default"] = 63;

  #if FT_ENABLED(FT_MONITOR)
    property = root.add<JsonObject>();
    property["name"] = "monitorOn";
    property["type"] = "checkbox";
    property["default"] = true;
  #endif
  }

  // implement business logic
  void onUpdate(UpdatedItem& updatedItem) override {
    // EXT_LOGD(ML_TAG, "handle %s[%d]%s[%d].%s = %s -> %s", updatedItem.parent[0].c_str(), updatedItem.index[0], updatedItem.parent[1].c_str(), updatedItem.index[1], updatedItem.name.c_str(),
    // updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
    if (updatedItem.name == "red") {
      layerP.lights.header.red = _state.data["red"];
    } else if (updatedItem.name == "green") {
      layerP.lights.header.green = _state.data["green"];
    } else if (updatedItem.name == "blue") {
      layerP.lights.header.blue = _state.data["blue"];
    } else if (updatedItem.name == "lightsOn" || updatedItem.name == "brightness") {
      layerP.lights.header.brightness = _state.data["lightsOn"] ? _state.data["brightness"] : 0;
    } else if (updatedItem.name == "palette") {
      // String value = _state.data["palette"];//updatedItem.oldValue;
      if (updatedItem.value == 0)
        layerP.palette = CloudColors_p;
      else if (updatedItem.value == 1)
        layerP.palette = LavaColors_p;
      else if (updatedItem.value == 2)
        layerP.palette = OceanColors_p;
      else if (updatedItem.value == 3)
        layerP.palette = ForestColors_p;
      else if (updatedItem.value == 4)
        layerP.palette = RainbowColors_p;
      else if (updatedItem.value == 5)
        layerP.palette = RainbowStripeColors_p;
      else if (updatedItem.value == 6)
        layerP.palette = PartyColors_p;
      else if (updatedItem.value == 7)
        layerP.palette = HeatColors_p;
      else if (updatedItem.value == 8) {
        for (int i = 0; i < sizeof(layerP.palette.entries) / sizeof(CRGB); i++) {
          layerP.palette[i] = CHSV(random8(), 255, 255);  // take the max saturation, max brightness of the colorwheel
        }
      } else {
        layerP.palette = PartyColors_p;  // should never occur
      }
      // layerP.palette = LavaColors_p;
    } else if (updatedItem.name == "preset") {
      // copy /.config/effects.json to the hidden folder /.config/presets/preset[x].json
      // do not set preset at boot...
      if (updatedItem.oldValue != "null" && !updatedItem.value["action"].isNull()) {
        uint16_t select = updatedItem.value["select"];
        Char<32> presetFile;
        presetFile.format("/.config/presets/preset%02d.json", select);

        if (updatedItem.value["action"] == "click") {
          updatedItem.value["selected"] = select;  // store the selected preset
          if (arrayContainsValue(updatedItem.value["list"], select)) {
            copyFile(presetFile.c_str(), "/.config/effects.json");

            // trigger notification of update of effects.json
            _fileManager->update(
                [&](FilesState& state) {
                  state.updatedItems.push_back("/.config/effects.json");
                  return StateUpdateResult::CHANGED;  // notify StatefulService by returning CHANGED
                },
                _moduleName);

          } else {
            copyFile("/.config/effects.json", presetFile.c_str());
            setPresetsFromFolder();  // update presets in UI
          }
        } else if (updatedItem.value["action"] == "dblclick") {
          ESPFS.remove(presetFile.c_str());
          setPresetsFromFolder();  // update presets in UI
        }
      }
    }
  }

  // update _state.data["preset"]["list"] and send update to endpoints
  void setPresetsFromFolder() {
    // loop over all files in the presets folder and add them to the preset array
    File rootFolder = ESPFS.open("/.config/presets/");
    _state.data["preset"]["list"].clear();  //.to<JsonArray>(); // clear the active preset array before adding new presets
    bool changed = false;
    walkThroughFiles(rootFolder, [&](File folder, File file) {
      int seq = -1;
      if (sscanf(file.name(), "preset%02d.json", &seq) == 1) {
        // seq now contains the 2-digit number, e.g., 34
        EXT_LOGV(ML_TAG, "Preset %d found", seq);
        _state.data["preset"]["list"].add(seq);  // add the preset to the preset array
        changed = true;
      }
    });

    if (changed) {
      // requestUIUpdate ...
      update(
          [&](ModuleState& state) {
            return StateUpdateResult::CHANGED;  // notify StatefulService by returning CHANGED
          },
          _moduleName);
    }
  }

  uint8_t lastPresetLooped = 0;
  unsigned long lastPresetTime = 0;

  void loop() {
    // process presetLoop
    uint8_t presetLoop = _state.data["presetLoop"];
    if (presetLoop && millis() - lastPresetTime > presetLoop * 1000) {  // every presetLoop seconds
      lastPresetTime = millis();

      // bugfix;
      //  std::lock_guard<std::mutex> lock(runInTask_mutex);
      //  runInTask1.push_back([&]() mutable { //mutable as updatedItem is called by reference (&)
      //  load the xth preset from FS
      JsonArray presets = _state.data["preset"]["list"];

      if (_state.data["firstPreset"] <= _state.data["lastPreset"]) {
        // find the next preset within the range
        while (presets[lastPresetLooped % presets.size()] < _state.data["firstPreset"] || presets[lastPresetLooped % presets.size()] > _state.data["lastPreset"]) lastPresetLooped++;

        // correct overflow
        lastPresetLooped = lastPresetLooped % presets.size();

        Char<32> presetFile;
        presetFile.format("/.config/presets/preset%02d.json", presets[lastPresetLooped].as<uint8_t>());

        EXT_LOGD(ML_TAG, "loading next preset %d %s", lastPresetLooped, presetFile.c_str());

        copyFile(presetFile.c_str(), "/.config/effects.json");

        _state.data["preset"]["selected"] = presets[lastPresetLooped];

        // update UI
        _socket->emitEvent(_moduleName, _state.data);

        // trigger file manager notification of update of effects.json
        _fileManager->update(
            [&](FilesState& state) {
              state.updatedItems.push_back("/.config/effects.json");
              // EXT_LOGD(ML_TAG, "   preset files %d %s", lastPresetLooped, presetFile.c_str());
              return StateUpdateResult::CHANGED;  // notify StatefulService by returning CHANGED
            },
            _moduleName);

        lastPresetLooped++;  // next
      }
      // });
    }

  #if FT_ENABLED(FT_MONITOR)
    if (layerP.lights.header.isPositions == 2) {  // send to UI
      read([&](ModuleState& _state) {
        if (_socket->getConnectedClients() && _state.data["monitorOn"]) {
          _socket->emitEvent("monitor", (char*)&layerP.lights.header, 37);  // sizeof(LightsHeader)); //sizeof(LightsHeader), nearest prime nr above 32 to avoid monitor data to be seen as header
          _socket->emitEvent("monitor", (char*)layerP.lights.channels, MIN(layerP.lights.header.nrOfLights * 3, layerP.lights.maxChannels));  //*3 is for 3 bytes position
        }
        memset(layerP.lights.channels, 0, layerP.lights.maxChannels);  // set all the channels to 0 //cleaning the positions
        EXT_LOGD(ML_TAG, "positions sent to monitor (2 -> 3, noL:%d noC:%d)", layerP.lights.header.nrOfLights, layerP.lights.maxChannels);
        layerP.lights.header.isPositions = 3;
      });
    } else if (layerP.lights.header.isPositions == 0 && layerP.lights.header.nrOfLights) {  // send to UI
      EVERY_N_MILLIS(layerP.lights.header.nrOfLights / 12) {
        read([&](ModuleState& _state) {
          if (_socket->getConnectedClients() && _state.data["monitorOn"])
            _socket->emitEvent("monitor", (char*)layerP.lights.channels, MIN(layerP.lights.header.nrOfLights * layerP.lights.header.channelsPerLight, layerP.lights.maxChannels));
        });
      }
    }
  #endif
  }

};  // class ModuleLightsControl

#endif
#endif