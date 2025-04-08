/**
    @title     MoonBase
    @file      ModuleInstances.h
    @repo      https://github.com/ewowi/MoonBase, submit changes to this file as PRs
    @Authors   https://github.com/ewowi/MoonBase/commits/main
    @Doc       https://ewowi.github.io/MoonBase/modules/module/demo/
    @Copyright © 2025 Github MoonBase Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
**/

#ifndef ModuleInstances_h
#define ModuleInstances_h

#include "Module.h"

#include "Utilities.h"

struct UDPMessage {
    byte rommel[6];
    Char<32> name;
};

class ModuleInstances : public Module
{
public:

    WiFiUDP instanceUDP;
    uint16_t instanceUDPPort = 65506;
    bool instanceUDPConnected = false;

    ModuleInstances(PsychicHttpServer *server,
            ESP32SvelteKit *sveltekit,
            FilesService *filesService
        ) : Module("instances", server, sveltekit, filesService) {
            ESP_LOGD(TAG, "constructor");
    }

    void setupDefinition(JsonArray root) override{
        ESP_LOGD(TAG, "");
        JsonObject property;
        JsonArray details;
        JsonArray values;

        property = root.add<JsonObject>(); property["name"] = "hostName"; property["type"] = "text"; property["default"] = "MoonBase";

        property = root.add<JsonObject>(); property["name"] = "instances"; property["type"] = "array"; details = property["n"].to<JsonArray>();
        {
            property = details.add<JsonObject>(); property["name"] = "name"; property["type"] = "text"; 
            property = details.add<JsonObject>(); property["name"] = "ip"; property["type"] = "ip";;
            property = details.add<JsonObject>(); property["name"] = "time"; property["type"] = "text";;
        }
    }

    void onUpdate(UpdatedItem updatedItem) override
    {
        ESP_LOGD(TAG, "no handle for %s = %s -> %s", updatedItem.name, updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
    }

    void loop1s() {
        if (!instanceUDPConnected) return;

        readUDP();
    }

    void loop10s() {
        if (!instanceUDPConnected) {
            instanceUDPConnected = instanceUDP.begin(instanceUDPPort);
            ESP_LOGD(TAG, "instanceUDPConnected %d", instanceUDPConnected);
        }

        if (!instanceUDPConnected) return;

        writeUDP();
    }

    void readUDP() {
        size_t packetSize = instanceUDP.parsePacket();
        if (packetSize > 0) {
            char buffer[packetSize];
            instanceUDP.read(buffer, packetSize);
            // ESP_LOGD(TAG, "UDP packet read from %d: %s (%d)", instanceUDP.remoteIP()[3], buffer+6, packetSize);

            bool found = false;
            for (JsonObject instance: _state.data["instances"].as<JsonArray>()) {
                if (instance["ip"] == instanceUDP.remoteIP().toString()) {
                    found = true;
                    instance["name"] = buffer+6;
                    instance["time"] = millis();
                    // ESP_LOGD(TAG, "updated %s", instance["ip"].as<String>().c_str());
                }
            }
            if (!found) {
                JsonObject instance = _state.data["instances"].as<JsonArray>().add<JsonObject>();
                instance["ip"] = instanceUDP.remoteIP().toString();
                instance["name"] = buffer+6;
                instance["time"] = millis();
                ESP_LOGD(TAG, "added %s", instance["ip"].as<String>().c_str());
            }

            // char buffer[2048];
            // serializeJson(_state.data, buffer, sizeof(buffer));
            // ESP_LOGD(TAG, "data %s", buffer);
            JsonObject instancesData = _state.data.as<JsonObject>();
            _socket->emitEvent("instances", instancesData);
        }
    }

    void writeUDP() {
        if (instanceUDP.beginPacket(IPAddress(255, 255, 255, 255), instanceUDPPort)) {  // WLEDMM beginPacket == 0 --> error
            
            UDPMessage message;
            message.name = _state.data["hostName"].as<String>().c_str();
            instanceUDP.write((byte *)&message, sizeof(message));
            instanceUDP.endPacket();
            // ESP_LOGD(TAG, "UDP packet written (%d)", WiFi.localIP()[3]);

        }
    }
};

#endif