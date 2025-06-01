/**
    @title     MoonLight
    @file      ModuleLightsControl.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/modules/module/lightsControl/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
**/

#ifndef ModuleLightsControl_h
#define ModuleLightsControl_h

#if FT_MOONLIGHT

#undef TAG
#define TAG "💫"

#include "FastLED.h"
#include "../MoonBase/Module.h"

#include "ModuleAnimations.h"

class ModuleLightsControl : public Module
{
public:

    PsychicHttpServer *_server;

    ModuleLightsControl(PsychicHttpServer *server,
        ESP32SvelteKit *sveltekit,
        FilesService *filesService
    ) : Module("lightsControl", server, sveltekit, filesService) {
        ESP_LOGD(TAG, "constructor");
        _server = server;
    }

    void begin() {
        Module::begin();

        ESP_LOGD(TAG, "L:%d(%d) LH:%d N:%d PL:%d(%d) VL:%d MH:%d", sizeof(Lights), sizeof(LightsHeader), sizeof(Lights) - sizeof(LightsHeader), sizeof(Node), sizeof(PhysicalLayer), sizeof(PhysicalLayer)-sizeof(Lights), sizeof(VirtualLayer), sizeof(MovingHeadMiniLed));
    }

    //define the data model
    void setupDefinition(JsonArray root) override {
        ESP_LOGD(TAG, "");
        JsonObject property; // state.data has one or more properties
        JsonArray details = root; // if a property is an array, this is the details of the array
        JsonArray values; // if a property is a select, this is the values of the select

        property = root.add<JsonObject>(); property["name"] = "lightsOn"; property["type"] = "checkbox"; property["default"] = true;
        property = root.add<JsonObject>(); property["name"] = "brightness"; property["type"] = "range"; property["default"] = 10;
        property = root.add<JsonObject>(); property["name"] = "red"; property["type"] = "range"; property["default"] = 255; property["color"] = "Red";
        property = root.add<JsonObject>(); property["name"] = "green"; property["type"] = "range"; property["default"] = 255; property["color"] = "Green";
        property = root.add<JsonObject>(); property["name"] = "blue"; property["type"] = "range"; property["default"] = 255; property["color"] = "Blue";
        property = root.add<JsonObject>(); property["name"] = "preset"; property["type"] = "select"; property["default"] = "Preset1"; values = property["values"].to<JsonArray>();
        values.add("Preset1");
        values.add("Preset2");
        property = root.add<JsonObject>(); property["name"] = "driverOn"; property["type"] = "checkbox"; property["default"] = true;

        #if FT_ENABLED(FT_MONITOR)
            property = root.add<JsonObject>(); property["name"] = "monitorOn"; property["type"] = "checkbox"; property["default"] = true;
        #endif

    }

    //implement business logic
    void onUpdate(UpdatedItem &updatedItem) override
    {
        if (equal(updatedItem.name, "red") || equal(updatedItem.name, "green") || equal(updatedItem.name, "blue")) {
            ESP_LOGD(TAG, "handle %s[%d]%s[%d].%s = %s -> %s", updatedItem.parent[0], updatedItem.index[0], updatedItem.parent[1], updatedItem.index[1], updatedItem.name, updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
            layerP.ledsDriver.setColorCorrection(_state.data["red"], _state.data["green"], _state.data["blue"]);
        } else if (equal(updatedItem.name, "lightsOn") || equal(updatedItem.name, "brightness")) {
            ESP_LOGD(TAG, "handle %s[%d]%s[%d].%s = %s -> %s", updatedItem.parent[0], updatedItem.index[0], updatedItem.parent[1], updatedItem.index[1], updatedItem.name, updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
            layerP.lights.header.brightness = _state.data["lightsOn"]?_state.data["brightness"]:0;
            layerP.ledsDriver.setBrightness(layerP.lights.header.brightness);
        }
        // ESP_LOGD(TAG, "no handle for %s[%d]%s[%d].%s = %s -> %s", updatedItem.parent[0], updatedItem.index[0], updatedItem.parent[1], updatedItem.index[1], updatedItem.name, updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
    }

    //ledsDriver.show
    void loop() {
        if (layerP.lights.header.isPositions == 0 && _state.data["driverOn"])
            layerP.ledsDriver.show();
    }

    void loop50ms() {
        #if FT_ENABLED(FT_MONITOR)

            static int monitorMillis = 0;
            if (millis() - monitorMillis >= layerP.lights.header.nrOfLights / 12) { //max 12000 leds per second -> 1 second for 12000 leds
                monitorMillis = millis();
                
                if (layerP.lights.header.isPositions == 10) { //send to UI
                    if (_socket->getConnectedClients() && _state.data["monitorOn"])
                        _socket->emitEvent("monitor", (char *)&layerP.lights, sizeof(LightsHeader) + MIN(layerP.lights.header.nrOfLights * sizeof(Coord3D), MAX_CHANNELS));
                    layerP.lights.header.isPositions = 0;
                } else if (layerP.lights.header.isPositions == 0) {//send to UI
                    if (_socket->getConnectedClients() && _state.data["monitorOn"])
                        _socket->emitEvent("monitor", (char *)&layerP.lights, sizeof(LightsHeader) + MIN(layerP.lights.header.nrOfLights * layerP.lights.header.channelsPerLight, MAX_CHANNELS));
                }
            }
        #endif
    }


}; // class ModuleLightsControl

#endif
#endif