/**
    @title     MoonBase
    @file      Module.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/develop/modules/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact us for more information.
**/

#ifndef Module_h
#define Module_h

#if FT_MOONBASE == 1

  #include <ESP32SvelteKit.h>
  #include <EventEndpoint.h>
  #include <EventSocket.h>
  #include <FSPersistence.h>
  #include <HttpEndpoint.h>
  #include <PsychicHttp.h>
  #include <WebSocketServer.h>

  #include "Utilities.h"

// sizeof was 160 chars -> 80 -> 68
struct UpdatedItem {
  String parent[2];   // 24 -> 32
  uint8_t index[2];   // 2x1 = 2
  String name;        // 16 -> 16
  String oldValue;    // 32 -> 16, smaller then 11 bytes mostly
  JsonVariant value;  // 8->16->4

  UpdatedItem() {
    parent[0] = "";  // will be checked in onUpdate
    parent[1] = "";
    index[0] = UINT8_MAX;
    index[1] = UINT8_MAX;
  }
};

typedef std::function<void(JsonObject data)> ReadHook;

static JsonDocument* doc = nullptr;  // shared document for all modules, to save RAM

class ModuleState {
 public:
  JsonObject data;

  ModuleState() {
    MB_LOGD(MB_TAG, "ModuleState constructor");
    if (!doc) {
      MB_LOGD(MB_TAG, "Creating doc");
      if (psramFound())
        doc = new JsonDocument(JsonRAMAllocator::instance());  // crashed on non psram esp32-d0
      else
        doc = new JsonDocument();
    }
    if (doc) {
      // doc = new JsonDocument();
      data = doc->add<JsonObject>();
      // data = doc->to<JsonObject>();
    } else {
      MB_LOGE(MB_TAG, "Failed to create doc");
    }
  }
  ~ModuleState() {
    MB_LOGD(MB_TAG, "ModuleState destructor");
    // delete data from doc
    JsonArray arr = doc->as<JsonArray>();
    for (size_t i = 0; i < arr.size(); i++) {
      JsonObject obj = arr[i];
      if (obj == data) {  // same object (identity check)
        MB_LOGD(MB_TAG, "Deleting data from doc");
        arr.remove(i);
        break;  // optional, if only one match
      }
    }
  }

  uint8_t onUpdateRunInTask = UINT8_MAX;  // if set to UINT8_MAX, runInTask1 is not called, otherwise it is called with this value as index

  std::function<void(JsonArray root)> setupDefinition = nullptr;

  void setupData();

  // called from ModuleState::update.checkReOrderSwap
  void execOnReOrderSwap(uint8_t stateIndex, uint8_t newIndex);

  // called from compareRecursive
  void execOnUpdate(UpdatedItem& updatedItem);

  // called from ModuleState::update and ModuleState::setupData()
  bool compareRecursive(JsonString parent, JsonVariant oldData, JsonVariant newData, UpdatedItem& updatedItem, uint8_t depth = UINT8_MAX, uint8_t index = UINT8_MAX);

  // called from ModuleState::update
  bool checkReOrderSwap(JsonString parent, JsonVariant oldData, JsonVariant newData, UpdatedItem& updatedItem, uint8_t depth = UINT8_MAX, uint8_t index = UINT8_MAX);

  std::function<void(UpdatedItem&)> onUpdate = nullptr;
  std::function<void(uint8_t, uint8_t)> onReOrderSwap = nullptr;

  static void read(ModuleState& state, JsonObject& root);
  static StateUpdateResult update(JsonObject& root, ModuleState& state);

  ReadHook readHook = nullptr;  // called when the UI requests the state, can be used to update the state before sending it to the UI
};

class Module : public StatefulService<ModuleState> {
 public:
  String _moduleName = "";

  Module(String moduleName, PsychicHttpServer* server, ESP32SvelteKit* sveltekit);

  void begin();

  virtual void setupDefinition(JsonArray root);

  // called in compareRecursive->execOnUpdate
  virtual void onUpdate(UpdatedItem& updatedItem);
  virtual void onReOrderSwap(uint8_t stateIndex, uint8_t newIndex);

 protected:
  EventSocket* _socket;
  void readFromFS() {             // used in ModuleEffects, for live scripts...
    _fsPersistence.readFromFS();  // overwrites the default settings in state
    //  sizeof(StatefulService<ModuleState>);
    //  sizeof(_httpEndpoint);
    //  sizeof(_eventEndpoint);
    //  sizeof(_webSocketServer);
    //  sizeof(_fsPersistence);
    //  sizeof(_server);
  }

 private:
  HttpEndpoint<ModuleState> _httpEndpoint;
  EventEndpoint<ModuleState> _eventEndpoint;
  WebSocketServer<ModuleState> _webSocketServer;
  FSPersistence<ModuleState> _fsPersistence;
  PsychicHttpServer* _server;

  void updateHandler();
};

#endif
#endif