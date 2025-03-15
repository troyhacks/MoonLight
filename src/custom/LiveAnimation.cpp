/**
    @title     MoonLight
    @file      LiveAnimation.cpp
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/moonlight/animation/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
**/

#if FT_LIVEANIMATION == 1

#include "LiveAnimation.h"
void LiveAnimationState::read(LiveAnimationState &state, JsonObject &root)
{
    ESP_LOGI("", "LiveAnimation::read");
    root["lightsOn"] = state.lightsOn;
    root["brightness"] = state.brightness;
    root["animation"] = state.animation;

    root["animations"].to<JsonArray>();
    root["animations"].add("Random");
    root["animations"].add("Rainbow");
    //find all the .sc files on FS
    #if FT_ENABLED(FT_FILEMANAGER)
        File rootFolder = ESPFS.open("/");
        walkThroughFiles(rootFolder, [&](File folder, File file) {
            if (strstr(file.name(), ".sc")) {
                ESP_LOGD("", "found file %s", file.path());
                root["animations"].add((char *)file.path());
            }
        });
        rootFolder.close();
    #endif

    root["driverOn"] = state.driverOn;
}

StateUpdateResult LiveAnimationState::update(JsonObject &root, LiveAnimationState &state)
{
    bool changed = false;

    if (state.lightsOn != root["lightsOn"] | state.lightsOn) {
        state.lightsOn = root["lightsOn"]; changed = true;
    }
    if (state.brightness != root["brightness"] | state.brightness) {
        state.brightness = root["brightness"]; changed = true;
        FastLED.setBrightness(state.lightsOn?state.brightness:0);
    }
    if (state.animation != root["animation"].as<String>()) {
        state.animation = root["animation"].as<String>(); changed = true;
        //set new animation (Yves)
    }
    if (state.driverOn != root["driverOn"] | state.driverOn) {
        state.driverOn = root["driverOn"]; changed = true;
    }

    if (changed) {
        ESP_LOGD("", "LiveAnimation::update");
    }

    return changed?StateUpdateResult::CHANGED:StateUpdateResult::UNCHANGED;
}

LiveAnimation::LiveAnimation(PsychicHttpServer *server,
                                     ESP32SvelteKit *sveltekit
                                     #if FT_ENABLED(FT_FILEMANAGER)
                                         , FilesService *filesService
                                     #endif
                                    ) : _httpEndpoint(LiveAnimationState::read,
                                                                                                         LiveAnimationState::update,
                                                                                                         this,
                                                                                                         server,
                                                                                                         "/rest/liveAnimation",
                                                                                                         sveltekit->getSecurityManager(),
                                                                                                         AuthenticationPredicates::IS_AUTHENTICATED),
                                                                                           _eventEndpoint(LiveAnimationState::read,
                                                                                                          LiveAnimationState::update,
                                                                                                          this,
                                                                                                          sveltekit->getSocket(),
                                                                                                          "liveAnimation"),
                                                                                           _webSocketServer(LiveAnimationState::read,
                                                                                                            LiveAnimationState::update,
                                                                                                            this,
                                                                                                            server,
                                                                                                            "/ws/liveAnimation",
                                                                                                            sveltekit->getSecurityManager(),
                                                                                                            AuthenticationPredicates::IS_AUTHENTICATED),
                                                                                            _socket(sveltekit->getSocket()),
                                                                                             _fsPersistence(LiveAnimationState::read,
                                                                                                      LiveAnimationState::update,
                                                                                                      this,
                                                                                                      sveltekit->getFS(),
                                                                                                      "/config/liveAnimation.json")
{

    // configure settings service update handler to update state
    addUpdateHandler([&](const String &originId)
                     { onConfigUpdated(); },
                     false);

    #if FT_ENABLED(FT_FILEMANAGER)
        _filesService = filesService;
    #endif
}

void LiveAnimation::begin()
{
    _httpEndpoint.begin();
    _eventEndpoint.begin();
    _fsPersistence.readFromFS();

    #if FT_ENABLED(FT_FILEMANAGER)
        update_handler_id_t _updateHandlerId = _filesService->addUpdateHandler([&](const String &originId)
        { 
            ESP_LOGD("", "FilesService::updateHandler %s", originId.c_str());
            _filesService->read([&](FilesState &state) {
                for (auto changedFile : state.changedFiles) {
                    //if file is the current animation, recompile it (to do: multiple animations)
                    if (changedFile == _state.animation) {
                        ESP_LOGD("", "LiveAnimation::updateHandler changedFile %s", changedFile.c_str());
                        //send UI spinner
                        //recompile ... (Yves)
                        //stop UI spinner
                        break;
                    }
                }
            });
        });
    #endif

    onConfigUpdated();

    FastLED.addLeds<WS2812B, 2, GRB>(_state.leds, 0, _state.nrOfLeds); //pin 16
    FastLED.setMaxPowerInMilliWatts(10000); // 5v, 2000mA
    ESP_LOGD("", "FastLED.addLeds n:%d o:%d b:%d", _state.nrOfLeds, _state.lightsOn, _state.brightness);
}

void LiveAnimation::onConfigUpdated()
{
    ESP_LOGI("", "LiveAnimation::onConfigUpdated o:%d b:%d", _state.lightsOn, _state.brightness);
}

void LiveAnimation::loop()
{
    //select the right effect
    if (_state.animation == "Random") {
        fadeToBlackBy(_state.leds, _state.nrOfLeds, 70);
        _state.leds[random16(_state.nrOfLeds)] = CRGB(255, 0, 0); //one random led red
    } else if (_state.animation == "Rainbow") {
        static uint8_t hue = 0;
        fill_rainbow(_state.leds, _state.nrOfLeds, hue++, 7);
    } else {
        //Live script (Yves)
    }

    if (_state.driverOn)
        FastLED.show();
}

#endif