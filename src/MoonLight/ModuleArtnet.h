/**
    @title     MoonBase
    @file      ModuleArtnet.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/modules/module/demo/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
**/

#ifndef ModuleArtnet_h
#define ModuleArtnet_h

#if FT_MOONBASE == 1

#include "../MoonBase/Module.h"
#include <AsyncUDP.h>

#define ARTNET_DEFAULT_PORT 6454

const size_t ART_NET_HEADER_SIZE = 12;
const uint8_t   ART_NET_HEADER[] PROGMEM = {0x41,0x72,0x74,0x2d,0x4e,0x65,0x74,0x00,0x00,0x50,0x00,0x0e};

class ModuleArtnet : public Module
{
public:

    IPAddress controllerIP; //tbd: controllerIP also configurable from fixtures and artnet instead of pin output
    std::vector<uint16_t> hardware_outputs = {1024,1024,1024,1024,1024,1024,1024,1024};
    std::vector<uint16_t> hardware_outputs_universe_start = { 0,7,14,21,28,35,42,49 }; //7*170 = 1190 leds => last universe not completely used
    size_t sequenceNumber = 0;

    ModuleArtnet(PsychicHttpServer *server,
            ESP32SvelteKit *sveltekit,
            FilesService *filesService
        ) : Module("artnet", server, sveltekit, filesService) {
            ESP_LOGD(TAG, "constructor");
    }

    void setupDefinition(JsonArray root) override{
        ESP_LOGD(TAG, "");
        JsonObject property; // state.data has one or more properties
        JsonArray details; // if a property is an array, this is the details of the array
        JsonArray values; // if a property is a select, this is the values of the select

        property = root.add<JsonObject>(); property["name"] = "on"; property["type"] = "checkbox"; property["default"] = false;
        property = root.add<JsonObject>(); property["name"] = "controllerIP"; property["type"] = "number"; property["min"] = 2; property["max"] = 255; property["default"] = 11;

        property = root.add<JsonObject>(); property["name"] = "outputs"; property["type"] = "array"; details = property["n"].to<JsonArray>();
        {
            property = details.add<JsonObject>(); property["name"] = "start"; property["type"] = "number"; property["default"] = 0; property["min"] = 0; property["max"] = 32;
            property = details.add<JsonObject>(); property["name"] = "size"; property["type"] = "number"; property["default"] = 1024; property["min"] = 0; property["max"] = 65536;
        }

        // {"hostName":"MoonBase","connectionMode":"Priority","savedNetworks":[],"invoices":[{"number":1000,"name":"ewowi","date":"2025-03-21", "lines":[{"service":"con", "price":124}]}]}

        // char buffer[256];
        // serializeJson(root, buffer, sizeof(buffer));
        // ESP_LOGD(TAG, "definition %s", buffer);
    }

    void onUpdate(UpdatedItem &updatedItem) override
    {
        if (updatedItem.name == "controllerIP") {
            controllerIP[3] = updatedItem.value.as<uint8_t>();
            ESP_LOGD(TAG, "controllerIP = %s", controllerIP.toString().c_str());
        }
        else if (updatedItem.parent[0] == "outputs") { // onNodes
            JsonVariant outputs = _state.data["outputs"][updatedItem.index[0]];

            if (updatedItem.name == "start") { //onStart
                ESP_LOGD(TAG, "Start[%d] = %d", updatedItem.index[0], updatedItem.value.as<uint8_t>());
                hardware_outputs_universe_start[updatedItem.index[0]] = updatedItem.value.as<uint8_t>();
            }
            if (updatedItem.name == "size") { //onStart
                ESP_LOGD(TAG, "Size[%d] = %d", updatedItem.index[0], updatedItem.value.as<uint16_t>());
                hardware_outputs[updatedItem.index[0]] = updatedItem.value.as<uint16_t>();
            }
        }
        // else
        //     ESP_LOGD(TAG, "no handle for %s[%d]%s[%d].%s = %s -> %s", updatedItem.parent[0], updatedItem.index[0], updatedItem.parent[1], updatedItem.index[1], updatedItem.name, updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
    }

void loop20ms() {

    // if(!mdls->isConnected) return;

    if (!_state.data["on"]) return;

    controllerIP[0] = WiFi.localIP()[0];
    controllerIP[1] = WiFi.localIP()[1];
    controllerIP[2] = WiFi.localIP()[2];

    if(!controllerIP) return;

    // if(!eff->newFrame) return;

    uint8_t bri = layerP.lights.header.brightness;

    // calculate the number of UDP packets we need to send

    uint8_t packet_buffer[ART_NET_HEADER_SIZE + 6 + 512];
    memcpy(packet_buffer, ART_NET_HEADER, 12); // copy in the Art-Net header.

    AsyncUDP artnetudp;// AsyncUDP so we can just blast packets.

    uint_fast16_t bufferOffset = 0;
    uint_fast16_t hardware_output_universe = 0;
    
    sequenceNumber++;

    if (sequenceNumber == 0) sequenceNumber = 1; // just in case, as 0 is considered "Sequence not in use"
    if (sequenceNumber > 255) sequenceNumber = 1;
    
    for (uint_fast16_t hardware_output = 0; hardware_output < hardware_outputs.size(); hardware_output++) { //loop over all outputs
        
        if (bufferOffset > layerP.lights.header.nrOfLights * layerP.lights.header.channelsPerLight) {
            // This stop is reached if we don't have enough pixels for the defined Art-Net output.
            return; // stop when we hit end of LEDs
        }

        hardware_output_universe = hardware_outputs_universe_start[hardware_output];

        uint_fast16_t channels_remaining = hardware_outputs[hardware_output] * layerP.lights.header.channelsPerLight;

        while (channels_remaining > 0) {
            const uint_fast16_t ARTNET_CHANNELS_PER_PACKET = 510; // 512/4=128 RGBW LEDs, 510/3=170 RGB LEDs

            uint_fast16_t packetSize = ARTNET_CHANNELS_PER_PACKET;

            if (channels_remaining < ARTNET_CHANNELS_PER_PACKET) {
                packetSize = channels_remaining;
                channels_remaining = 0;
            } else {
                channels_remaining -= packetSize;
            }

            // set the parts of the Art-Net packet header that change:
            packet_buffer[12] = sequenceNumber;
            packet_buffer[14] = hardware_output_universe;
            packet_buffer[16] = packetSize >> 8;
            packet_buffer[17] = packetSize;

            // bulk copy the buffer range to the packet buffer after the header 
            memcpy(packet_buffer+18, (&layerP.lights.channels[0])+bufferOffset, packetSize); //start from the first byte of ledsP[0]

            //no brightness scaling for the time being
            for (int i = 18; i < packetSize+18; i+= layerP.lights.header.channelsPerLight) {
                // set brightness all at once - seems slightly faster than scale8()?
                // for some reason, doing 3/4 at a time is 200 micros faster than 1 at a time.
                
                //to do: only correct light channels !!!
                if (layerP.lights.header.offsetBrightness == UINT8_MAX)
                    for (int j=0; j < 3; j++) packet_buffer[i+j+layerP.lights.header.offsetRGB] = (packet_buffer[i+j+layerP.lights.header.offsetRGB] * bri) >> 8;
                //if no brightness control and other colors, apply bri
                if (layerP.lights.header.offsetRGB1 != UINT8_MAX && layerP.lights.header.offsetBrightness == UINT8_MAX)
                    for (int j=0; j < 3; j++) packet_buffer[i+j+layerP.lights.header.offsetRGB1] = (packet_buffer[i+j+layerP.lights.header.offsetRGB1] * bri) >> 8;
                if (layerP.lights.header.offsetRGB2 != UINT8_MAX && layerP.lights.header.offsetBrightness == UINT8_MAX)
                    for (int j=0; j < 3; j++) packet_buffer[i+j+layerP.lights.header.offsetRGB2] = (packet_buffer[i+j+layerP.lights.header.offsetRGB2] * bri) >> 8;
                if (layerP.lights.header.offsetRGB3 != UINT8_MAX && layerP.lights.header.offsetBrightness == UINT8_MAX)
                    for (int j=0; j < 3; j++) packet_buffer[i+j+layerP.lights.header.offsetRGB3] = (packet_buffer[i+j+layerP.lights.header.offsetRGB3] * bri) >> 8;
                if (layerP.lights.header.offsetWhite != UINT8_MAX && layerP.lights.header.offsetBrightness == UINT8_MAX)
                    packet_buffer[i+layerP.lights.header.offsetWhite] = (packet_buffer[i+layerP.lights.header.offsetWhite] * bri) >> 8;
            }

            bufferOffset += packetSize;
            
            if (!artnetudp.writeTo(packet_buffer, packetSize+18, controllerIP, ARTNET_DEFAULT_PORT)) {
                Serial.print("🐛");
                return; // borked
            }

            hardware_output_universe++;
        }
    }
  }   //loop20ms
};

#endif
#endif