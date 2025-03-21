/**
    @title     MoonLight
    @file      Module.cpp
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/moonlight/animation/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
**/

#if FT_LIVEANIMATION == 1

#include "Module.h"

void ModuleState::read(ModuleState &state, JsonObject &root)
{
    TaskHandle_t httpdTask = xTaskGetHandle("httpd");
    ESP_LOGI("", "Module::read task %s %d", pcTaskGetName(httpdTask), uxTaskGetStackHighWaterMark(httpdTask));

    root.set(state.data.as<JsonObject>()); //copy
    char buffer[256];
    serializeJson(root, buffer, sizeof(buffer));
    ESP_LOGD("", "state.doc %s", buffer);

}

StateUpdateResult ModuleState::update(JsonObject &root, ModuleState &state)
{
    TaskHandle_t httpdTask = xTaskGetHandle("httpd");
    ESP_LOGI("", "Module::update task %s %d", pcTaskGetName(httpdTask), uxTaskGetStackHighWaterMark(httpdTask));
    state.updatedItems.clear();

    //to do: check which propertys have updated

    if (root.size() != 0) { // in case of empty file
        state.data.set(root); //copy
        char buffer[256];
        serializeJson(state.data, buffer, sizeof(buffer));
        ESP_LOGD("", "state.doc %s", buffer);
    }

    if (state.updatedItems.size())
        ESP_LOGD("", "Module::update %s", state.updatedItems.front().c_str());

    return root.size()?StateUpdateResult::CHANGED:StateUpdateResult::UNCHANGED;
    // return state.updatedItems.size()?StateUpdateResult::CHANGED:StateUpdateResult::UNCHANGED;
}

Module::Module(PsychicHttpServer *server,
                                     ESP32SvelteKit *sveltekit
                                     #if FT_ENABLED(FT_FILEMANAGER)
                                         , FilesService *filesService
                                     #endif
                                    ) : _httpEndpoint(ModuleState::read,
                                                                                                         ModuleState::update,
                                                                                                         this,
                                                                                                         server,
                                                                                                         "/rest/module",
                                                                                                         sveltekit->getSecurityManager(),
                                                                                                         AuthenticationPredicates::IS_AUTHENTICATED),
                                                                                           _eventEndpoint(ModuleState::read,
                                                                                                          ModuleState::update,
                                                                                                          this,
                                                                                                          sveltekit->getSocket(),
                                                                                                          "module"),
                                                                                           _webSocketServer(ModuleState::read,
                                                                                                            ModuleState::update,
                                                                                                            this,
                                                                                                            server,
                                                                                                            "/ws/module",
                                                                                                            sveltekit->getSecurityManager(),
                                                                                                            AuthenticationPredicates::IS_AUTHENTICATED),
                                                                                            _socket(sveltekit->getSocket()),
                                                                                             _fsPersistence(ModuleState::read,
                                                                                                      ModuleState::update,
                                                                                                      this,
                                                                                                      sveltekit->getFS(),
                                                                                                      "/config/module.json")
{
    ESP_LOGD("", "Module::constructor");
    _server = server;

    // configure settings service update handler to update state
    #if FT_ENABLED(FT_FILEMANAGER)
        _filesService = filesService;
    #endif
}

void Module::begin()
{
    ESP_LOGD("", "Module::begin");
    _httpEndpoint.begin();
    _eventEndpoint.begin();
    _fsPersistence.readFromFS(); //overwrites the default settings in state
    _server->on("/rest/moduleDef", HTTP_GET, [&](PsychicRequest *request) {
        PsychicJsonResponse response = PsychicJsonResponse(request, false);
        JsonArray root = response.getRoot().to<JsonArray>();

        _state.setupDefinition(root);

        char buffer[256];
        serializeJson(root, buffer, sizeof(buffer));
        ESP_LOGD("", "moduleDef %s", buffer);
    
        return response.send();
    });

}

void Module::loop()
{
}


#endif

