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

    JsonDocument data;

    ModuleState() {
        //set default

        //only if no file ...
        JsonDocument definition;
        setupDefinition(definition.to<JsonArray>());
        char buffer[256];
        serializeJson(definition, buffer, sizeof(buffer));
        ESP_LOGD("", "setupDefinition %s", buffer);
        
        JsonObject root = data.to<JsonObject>();

        //create doc based on definition...
        for (JsonObject property: definition.as<JsonArray>()) {
            if (property["type"] != "array") {
                root[property["name"]] = property["default"];
            } else {
                root[property["name"]].to<JsonArray>();
                //loop over detail propertys (recursive)
            }
        }
        //to do: make recursive

        // JsonArray array = root["nodes"].to<JsonArray>();
        // JsonObject property;
        // property = array.add<JsonObject>(); property["animation"] = "Random"; property["size"] = 100; 
        // property = array.add<JsonObject>(); property["animation"] = "Rainbow"; property["size"] = 200; 

        // char buffer[256];
        serializeJson(root, buffer, sizeof(buffer));
        ESP_LOGD("", "setupData %s", buffer);

    }

    void setupDefinition(JsonArray root) {
        JsonObject property;
        JsonArray details;
        JsonArray values;

        property = root.add<JsonObject>(); property["name"] = "lightsOn"; property["type"] = "boolean"; property["default"] = true;
        property = root.add<JsonObject>(); property["name"] = "brightness"; property["type"] = "number"; property["min"] = 0; property["max"] = 255; property["default"] = 73;
        property = root.add<JsonObject>(); property["name"] = "driverOn"; property["type"] = "boolean"; property["default"] = false;

        property = root.add<JsonObject>(); property["name"] = "nodes"; property["type"] = "array"; details = property["n"].to<JsonArray>();
        property = details.add<JsonObject>(); property["name"] = "animation"; property["type"] = "select"; property["default"] = "Random"; values = property["values"].to<JsonArray>();
        property = details.add<JsonObject>(); property["name"] = "size"; property["type"] = "number"; property["default"] = 85;

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

        property = root.add<JsonObject>(); property["name"] = "projections"; property["type"] = "array"; details = property["n"].to<JsonArray>();
        property = details.add<JsonObject>(); property["name"] = "name"; property["type"] = "text"; property["default"] = "Pinwheel"; values = property["values"].to<JsonArray>();
        property = details.add<JsonObject>(); property["name"] = "type"; property["type"] = "text"; property["default"] = "normal";

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
