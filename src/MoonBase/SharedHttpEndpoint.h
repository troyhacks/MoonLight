// In a new file: lib/framework/SharedHttpEndpoint.h
#ifndef SharedHttpEndpoint_h
#define SharedHttpEndpoint_h

#include <PsychicHttp.h>
#include <SecurityManager.h>

#include <map>

#include "Module.h"

class SharedHttpEndpoint {
 private:
//   std::map<String, Module*> _modulesByPath;  // "/rest/moduleName" -> Module*
  PsychicHttpServer* _server;
  SecurityManager* _securityManager;

 public:
  SharedHttpEndpoint(PsychicHttpServer* server, SecurityManager* securityManager) : _server(server), _securityManager(securityManager) {}

  // Register a module with its path
  void registerModule(Module* module) { 
    _server->on(String("/rest/" + module->_moduleName).c_str(), HTTP_GET, _securityManager->wrapRequest([this](PsychicRequest* request) { return handleGet(request); }, AuthenticationPredicates::IS_ADMIN));

    _server->on(String("/rest/" + module->_moduleName).c_str(), HTTP_POST, _securityManager->wrapCallback([this](PsychicRequest* request, JsonVariant& json) { return handlePost(request, json); }, AuthenticationPredicates::IS_ADMIN));
    EXT_LOGD(ML_TAG, "%s", module->_moduleName.c_str());
    // _modulesByPath[path] = module; 
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

  esp_err_t handlePost(PsychicRequest* request, JsonVariant& json) {
    if (!json.is<JsonObject>()) return request->reply(400);

    EXT_LOGD(ML_TAG, "search module %s", request->path().c_str());

    Module* module = findModule(request->path());
    if (!module) return request->reply(404);

    JsonObject jsonObject = json.as<JsonObject>();
    StateUpdateResult outcome = module->update(jsonObject, ModuleState::update, "http");
    return (outcome == StateUpdateResult::ERROR) ? request->reply(400) : request->reply(200);
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