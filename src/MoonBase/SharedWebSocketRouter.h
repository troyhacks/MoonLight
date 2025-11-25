// In lib/framework/SharedWebSocketRouter.h
#ifndef SharedWebSocketRouter_h
#define SharedWebSocketRouter_h

#include <PsychicHttp.h>

#include <map>
#include <set>

#include "Module.h"

class SharedWebSocketRouter {
 private:
//   std::map<String, Module*> _modulesByPath;  // "/ws/moduleName" -> Module*
  std::set<int> _initializedSockets;         // Sockets that have received initial data
  PsychicWebSocketHandler _handler;
  PsychicHttpServer* _server;
  SecurityManager* _securityManager;

 public:
  SharedWebSocketRouter(PsychicHttpServer* server, SecurityManager* securityManager) : _server(server), _securityManager(securityManager) {}

  void registerModule(Module* module) {
    EXT_LOGD(ML_TAG, "%s", module->_moduleName.c_str());
    // _modulesByPath[path] = module;
    // Register this module for state updates
    module->addUpdateHandler([this, module](const String& originId) { transmitData(String("/rest/" + module->_moduleName).c_str(), nullptr, originId); }, false);
  }

  void begin() {
    _handler.setFilter(_securityManager->filterRequest(AuthenticationPredicates::IS_ADMIN));

    _handler.onOpen([this](PsychicWebSocketClient* client) {
      // Just note that this socket is new (not initialized yet)
      // Don't add to _initializedSockets - let onFrame handle it
    });

    _handler.onFrame([this](PsychicWebSocketRequest* request, httpd_ws_frame* frame) {
      int socket = request->client()->socket();

      // Check if this is the first frame for this socket
      if (_initializedSockets.find(socket) == _initializedSockets.end()) {
        _initializedSockets.insert(socket);

        // Send initial state
        EXT_LOGD(ML_TAG, "search module %s", request->path().c_str());
        Module* module = findModule(request->url());
        if (module) {
          JsonDocument doc;
          JsonObject root = doc.to<JsonObject>();
          module->read(root, ModuleState::read);
          String buffer;
          serializeJson(doc, buffer);
          request->client()->sendMessage(buffer.c_str());
        }
      }

      // Handle incoming frame data
      if (frame->type == HTTPD_WS_TYPE_TEXT) {
        EXT_LOGD(ML_TAG, "search module %s", request->url());
        Module* module = findModule(request->url());
        if (module) {
          JsonDocument doc;
          DeserializationError error = deserializeJson(doc, (char*)frame->payload, frame->len);
          if (!error && doc.is<JsonObject>()) {
            JsonObject obj = doc.as<JsonObject>();
            module->update(obj, ModuleState::update, "wsserver");
          }
        }
      }
      return ESP_OK;
    });

    _handler.onClose([this](PsychicWebSocketClient* client) {
      _initializedSockets.erase(client->socket());  // Clean up
    });

    _server->on("/ws/*", &_handler);
  }

 private:
  void transmitData(const String& path, PsychicWebSocketClient* client, const String& originId) {
    Module* module = findModule(path);
    if (!module) return;

    JsonDocument doc;
    JsonObject root = doc.to<JsonObject>();
    module->read(root, ModuleState::read);
    String buffer;
    serializeJson(doc, buffer);

    if (client) {
      client->sendMessage(buffer.c_str());
    } else {
      _handler.sendAll(buffer.c_str());
    }
  }

  Module* findModule(const String& path) {
    for (Module *module:modules){
        if (contains(path.c_str(), module->_moduleName.c_str()))
            return module;
    }
    return nullptr;
    // auto it = _modulesByPath.find(path);
    // return (it != _modulesByPath.end()) ? it->second : nullptr;
  }
};

#endif