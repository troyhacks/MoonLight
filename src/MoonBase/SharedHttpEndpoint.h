// In a new file: lib/framework/SharedHttpEndpoint.h
#ifndef SharedHttpEndpoint_h
#define SharedHttpEndpoint_h

#include <PsychicHttp.h>
#include <SecurityManager.h>

#include <map>

#include "Module.h"

class SharedHttpEndpoint {
 private:
  PsychicHttpServer* _server;
  SecurityManager* _securityManager;

 public:
  SharedHttpEndpoint(PsychicHttpServer* server, SecurityManager* securityManager) : _server(server), _securityManager(securityManager) {}

  // Register a module with its path
  void registerModule(Module* module) {
    String path = "/rest/" + module->_moduleName;

// ADDED: CORS OPTIONS handler
#ifdef ENABLE_CORS
    _server->on(path.c_str(), HTTP_OPTIONS, _securityManager->wrapRequest([](PsychicRequest* request) { return request->reply(200); }, AuthenticationPredicates::IS_AUTHENTICATED));
#endif

    // GET handler
    _server->on(path.c_str(), HTTP_GET, _securityManager->wrapRequest([this](PsychicRequest* request) { return handleGet(request); }, AuthenticationPredicates::IS_ADMIN));

    // POST handler
    _server->on(path.c_str(), HTTP_POST, _securityManager->wrapCallback([this](PsychicRequest* request, JsonVariant& json) { return handlePost(request, json); }, AuthenticationPredicates::IS_ADMIN));

    ESP_LOGV(SVK_TAG, "Registered HTTP endpoint: %s", path.c_str());
  }

  void begin() {
    // Single wildcard handler for all modules
    // _server->on("/rest/*", HTTP_GET, _securityManager->wrapRequest([this](PsychicRequest* request) { return handleGet(request); }, AuthenticationPredicates::IS_ADMIN));

    // _server->on("/rest/*", HTTP_POST, _securityManager->wrapCallback([this](PsychicRequest* request, JsonVariant& json) { return handlePost(request, json); }, AuthenticationPredicates::IS_ADMIN));
  }

 private:
  esp_err_t handleGet(PsychicRequest* request) {
    EXT_LOGD(ML_TAG, "search module %s", request->path().c_str());
    Module* module = findModule(request->path());
    if (!module) return request->reply(404);

    PsychicJsonResponse response = PsychicJsonResponse(request, false);
    JsonObject jsonObject = response.getRoot();
    module->read(jsonObject, ModuleState::read);
    return response.send();
  }

  // CHANGED: POST handler returns updated state
  esp_err_t handlePost(PsychicRequest* request, JsonVariant& json) {
    if (!json.is<JsonObject>()) return request->reply(400);

    Module* module = findModule(request->path());
    if (!module) return request->reply(404);

    JsonObject jsonObject = json.as<JsonObject>();

    // CHANGED: Use updateWithoutPropagation pattern
    StateUpdateResult outcome = module->updateWithoutPropagation(jsonObject, ModuleState::update);

    if (outcome == StateUpdateResult::ERROR) {
      return request->reply(400);
    } else if (outcome == StateUpdateResult::CHANGED) {
      // Persist changes to FS
      module->callUpdateHandlers(HTTP_ENDPOINT_ORIGIN_ID);
    }

    // ADDED: Return updated state in response
    PsychicJsonResponse response = PsychicJsonResponse(request, false);
    JsonObject responseObj = response.getRoot();
    module->read(responseObj, ModuleState::read);
    return response.send();
  }

  Module* findModule(const String& path) {
    for (Module* module : modules) {
      if (contains(path.c_str(), module->_moduleName.c_str())) return module;
    }
    return nullptr;
  }
};

#endif