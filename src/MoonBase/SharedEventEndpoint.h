// src/MoonBase/SharedEventEndpoint.h
#ifndef SharedEventEndpoint_h
#define SharedEventEndpoint_h

#include <EventSocket.h>
#include <StatefulService.h>

#include "Module.h"

class SharedEventEndpoint {
 private:
  EventSocket* _socket;

 public:
  SharedEventEndpoint(EventSocket* socket) : _socket(socket) {}

  void registerModule(Module* module) {
    const char* eventName = module->_moduleName.c_str();

    // Register the event with the socket
    _socket->registerEvent(eventName);

    // ADDED: Register handler for INCOMING events (client -> server updates)
    _socket->onEvent(eventName, [this, module](JsonObject& root, int originId) { module->update(root, ModuleState::update, String(originId)); });

    // ADDED: Register handler for new subscriptions (send state when client subscribes)
    _socket->onSubscribe(eventName, [this, module](const String& originId) { syncState(module, originId, true); });

    // Register this module for state updates (server -> clients)
    module->addUpdateHandler([this, module](const String& originId) { syncState(module, originId, false); }, false);
  }

  void begin() {
    // All events are registered during registerModule
  }

 private:
  void syncState(Module* module, const String& originId, bool sync = false) {
    // ADDED: Check if there are connected clients before creating JSON
    if (!_socket->getConnectedClients()) return;

    JsonDocument doc;
    JsonObject root = doc.to<JsonObject>();
    module->read(root, ModuleState::read);

    // CHANGED: Use JsonObject overload, not buffer
    _socket->emitEvent(module->_moduleName.c_str(), root, originId.c_str(), sync);
  }
};

#endif