/**
    @title     MoonBase
    @file      FileManager.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/moonbase/FileManager/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact us for more information.
**/

#ifndef FileManager_h
#define FileManager_h

#if FT_MOONBASE == 1

#include <EventSocket.h>
#include <HttpEndpoint.h>
#include <EventEndpoint.h>
#include <WebSocketServer.h>
#include <PsychicHttp.h>
#include <ESP32SvelteKit.h>

class FilesState
{
public:
    std::vector<String> updatedItems;
    bool showHidden = false;

    static void read(FilesState &settings, JsonObject &root);

    static StateUpdateResult update(JsonObject &root, FilesState &filesState);

};

class FileManager : public StatefulService<FilesState>
{
public:
    FileManager(PsychicHttpServer *server,
                      ESP32SvelteKit *sveltekit);

    void begin();

protected:
    EventSocket *_socket;

private:
    HttpEndpoint<FilesState> _httpEndpoint;
    EventEndpoint<FilesState> _eventEndpoint;
    WebSocketServer<FilesState> _webSocketServer;
    PsychicHttpServer *_server;
    ESP32SvelteKit *_sveltekit;

    void onConfigUpdated();
};

#endif
#endif
