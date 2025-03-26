/**
    @title     MoonLight
    @file      Module.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/custom/animation/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
**/

#ifndef Module_h
#define Module_h

#if FT_LIVEANIMATION == 1

#include <EventSocket.h>
#include <HttpEndpoint.h>
#include <EventEndpoint.h>
#include <WebSocketServer.h>
#include <PsychicHttp.h>
#include <FSPersistence.h>
#include <ESP32SvelteKit.h>

#if FT_ENABLED(FT_FILEMANAGER)
    #include "FilesService.h"
#endif

struct UpdatedItem {
    JsonString parent;
    JsonString name;
    JsonVariant value;
    uint8_t index;
};

class ModuleState
{
public:
    std::vector<UpdatedItem> updatedItems;

    JsonDocument data;

    void setupData() {

        ESP_LOGD("", "ModuleState::setupData size %d", data.size());

        //only if no file ...
        if (data.size() == 0) {
            JsonDocument definition;
            setupDefinition(definition.to<JsonArray>());

            char buffer[256];
            serializeJson(definition, buffer, sizeof(buffer));
            ESP_LOGD("", "setupDefinition %s", buffer);
            
            JsonObject root = data.to<JsonObject>();

            //create doc based on definition... only top level properties
            for (JsonObject property: definition.as<JsonArray>()) {
                if (property["type"] != "array") {
                    root[property["name"]] = property["default"];
                } else {
                    root[property["name"]].to<JsonArray>();
                    //loop over detail propertys (recursive)
                }
            }

            // char buffer[256];
            serializeJson(root, buffer, sizeof(buffer));
            ESP_LOGD("", "setupData %s", buffer);
        }
    }

    virtual void setupDefinition(JsonArray root) { //virtual so it can be overriden in derived classes
        JsonObject property;
        JsonArray details;
        JsonArray values;

        property = root.add<JsonObject>(); property["name"] = "text"; property["type"] = "text"; property["default"] = "MoonLight";
    }

    static void read(ModuleState &state, JsonObject &root);
    static StateUpdateResult update(JsonObject &root, ModuleState &state);
};

template <class T>
class Module : public StatefulService<T>
{
public:
    String _moduleName = "";

    Module(String moduleName, PsychicHttpServer *server,
                      ESP32SvelteKit *sveltekit
                      #if FT_ENABLED(FT_FILEMANAGER)
                          , FilesService *filesService
                      #endif
                    ): _httpEndpoint(T::read,
                                        T::update,
                                        this,
                                        server,
                                        String("/rest/" + moduleName).c_str(),
                                        sveltekit->getSecurityManager(),
                                        AuthenticationPredicates::IS_AUTHENTICATED),
                        _eventEndpoint(T::read,
                                        T::update,
                                        this,
                                        sveltekit->getSocket(),
                                        moduleName.c_str()),
                        _webSocketServer(T::read,
                                        T::update,
                                        this,
                                        server,
                                        String("/ws/" + moduleName).c_str(),
                                        sveltekit->getSecurityManager(),
                                        AuthenticationPredicates::IS_AUTHENTICATED),
                        _socket(sveltekit->getSocket()),
                            _fsPersistence(T::read,
                                    T::update,
                                    this,
                                    sveltekit->getFS(),
                                    String("/config/" + moduleName + ".json").c_str())
    {
        _moduleName = moduleName;
        
        ESP_LOGD("", "Module::constructor %s", moduleName.c_str());
        _server = server;

        // configure settings service update handler to update state
        #if FT_ENABLED(FT_FILEMANAGER)
        _filesService = filesService;
        #endif

    }

    void begin()
    {
        ESP_LOGD("", "Module::begin");
        _httpEndpoint.begin();
        _eventEndpoint.begin();
        _fsPersistence.readFromFS(); //overwrites the default settings in state

        this->read([&](T &state) {
            state.setupData(); //if no data readFromFS, using overridden setupDefinition
        });

        _server->on(String("/rest/" + _moduleName + "Def").c_str(), HTTP_GET, [&](PsychicRequest *request) {
            PsychicJsonResponse response = PsychicJsonResponse(request, false);
            JsonArray root = response.getRoot().to<JsonArray>();
    
            this->read([&](T &state) {
                state.setupDefinition(root);
            });
    
            char buffer[256];
            serializeJson(root, buffer, sizeof(buffer));
            ESP_LOGD("", "moduleDef %s", buffer);
        
            return response.send();
        });
    
        //if update, for all updated items, run onUpdate
        this->addUpdateHandler([&](const String &originId)
        {
            ESP_LOGD("", "Module::updateHandler %s", originId.c_str());
            this->read([&](T &state) {
                for (UpdatedItem updatedItem : state.updatedItems) {
                    onUpdate(updatedItem);
                }
            });
        });
    
    }
    
    void onUpdate(UpdatedItem updatedItem)
    {
        ESP_LOGD("", "no handle for %s.%s[%d] %s", updatedItem.parent.c_str(), updatedItem.name.c_str(), updatedItem.index, updatedItem.value.as<String>().c_str());
    }

    void loop() {};

protected:
    EventSocket *_socket;

private:
    HttpEndpoint<T> _httpEndpoint;
    EventEndpoint<T> _eventEndpoint;
    WebSocketServer<T> _webSocketServer;
    FSPersistence<T> _fsPersistence;
    PsychicHttpServer *_server;

    #if FT_ENABLED(FT_FILEMANAGER)
        FilesService *_filesService;
    #endif

};

#endif
#endif
