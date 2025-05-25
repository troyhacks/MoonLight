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

        ESP_LOGD(TAG, "L:%d(%d) LH:%d N:%d PL:%d(%d) VL:%d MH:%d", sizeof(Lights), sizeof(LightsHeader), sizeof(Lights) - sizeof(LightsHeader), sizeof(Node), sizeof(PhysicalLayer), sizeof(PhysicalLayer)-sizeof(Lights), sizeof(VirtualLayer), sizeof(MovingHead));
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
        property = root.add<JsonObject>(); property["name"] = "pin"; property["type"] = "select"; property["default"] = "16"; values = property["values"].to<JsonArray>();
        values.add("2");
        values.add("16");

        #if FT_ENABLED(FT_MONITOR)
            property = root.add<JsonObject>(); property["name"] = "monitorOn"; property["type"] = "checkbox"; property["default"] = true;
        #endif

    }

    //implement business logic
    void onUpdate(UpdatedItem &updatedItem) override
    {
        if (equal(updatedItem.name, "pin") || equal(updatedItem.name, "red") || equal(updatedItem.name, "green") || equal(updatedItem.name, "blue")) {
            ESP_LOGD(TAG, "handle %s[%d]%s[%d].%s = %s -> %s", updatedItem.parent[0], updatedItem.index[0], updatedItem.parent[1], updatedItem.index[1], updatedItem.name, updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());

            //addLeds twice is temp hack to make rgb sliders work
            switch (_state.data["pin"].as<int>()) {
                case 2:
                    FastLED.addLeds<WS2812B, 16, GRB>(layerP.lights.leds, 0, layerP.lights.header.nrOfLights).setCorrection(CRGB(_state.data["red"],_state.data["green"],_state.data["blue"]));
                    FastLED.addLeds<WS2812B, 2, GRB>(layerP.lights.leds, 0, layerP.lights.header.nrOfLights).setCorrection(CRGB(_state.data["red"],_state.data["green"],_state.data["blue"]));
                    break;
                case 16:
                    FastLED.addLeds<WS2812B, 2, GRB>(layerP.lights.leds, 0, layerP.lights.header.nrOfLights).setCorrection(CRGB(_state.data["red"],_state.data["green"],_state.data["blue"]));
                    FastLED.addLeds<WS2812B, 16, GRB>(layerP.lights.leds, 0, layerP.lights.header.nrOfLights).setCorrection(CRGB(_state.data["red"],_state.data["green"],_state.data["blue"]));
                    break;
                default:
                    ESP_LOGD(TAG, "unknown pin %d", _state.data["pin"].as<int>());
            }
            FastLED.setMaxPowerInMilliWatts(10000); // 5v, 2000mA, to protect usb while developing
            // layerP.lights.header.brightness = _state.data["lightsOn"]?_state.data["brightness"]:0;
            // FastLED.setBrightness(layerP.lights.header.brightness);
            ESP_LOGD(TAG, "FastLED.addLeds n:%d", layerP.lights.header.nrOfLights);
        } else if (equal(updatedItem.name, "lightsOn") || equal(updatedItem.name, "brightness")) {
            ESP_LOGD(TAG, "handle %s[%d]%s[%d].%s = %s -> %s", updatedItem.parent[0], updatedItem.index[0], updatedItem.parent[1], updatedItem.index[1], updatedItem.name, updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
            layerP.lights.header.brightness = _state.data["lightsOn"]?_state.data["brightness"]:0;
            FastLED.setBrightness(layerP.lights.header.brightness);

        // ESP_LOGD(TAG, "no handle for %s[%d]%s[%d].%s = %s -> %s", updatedItem.parent[0], updatedItem.index[0], updatedItem.parent[1], updatedItem.index[1], updatedItem.name, updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
    }
}
  
}; // class ModuleLightsControl

#endif
#endif