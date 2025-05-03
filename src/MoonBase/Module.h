/**
    @title     MoonBase
    @file      Module.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/modules/modules/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
**/

#ifndef Module_h
#define Module_h

#if FT_MOONBASE == 1

#undef TAG
#define TAG "🌙"

#include <EventSocket.h>
#include <HttpEndpoint.h>
#include <EventEndpoint.h>
#include <WebSocketServer.h>
#include <PsychicHttp.h>
#include <FSPersistence.h>
#include <ESP32SvelteKit.h>

#include "FilesService.h"

#include "Utilities.h"

//sizeof was 160 chars -> 80 -> 40
struct UpdatedItem {
    const char *parent[2]; //24 -> 8
    uint8_t index[2]; //2x1 = 2
    const char *name; //16 -> 4
    String oldValue; //32 -> 16, smaller then 11 bytes mostly
    JsonVariant value; //8

    UpdatedItem() {
        parent[0] = nullptr; //will be checked in onUpdate
        parent[1] = nullptr;
        index[0] = UINT8_MAX;
        index[1] = UINT8_MAX;
    }

    const char *parentX() { return equal(parent[1],"")?parent[0]:parent[1];}
    uint8_t indexX() { return index[1]==UINT8_MAX?index[0]:index[1];}

    const char *grandParent() { return equal(parent[1],"")?parent[1]:parent[0];}
    uint8_t grandIndex() { return index[1]==UINT8_MAX?index[1]:index[0];}
};

class ModuleState
{
public:
    JsonDocument data;

    std::function<void(JsonArray root)> setupDefinition = nullptr;

    void setupData();
    bool compareRecursive(JsonString parent, JsonVariant oldData, JsonVariant newData, UpdatedItem &updatedItem, uint8_t depth = UINT8_MAX, uint8_t index =UINT8_MAX);
    std::function<void(UpdatedItem &)> onUpdate = nullptr;

    static void read(ModuleState &state, JsonObject &root);
    static StateUpdateResult update(JsonObject &root, ModuleState &state);

};

class Module : public StatefulService<ModuleState>
{
public:
    String _moduleName = "";

    Module(String moduleName, PsychicHttpServer *server,
                      ESP32SvelteKit *sveltekit,
                      FilesService *filesService
                    );

    void begin();

    virtual void setupDefinition(JsonArray root);

    virtual void onUpdate(UpdatedItem &updatedItem);

protected:
    EventSocket *_socket;
    FilesService *_filesService;

private:
    HttpEndpoint<ModuleState> _httpEndpoint;
    EventEndpoint<ModuleState> _eventEndpoint;
    WebSocketServer<ModuleState> _webSocketServer;
    FSPersistence<ModuleState> _fsPersistence;
    PsychicHttpServer *_server;

    void onConfigUpdated();

};

#endif
#endif