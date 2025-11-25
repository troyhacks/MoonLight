// In lib/framework/SharedWebSocketServer.h
#ifndef SharedWebSocketServer_h
#define SharedWebSocketServer_h

#include <PsychicHttp.h>

#include <map>
#include <set>

#include "Module.h"

class SharedWebSocketServer {
 private:
  PsychicWebSocketHandler _handler;
  PsychicHttpServer* _server;
  SecurityManager* _securityManager;

 public:
  SharedWebSocketServer(PsychicHttpServer* server, SecurityManager* securityManager) : _server(server), _securityManager(securityManager) {}

  void registerModule(Module* module) {
    EXT_LOGD(ML_TAG, "%s", module->_moduleName.c_str());
    // Register this module for state updates
    module->addUpdateHandler([this, module](const String& originId) { transmitData(String("/rest/" + module->_moduleName).c_str(), nullptr, originId); }, false);
  }

  void begin() {
    _handler.setFilter(_securityManager->filterRequest(AuthenticationPredicates::IS_ADMIN));

    _handler.onOpen([this](PsychicWebSocketClient* client) {
      transmitId(client);
      ESP_LOGI(SVK_TAG, "ws[%s][%u] connect", client->remoteIP().toString().c_str(), client->socket());
    });

    _handler.onFrame([this](PsychicWebSocketRequest* request, httpd_ws_frame* frame) {
      ESP_LOGV(SVK_TAG, "ws[%s][%u] opcode[%d]", request->client()->remoteIP().toString().c_str(), request->client()->socket(), frame->type);

      int socket = request->client()->socket();

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

    // ADDED: Better logging in onClose
    _handler.onClose([this](PsychicWebSocketClient* client) {
      ESP_LOGI(SVK_TAG, "ws[%s][%u] disconnect", client->remoteIP().toString().c_str(), client->socket());
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

  void transmitId(PsychicWebSocketClient* client) {
    JsonDocument doc;
    JsonObject root = doc.to<JsonObject>();
    root["type"] = "id";
    root["id"] = clientId(client);

    String buffer;
    serializeJson(doc, buffer);
    client->sendMessage(buffer.c_str());
  }

  String clientId(PsychicWebSocketClient* client) { return WEB_SOCKET_ORIGIN_CLIENT_ID_PREFIX + String(client->socket()); }

  Module* findModule(const String& path) {
    for (Module* module : modules) {
      if (contains(path.c_str(), module->_moduleName.c_str())) return module;
    }
    return nullptr;
  }
};

#endif