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

// sizeof was 160 chars -> 80 -> 68 -> 88
struct UpdatedItem {
  Char<20> parent[2];  // 24 -> 2*16
  uint8_t index[2];    // 2*1
  Char<20> name;       // 16 -> 16
  Char<20> oldValue;   // 32 -> 16, smaller then 11 bytes mostly
  JsonVariant value;   // 8->16->4

  UpdatedItem() {
    parent[0] = "";  // will be checked in onUpdate
    parent[1] = "";
    index[0] = UINT8_MAX;
    index[1] = UINT8_MAX;
  }
};

typedef std::function<void(JsonObject data)> ReadHook;

// heap-optimization: request heap optimization review
// on boards without PSRAM, heap is only 60 KB (30KB max alloc) available, need to find out how to increase the heap
// JsonDocument* doc is used to store all definitions of fields in the modules used (about 15 modules in total). doc is a JsonArray with JsonObjects, one for each module

extern JsonDocument* gModulesDoc;  // shared document for all modules, to save RAM

class ModuleState {
 public:
  JsonObject data = JsonObject();  // isNull()

  ModuleState() {
    EXT_LOGD(MB_TAG, "ModuleState constructor");
    // std::lock_guard<std::mutex> lock(runInAppTask_mutex);
    if (!gModulesDoc) {
      EXT_LOGD(MB_TAG, "Creating doc");
      if (psramFound())
        gModulesDoc = new JsonDocument(JsonRAMAllocator::instance());  // crashed on non psram esp32-d0
      else
        gModulesDoc = new JsonDocument();
    }
    if (gModulesDoc) {
      // doc = new JsonDocument();
      data = gModulesDoc->add<JsonObject>();
      // data = doc->to<JsonObject>();
    } else {
      EXT_LOGE(MB_TAG, "Failed to create doc");
    }
  }

  ~ModuleState() {
    EXT_LOGD(MB_TAG, "ModuleState destructor");
    // delete data from doc
    // std::lock_guard<std::mutex> lock(runInAppTask_mutex);
    if (!gModulesDoc) return;
    JsonArray arr = gModulesDoc->as<JsonArray>();
    for (size_t i = 0; i < arr.size(); i++) {
      JsonObject obj = arr[i];
      if (obj == data) {  // same object (identity check)
        EXT_LOGD(MB_TAG, "Deleting data from doc");
        arr.remove(i);
        break;  // optional, if only one match
      }
    }
  }

  std::function<void(JsonArray root)> setupDefinition = nullptr;

  void setupData();

  // called from ModuleState::update and ModuleState::setupData()
  bool compareRecursive(JsonString parent, JsonVariant oldData, JsonVariant newData, UpdatedItem& updatedItem, uint8_t depth = UINT8_MAX, uint8_t index = UINT8_MAX);

  // called from ModuleState::update
  bool checkReOrderSwap(JsonString parent, JsonVariant oldData, JsonVariant newData, UpdatedItem& updatedItem, uint8_t depth = UINT8_MAX, uint8_t index = UINT8_MAX);

  std::function<void(const UpdatedItem&)> execOnUpdate = nullptr;
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

  JsonObject addControl(JsonArray root, const char* name, const char* type, int min = 0, int max = UINT8_MAX, bool ro = false, const char* desc = nullptr);

  // called in compareRecursive->execOnUpdate
  // called from compareRecursive
  void execOnUpdate(const UpdatedItem& updatedItem);
  virtual void onUpdate(const UpdatedItem& updatedItem) {};
  virtual void onReOrderSwap(uint8_t stateIndex, uint8_t newIndex) {};

  Char<20> updateOriginId;

 protected:
  EventSocket* _socket;
  void readFromFS() {             // used in ModuleEffects, for live scripts...
    _fsPersistence.readFromFS();  // overwrites the default settings in state
    // sizeof(StatefulService<ModuleState>);  // 200
    // sizeof(_server);                       // 8
    // sizeof(HttpEndpoint<ModuleState>);     // 152
    //   sizeof(EventEndpoint<ModuleState>);    // 112
    //   sizeof(WebSocketServer<ModuleState>);  // 488
    //   sizeof(FSPersistence<ModuleState>);    // 128
    //   sizeof(PsychicHttpServer*);            // 8
    //   sizeof(HttpEndpoint<APSettings>);      // 152
    //   sizeof(FSPersistence<APSettings>);     // 128
    //   sizeof(Module);                        // 1144
  }

 private:
  // heap-optimization: request heap optimization review
  // on boards without PSRAM, heap is only 60 KB (30KB max alloc) available, need to find out how to increase the heap
  // This module class is used for each module, about 15 times, 1144 bytes each (allocated in main.cpp, in global memory area) + each class allocates it's own heap

  HttpEndpoint<ModuleState> _httpEndpoint;
  EventEndpoint<ModuleState> _eventEndpoint;
  WebSocketServer<ModuleState> _webSocketServer;
  FSPersistence<ModuleState> _fsPersistence;
  PsychicHttpServer* _server;

  void updateHandler(const String& originId);
};

#endif
#endif