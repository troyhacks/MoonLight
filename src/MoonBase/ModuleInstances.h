/**
    @title     MoonBase
    @file      ModuleInstances.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/moonbase/module/instances/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
**/

#ifndef ModuleInstances_h
#define ModuleInstances_h

#if FT_MOONBASE == 1

#include "Module.h"

#include "Utilities.h"

struct UDPMessage {
    uint8_t rommel[6];
    Char<32> name;
};

class ModuleInstances : public Module
{
public:

    WiFiUDP instanceUDP;
    uint16_t instanceUDPPort = 65506;
    bool instanceUDPConnected = false;

    ModuleInstances(PsychicHttpServer *server,
            ESP32SvelteKit *sveltekit
        ) : Module("instances", server, sveltekit) {
            ESP_LOGD(TAG, "constructor");
    }

    void setupDefinition(JsonArray root) override{
        ESP_LOGD(TAG, "");
        JsonObject property; // state.data has one or more properties
        JsonArray details; // if a property is an array, this is the details of the array
        JsonArray values; // if a property is a select, this is the values of the select

        Char<32> instanceName;
        instanceName = "MoonLight-";
        uint8_t mac[6];
        esp_read_mac(mac, ESP_MAC_WIFI_STA);
        char macStr[5] = {0};
        sprintf(macStr, "%02x%02x", mac[4], mac[5]);
        // instanceName += WiFi.macAddress().substring(12);//localIP().toString().c_str();
        instanceName += macStr;
        property = root.add<JsonObject>(); property["name"] = "instanceName"; property["type"] = "text"; property["default"] = instanceName.c_str();

        property = root.add<JsonObject>(); property["name"] = "instances"; property["type"] = "array"; details = property["n"].to<JsonArray>();
        {
            property = details.add<JsonObject>(); property["name"] = "name"; property["type"] = "text"; property["ro"] = true;
            property = details.add<JsonObject>(); property["name"] = "ip"; property["type"] = "ip"; property["ro"] = true;
            property = details.add<JsonObject>(); property["name"] = "time"; property["type"] = "time"; property["ro"] = true;
        }
    }

    void onUpdate(UpdatedItem &updatedItem) override
    {
        ESP_LOGD(TAG, "no handle for %s[%d]%s[%d].%s = %s -> %s", updatedItem.parent[0], updatedItem.index[0], updatedItem.parent[1], updatedItem.index[1], updatedItem.name, updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
    }

    void loop1s() {
        if (!_socket->getConnectedClients()) return; 

        if (!instanceUDPConnected) return;

        readUDP();
    }

    void loop10s() {
        if (!instanceUDPConnected) {
            instanceUDPConnected = instanceUDP.begin(instanceUDPPort);
            ESP_LOGD(TAG, "instanceUDPConnected %d i:%d p:%d", instanceUDPConnected, instanceUDP.remoteIP()[3], instanceUDPPort);
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

            //use state.data or newData?
            for (JsonObject instance: _state.data["instances"].as<JsonArray>()) {
                if (instance["ip"] == instanceUDP.remoteIP().toString()) {
                    found = true;
                    instance["name"] = buffer+6;
                    instance["time"] = time(nullptr);
                    // ESP_LOGD(TAG, "updated %s %s %lu", instance["ip"].as<String>().c_str(), instance["time"].as<String>().c_str(), time(nullptr));
                }
            }
            if (!found) {
                JsonObject instance = _state.data["instances"].as<JsonArray>().add<JsonObject>();
                instance["ip"] = instanceUDP.remoteIP().toString();
                instance["name"] = buffer+6;
                instance["time"] = time(nullptr);
                ESP_LOGD(TAG, "added %s %s", instance["ip"].as<String>().c_str(), buffer+6);
            }

            // char buffer[2048];
            // serializeJson(_state.data, buffer, sizeof(buffer));
            // ESP_LOGD(TAG, "data %s", buffer);
            if (!_socket->getConnectedClients()) return; 

            JsonObject instancesData = _state.data.as<JsonObject>();
            _socket->emitEvent("instances", instancesData);
        }
    }

    void writeUDP() {
        if (instanceUDP.beginPacket(IPAddress(255, 255, 255, 255), instanceUDPPort)) {
            
            UDPMessage message;
            message.name = _state.data["instanceName"].as<String>().c_str();
            instanceUDP.write((uint8_t *)&message, sizeof(message));
            instanceUDP.endPacket();
            // ESP_LOGD(TAG, "UDP packet written (%d)", WiFi.localIP()[3]);

        }
    }
};

#endif
#endif