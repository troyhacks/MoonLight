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

class ModuleState
{
public:
    std::vector<String> updatedItems;

    JsonDocument doc;

    ModuleState() {
        //set default

        //only if no file ...
        JsonDocument docDD;
        setupDD(docDD.to<JsonArray>());
        char buffer[256];
        serializeJson(docDD, buffer, sizeof(buffer));
        ESP_LOGD("", "initDD %s", buffer);
        
        JsonObject root = doc.to<JsonObject>();

        //create doc based on docDD...
        for (JsonObject object: docDD.as<JsonArray>()) {
            if (object["type"] != "array") {
                root[object["name"]] = object["default"];
            } else {
                root[object["name"]].to<JsonArray>();
                //loop over detail objects (recursive)
            }
        }
        //to do: make recursive

        root["nodes"].to<JsonArray>();
        JsonObject object;
        object = root["nodes"].add<JsonObject>(); object["animation"] = "Random"; object["size"] = 100; 
        object = root["nodes"].add<JsonObject>(); object["animation"] = "Rainbow"; object["size"] = 200; 

        // char buffer[256];
        serializeJson(root, buffer, sizeof(buffer));
        ESP_LOGD("", "init %s", buffer);

    }

    void setupDD(JsonArray root) {
        JsonObject object;
        object = root.add<JsonObject>(); object["name"] = "lightsOn"; object["type"] = "boolean"; object["default"] = true;
        object = root.add<JsonObject>(); object["name"] = "brightness"; object["type"] = "number"; object["min"] = 0; object["max"] = 255; object["default"] = 73;
        object = root.add<JsonObject>(); object["name"] = "driverOn"; object["type"] = "boolean"; object["default"] = false;
        object = root.add<JsonObject>(); object["name"] = "nodes"; object["type"] = "array"; JsonArray details = object["n"].to<JsonArray>();

        object = details.add<JsonObject>(); object["name"] = "animation"; object["type"] = "select"; object["default"] = "Random"; JsonArray values = object["values"].to<JsonArray>();

        values.add("Random");
        values.add("Rainbow");
        //find all the .sc files on FS
        #if FT_ENABLED(FT_FILEMANAGER)
            File rootFolder = ESPFS.open("/");
            walkThroughFiles(rootFolder, [&](File folder, File file) {
                if (strstr(file.name(), ".sc")) {
                    // ESP_LOGD("", "found file %s", file.path());
                    values.add((char *)file.path());
                }
            });
            rootFolder.close();
        #endif

        object = details.add<JsonObject>(); object["name"] = "size"; object["type"] = "number";

        char buffer[256];
        serializeJson(root, buffer, sizeof(buffer));
        ESP_LOGD("", "definition %s", buffer);

    }

    static void read(ModuleState &state, JsonObject &root);
    static StateUpdateResult update(JsonObject &root, ModuleState &state);
};

class Module : public StatefulService<ModuleState>
{
public:
    Module(PsychicHttpServer *server,
                      ESP32SvelteKit *sveltekit
                      #if FT_ENABLED(FT_FILEMANAGER)
                          , FilesService *filesService
                      #endif
                    );

    void begin();
    void loop();

protected:
    EventSocket *_socket;

private:
    HttpEndpoint<ModuleState> _httpEndpoint;
    EventEndpoint<ModuleState> _eventEndpoint;
    WebSocketServer<ModuleState> _webSocketServer;
    FSPersistence<ModuleState> _fsPersistence;
    PsychicHttpServer *_server;

    #if FT_ENABLED(FT_FILEMANAGER)
        FilesService *_filesService;
    #endif

};

#endif
#endif
