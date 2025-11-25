// lib/framework/SharedFSPersistence.h
#ifndef SharedFSPersistence_h
#define SharedFSPersistence_h

#include <FS.h>
#include <StatefulService.h>

#include <map>

#include "Module.h"

class SharedFSPersistence {
 private:
  FS* _fs;
//   size_t _bufferSize;

 public:
//   SharedFSPersistence(FS* fs, size_t bufferSize = DEFAULT_BUFFER_SIZE) : _fs(fs), _bufferSize(bufferSize) {}
  SharedFSPersistence(FS* fs) : _fs(fs) {}

  // Register a module with its filesystem path
  void registerModule(Module* module) {

    // Register this module for state updates - write to FS when state changes
    module->addUpdateHandler([this, module](const String& originId) { writeToFS(String("/.config/" + module->_moduleName + ".json").c_str()); }, false);

    // Read initial state from filesystem after registration
    readFromFS(String("/.config/" + module->_moduleName + ".json").c_str());
  }

  void begin() {
    // No additional setup needed - reading happens during registerModule
  }

  // Public method to trigger read for a specific module (if needed)
  void readFromFS(const String& filePath) {
    Module* module = findModule(filePath);
    if (!module) return;

    File file = _fs->open(filePath.c_str(), "r");
    if (file) {
      JsonDocument doc;
      DeserializationError error = deserializeJson(doc, file);
      file.close();

      if (!error && doc.is<JsonObject>()) {
        JsonObject obj = doc.as<JsonObject>();
        // Use updateWithoutPropagation to avoid triggering write during read
        module->updateWithoutPropagation(obj, ModuleState::update);
      }
    }
  }

  // Public method to trigger write for a specific module (if needed)
  void writeToFS(const String& filePath) {
    Module* module = findModule(filePath);
    if (!module) return;

    File file = _fs->open(filePath.c_str(), "w");
    if (file) {
      JsonDocument doc;
      JsonObject root = doc.to<JsonObject>();
      module->read(root, ModuleState::read);
      serializeJson(doc, file);
      file.close();
    }
  }

 private:
  Module* findModule(const String& filePath) {
    for (Module* module : modules) {
      if (contains(path.c_str(), module->_moduleName.c_str())) return module;
    }
  }
};

#endif