// lib/framework/SharedEventRouter.h
#ifndef SharedEventRouter_h
#define SharedEventRouter_h

#include <EventSocket.h>
#include <StatefulService.h>

#include <map>

#include "Module.h"

class SharedEventRouter {
 private:
  EventSocket* _socket;

 public:
  SharedEventRouter(EventSocket* socket) : _socket(socket) {}

  // Register a module with its event name
  void registerModule(Module* module) {

    // Register the event with the socket
    _socket->registerEvent(module->_moduleName.c_str());

    // Register this module for state updates - transmit when state changes
    module->addUpdateHandler([this, module](const String& originId) { transmitData(module->_moduleName, nullptr, originId); }, false);
  }

  void begin() {
    // EventSocket doesn't need additional setup beyond per-event registration
    // All events are already registered during registerModule
  }

 private:
  void transmitData(const String& eventName, PsychicWebSocketClient* client, const String& originId) {
    Module* module = findModule(eventName);
    if (!module) return;

    // Create JSON document with module state
    JsonDocument doc;
    JsonObject root = doc.to<JsonObject>();
    module->read(root, ModuleState::read);

    // Measure and serialize
    size_t len = measureJson(doc);
    AsyncWebSocketMessageBuffer* buffer = _socket->makeBuffer(len);
    if (buffer) {
      serializeJson(doc, buffer->get(), len);

      if (client) {
        // Send to specific client
        client->text(buffer);
      } else {
        // Emit to all subscribers of this event
        _socket->emitEvent(eventName.c_str(), buffer->get(), len, originId.c_str());
      }
    }
  }

  Module* findModule(const String& eventName) {
    for (Module* module : modules) {
      if (contains(eventName.c_str(), module->_moduleName.c_str())) return module;
    }
  }
};

#endif