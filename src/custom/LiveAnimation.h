/**
    @title     MoonLight
    @file      LiveAnimation.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/custom/animation/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
**/

#ifndef LiveAnimation_h
#define LiveAnimation_h

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

#include "FastLED.h"
#define MAXLEDS 8192

class LiveAnimationState
{
public:
    std::vector<String> updatedItems;

    CRGB leds[MAXLEDS];
    uint16_t nrOfLeds = 256;

    bool lightsOn = true;
    uint8_t brightness = 32;
    String animation = "Random";
    bool driverOn = true;

    static void read(LiveAnimationState &state, JsonObject &root);
    static StateUpdateResult update(JsonObject &root, LiveAnimationState &state);
};

class LiveAnimation : public StatefulService<LiveAnimationState>
{
public:
    LiveAnimation(PsychicHttpServer *server,
                      ESP32SvelteKit *sveltekit
                      #if FT_ENABLED(FT_FILEMANAGER)
                          , FilesService *filesService
                      #endif
                    );

    void begin();
    void loop();

    void compileAndRun();
    void driverShow();

protected:
    EventSocket *_socket;

private:
    HttpEndpoint<LiveAnimationState> _httpEndpoint;
    EventEndpoint<LiveAnimationState> _eventEndpoint;
    WebSocketServer<LiveAnimationState> _webSocketServer;
    FSPersistence<LiveAnimationState> _fsPersistence;

    #if FT_ENABLED(FT_FILEMANAGER)
        FilesService *_filesService;
    #endif

};

#endif
#endif
