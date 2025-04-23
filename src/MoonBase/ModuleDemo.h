/**
    @title     MoonBase
    @file      ModuleDemo.h
    @repo      https://github.com/ewowi/MoonBase, submit changes to this file as PRs
    @Authors   https://github.com/ewowi/MoonBase/commits/main
    @Doc       https://ewowi.github.io/MoonBase/modules/module/demo/
    @Copyright © 2025 Github MoonBase Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
**/

#ifndef ModuleDemo_h
#define ModuleDemo_h

#if FT_MOONBASE == 1

#include "Module.h"

class ModuleDemo : public Module
{
public:

    ModuleDemo(PsychicHttpServer *server,
            ESP32SvelteKit *sveltekit,
            FilesService *filesService
        ) : Module("demo", server, sveltekit, filesService) {
            ESP_LOGD(TAG, "constructor");
        }

    void setupDefinition(JsonArray root) override{
        ESP_LOGD(TAG, "");
        JsonObject property; // state.data has one or more properties
        JsonArray details; // if a property is an array, this is the details of the array
        JsonArray values; // if a property is a select, this is the values of the select

        property = root.add<JsonObject>(); property["name"] = "hostName"; property["type"] = "text"; property["default"] = "MoonBase";
        property = root.add<JsonObject>(); property["name"] = "connectionMode"; property["type"] = "select"; property["default"] = "Signal Strength"; values = property["values"].to<JsonArray>();
        values.add("Offline");
        values.add("Signal Strength");
        values.add("Priority");
        property = root.add<JsonObject>(); property["name"] = "millis"; property["type"] = "number";

        property = root.add<JsonObject>(); property["name"] = "savedNetworks"; property["type"] = "array"; details = property["n"].to<JsonArray>();
        {
            property = details.add<JsonObject>(); property["name"] = "SSID"; property["type"] = "text"; property["default"] = "ewtr"; property["min"] = 3; property["max"] = 32; 
            property = details.add<JsonObject>(); property["name"] = "Password"; property["type"] = "password"; property["default"] = "";
        }

        property = root.add<JsonObject>(); property["name"] = "invoices"; property["type"] = "array"; details = property["n"].to<JsonArray>();
        {
            property = details.add<JsonObject>(); property["name"] = "number"; property["type"] = "number"; property["default"] = 1000; property["min"] = 1000; property["max"] = 9999; 
            property = details.add<JsonObject>(); property["name"] = "name"; property["type"] = "text"; property["default"] = "ewowi";
            property = details.add<JsonObject>(); property["name"] = "date"; property["type"] = "date"; property["default"] = "2025-03-21";
            property = details.add<JsonObject>(); property["name"] = "lines"; property["type"] = "array"; details = property["n"].to<JsonArray>();
            {
                property = details.add<JsonObject>(); property["name"] = "service"; property["type"] = "text"; property["default"] = "consulting";
                property = details.add<JsonObject>(); property["name"] = "price"; property["type"] = "number"; property["default"] = 100;
            }
        }

        // {"hostName":"MoonBase","connectionMode":"Priority","savedNetworks":[],"invoices":[{"number":1000,"name":"ewowi","date":"2025-03-21", "lines":[{"service":"con", "price":124}]}]}

        // char buffer[256];
        // serializeJson(root, buffer, sizeof(buffer));
        // ESP_LOGD(TAG, "definition %s", buffer);
    }

    void onUpdate(UpdatedItem &updatedItem) override
    {
        ESP_LOGD(TAG, "no handle for %s = %s -> %s", updatedItem.name, updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
    }

    void loop1s() {
        if (!_socket->getConnectedClients()) return; 

        JsonDocument newData; //to only send updatedData

        newData["millis"] = millis()/1000;

        JsonObject newDataObject = newData.as<JsonObject>();
}
};

#endif
#endif