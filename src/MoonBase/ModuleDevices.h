/**
    @title     MoonBase
    @file      ModuleDevices.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/moonbase/module/devices/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact us for more information.
**/

#ifndef ModuleDevices_h
#define ModuleDevices_h

#if FT_MOONBASE == 1

#include "Module.h"

#include "Utilities.h"

struct UDPMessage {
    uint8_t rommel[6];
    Char<32> name;
};

class ModuleDevices : public Module
{
public:

    WiFiUDP deviceUDP;
    uint16_t deviceUDPPort = 65506;
    bool deviceUDPConnected = false;

    ModuleDevices(PsychicHttpServer *server,
            ESP32SvelteKit *sveltekit
        ) : Module("devices", server, sveltekit) {
            ESP_LOGV(TAG, "constructor");
    }

    void setupDefinition(JsonArray root) override{
        ESP_LOGV(TAG, "");
        JsonObject property; // state.data has one or more properties
        JsonArray details; // if a property is an array, this is the details of the array
        JsonArray values; // if a property is a select, this is the values of the select

        Char<32> deviceName;
        deviceName = "MoonLight-";
        uint8_t mac[6];
        esp_read_mac(mac, ESP_MAC_WIFI_STA);
        char macStr[5] = {0};
        sprintf(macStr, "%02x%02x", mac[4], mac[5]);
        // deviceName += WiFi.macAddress().substring(12);//localIP().toString().c_str();
        deviceName += macStr;
        property = root.add<JsonObject>(); property["name"] = "deviceName"; property["type"] = "text"; property["default"] = deviceName.c_str();

        property = root.add<JsonObject>(); property["name"] = "devices"; property["type"] = "array"; details = property["n"].to<JsonArray>();
        {
            property = details.add<JsonObject>(); property["name"] = "name"; property["type"] = "text"; property["ro"] = true;
            property = details.add<JsonObject>(); property["name"] = "ip"; property["type"] = "ip"; property["ro"] = true;
            property = details.add<JsonObject>(); property["name"] = "time"; property["type"] = "time"; property["ro"] = true;
        }
    }

    void onUpdate(UpdatedItem &updatedItem) override
    {
        ESP_LOGV(TAG, "no handle for %s[%d]%s[%d].%s = %s -> %s", updatedItem.parent[0].c_str(), updatedItem.index[0], updatedItem.parent[1].c_str(), updatedItem.index[1], updatedItem.name.c_str(), updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
    }

    void loop1s() {
        if (!_socket->getConnectedClients()) return; 

        if (!deviceUDPConnected) return;

        readUDP();
    }

    void loop10s() {
        if (!deviceUDPConnected) {
            deviceUDPConnected = deviceUDP.begin(deviceUDPPort);
            ESP_LOGV(TAG, "deviceUDPConnected %d i:%d p:%d", deviceUDPConnected, deviceUDP.remoteIP()[3], deviceUDPPort);
        }

        if (!deviceUDPConnected) return;

        writeUDP();
    }

    void readUDP() {
        size_t packetSize = deviceUDP.parsePacket();
        if (packetSize > 0) {
            char buffer[packetSize];
            deviceUDP.read(buffer, packetSize);
            // ESP_LOGV(TAG, "UDP packet read from %d: %s (%d)", deviceUDP.remoteIP()[3], buffer+6, packetSize);

            bool found = false;

            //use state.data or newData?
            for (JsonObject device: _state.data["devices"].as<JsonArray>()) {
                if (device["ip"] == deviceUDP.remoteIP().toString()) {
                    found = true;
                    device["name"] = buffer+6;
                    device["time"] = time(nullptr);
                    // ESP_LOGV(TAG, "updated %s %s %lu", device["ip"].as<String>().c_str(), device["time"].as<String>().c_str(), time(nullptr));
                }
            }
            if (!found) {
                JsonObject device = _state.data["devices"].as<JsonArray>().add<JsonObject>();
                device["ip"] = deviceUDP.remoteIP().toString();
                device["name"] = buffer+6;
                device["time"] = time(nullptr);
                ESP_LOGV(TAG, "added %s %s", device["ip"].as<String>().c_str(), buffer+6);
            }

            // char buffer[2048];
            // serializeJson(_state.data, buffer, sizeof(buffer));
            // ESP_LOGV(TAG, "data %s", buffer);
            if (!_socket->getConnectedClients()) return; 

            JsonObject data = _state.data.as<JsonObject>();
            _socket->emitEvent("devices", data);
        }
    }

    void writeUDP() {
        if (deviceUDP.beginPacket(IPAddress(255, 255, 255, 255), deviceUDPPort)) {
            
            UDPMessage message;
            message.name = _state.data["deviceName"].as<String>().c_str();
            deviceUDP.write((uint8_t *)&message, sizeof(message));
            deviceUDP.endPacket();
            // ESP_LOGV(TAG, "UDP packet written (%d)", WiFi.localIP()[3]);

        }
    }
};

#endif
#endif