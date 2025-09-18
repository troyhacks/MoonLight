/**
    @title     MoonLight
    @file      PhysicalLayer.cpp
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/moonlight/overview/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact us for more information.
**/

#if FT_MOONLIGHT

#include "PhysicalLayer.h"

#include "VirtualLayer.h"

#include "Nodes.h"

#include <ESP32SvelteKit.h> //for safeModeMB

#include "../MoonBase/Utilities.h"

PhysicalLayer::PhysicalLayer() {
        MB_LOGD(ML_TAG, "constructor");

        if (psramFound()) {
            lights.nrOfChannels = MIN(ESP.getPsramSize() / 2, 61440*3); //fill halve with channels, max 120 pins * 512 LEDs, still addressable with uint16_t
            // lights.channels = (uint8_t *)heap_caps_malloc(lights.nrOfChannels, MALLOC_CAP_SPIRAM);
            lights.channels = (uint8_t *)heap_caps_malloc_prefer(lights.nrOfChannels, 2, MALLOC_CAP_SPIRAM, MALLOC_CAP_DEFAULT);
            if (lights.channels)
                MB_LOGD(ML_TAG, "allocated %d bytes in %s", lights.nrOfChannels, isInPSRAM(lights.channels)?"PSRAM":"RAM" );
        }
        if (!lights.channels) {
            lights.nrOfChannels = 4096 * 3;
            lights.channels = (uint8_t *)heap_caps_malloc_prefer(lights.nrOfChannels, 2, MALLOC_CAP_DEFAULT, MALLOC_CAP_8BIT);//heap_caps_malloc(lights.nrOfChannels, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
            if (lights.channels)
                MB_LOGD(ML_TAG, "allocated %d bytes of RAM", lights.nrOfChannels );
        }
        if (!lights.channels) {
            MB_LOGD(ML_TAG, "failed to allocated RAM or PSRAM" );
            lights.nrOfChannels = 0;
        }

        //for some reason this says 
        // E (1431) 🌙: [Utilities.h:332] allocMB: heap_caps_malloc of 184320 x 1 not succeeded
        // E (1431) 💫: [PhysicalLayer.cpp:55] PhysicalLayer: failed to allocated RAM or PSRAM for 184320 channels
        // if (psramFound())
        //     lights.nrOfChannels = MIN(ESP.getPsramSize() / 2, 61440*3); //fill halve with channels, max 120 pins * 512 LEDs, still addressable with uint16_t
        // else
        //     lights.nrOfChannels = 4096 * 3;

        // lights.channels = allocMB<uint8_t>(lights.nrOfChannels); //including  memset(lights.channels, 0, lights.nrOfChannels); // set all the channels to 0
            
        // if (!lights.channels)
        //     MB_LOGE(ML_TAG, "failed to allocated RAM or PSRAM for %d channels", lights.nrOfChannels);
        //     lights.nrOfChannels = 0;
        // }

        // initLightsToBlend();

        //create one layer - temporary
        layerV.push_back(new VirtualLayer());
        layerV[0]->layerP = this;
    }

    void PhysicalLayer::setup() {
        for (VirtualLayer * layer: layerV) {
            layer->setup();
        }
    }
    
    void PhysicalLayer::loop() {
        if (lights.header.isPositions == 0 || lights.header.isPositions == 3) {//otherwise lights is used for positions etc.

            //runs the loop of all effects / nodes in the layer
            for (VirtualLayer * layer: layerV) {
                if (layer) layer->loop(); //if (layer) needed when deleting rows ...
            }

            if (requestMapPhysical) {
                MB_LOGD(ML_TAG, "mapLayout physical requested");
                
                pass = 1;
                mapLayout();

                requestMapPhysical = false;
            }

            if (requestMapVirtual) {
                MB_LOGD(ML_TAG, "mapLayout virtual requested");

                pass = 2;
                mapLayout();
                
                requestMapVirtual = false;
            }    

            if (lights.header.isPositions == 3) {
                MB_LOGD(ML_TAG, "pos from 3 to 0");
                lights.header.isPositions = 0; //now driver can show again
            }
        }
    }

    void PhysicalLayer::loopDrivers() {
        if (lights.header.isPositions == 0) { //otherwise lights is used for positions etc.
            if (prevSize != lights.header.size)
                MB_LOGD(ML_TAG, "onSizeChanged P %d,%d,%d -> %d,%d,%d", prevSize.x, prevSize.y, prevSize.z, lights.header.size.x, lights.header.size.y, lights.header.size.z);
            for (Node *node: nodes) {
                if (prevSize != lights.header.size)
                    node->onSizeChanged(prevSize);
                if (node->on)
                    node->loop();
            }
            prevSize = lights.header.size;
        }
    }

    void PhysicalLayer::mapLayout() {
        addLayoutPre();
        for (Node *node: nodes) {
            if (node->on && node->hasLayout) {
                node->addLayout();
            }
        }
        addLayoutPost();
    }

    void PhysicalLayer::addLayoutPre() {
        MB_LOGD(ML_TAG, "pass %d", pass);
        
        if (pass == 1) {
            lights.header.nrOfLights = 0; // for pass1 and pass2 as in pass2 virtual layer needs it
            lights.header.size = {0,0,0};
            MB_LOGD(ML_TAG, "pos from x(%d) to 1", lights.header.isPositions);
            lights.header.isPositions = 1; //in progress...
            delay(100); //wait to stop effects
            //set all channels to 0 (e.g for multichannel to not activate unused channels, e.g. fancy modes on MHs)
            memset(lights.channels, 0, lights.nrOfChannels); // set all the channels to 0
            //dealloc pins
            sortedPins.clear(); //clear the added pins for the next pass
        } else if (pass == 2) {
            indexP = 0;
            for (VirtualLayer * layer: layerV) {
                //add the lights in the virtual layer
                layer->addLayoutPre();
            }
        }
    }

    // void packCoord3DInto3Bytes(uint8_t *buf, Coord3D position) {
    //     uint32_t packed = ((position.x & 0x7FF) << 13) | ((position.y & 0xFF) << 5) | (position.z & 0x1F);
    //     buf[0] = (packed >> 16) & 0xFF;
    //     buf[1] = (packed >> 8) & 0xFF;
    //     buf[2] = packed & 0xFF;
    // }
    // void packCoord3DInto3Bytes(uint8_t *buf, Coord3D position) {
    //     // Clamp values to fit in the original bit ranges
    //     uint32_t x_clamped = position.x & 0x7FF;  // 11 bits: 0 to 2047
    //     uint32_t y_clamped = position.y & 0xFF;   // 8 bits: 0 to 255  
    //     uint32_t z_clamped = position.z & 0x1F;   // 5 bits: 0 to 31
        
    //     uint32_t packed = (x_clamped << 13) | (y_clamped << 5) | z_clamped;
    //     buf[0] = (packed >> 16) & 0xFF;
    //     buf[1] = (packed >> 8) & 0xFF;
    //     buf[2] = packed & 0xFF;
    // }

    // Mode configuration constants
    #define MODE0_X_MAX 127    // 2^7 - 1
    #define MODE0_Y_MAX 127    // 2^7 - 1
    #define MODE0_Z_MAX 255    // 2^8 - 1

    #define MODE1_X_MAX 511    // 2^9 - 1
    #define MODE1_Y_MAX 1      // 2^1 - 1
    #define MODE1_Z_MAX 4095   // 2^12 - 1

    #define MODE2_X_MAX 1      // 2^1 - 1
    #define MODE2_Y_MAX 511    // 2^9 - 1
    #define MODE2_Z_MAX 4095   // 2^12 - 1

    #define MODE3_X_MAX 31     // 2^5 - 1
    #define MODE3_Y_MAX 31     // 2^5 - 1
    #define MODE3_Z_MAX 31     // 2^5 - 1

    void packCoord3DInto3Bytes(uint8_t *buf, int x, int y, int z) {
        uint32_t packed = 0;
        uint8_t mode = 0;
        
        // Determine the best mode based on value ranges
        // Mode 0: x=7, y=7, z=8  (max: 127, 127, 255) - for 128x128x1
        // Mode 1: x=9, y=1, z=12 (max: 511, 1, 4095) - for 300x1x1 and 1x1x300
        // Mode 2: x=1, y=9, z=12 (max: 1, 511, 4095) - for 1x300x1
        // Mode 3: x=5, y=5, z=5  (max: 31, 31, 31) - for 30x30x30
        
        if (x <= MODE3_X_MAX && y <= MODE3_Y_MAX && z <= MODE3_Z_MAX) {
            // Mode 3: x=5, y=5, z=5 (max: 31, 31, 31) - for 30x30x30
            mode = 3;
            packed = (mode << 22) | ((x & 0x1F) << 17) | ((y & 0x1F) << 12) | ((z & 0x1F) << 7);
        }
        else if (x <= MODE2_X_MAX && y <= MODE2_Y_MAX) {
            // Mode 2: x=1, y=9, z=12 (max: 1, 511, 4095) - for 1x300x1
            mode = 2;
            packed = (mode << 22) | ((x & 0x1) << 21) | ((y & 0x1FF) << 12) | (z & 0xFFF);
        }
        else if (y <= MODE1_Y_MAX) {
            // Mode 1: x=9, y=1, z=12 (max: 511, 1, 4095) - for 300x1x1 and 1x1x300
            mode = 1;
            packed = (mode << 22) | ((x & 0x1FF) << 13) | ((y & 0x1) << 12) | (z & 0xFFF);
        }
        else {
            // Mode 0: x=7, y=7, z=8 (max: 127, 127, 255) - for 128x128x1
            mode = 0;
            packed = (mode << 22) | ((x & 0x7F) << 15) | ((y & 0x7F) << 8) | (z & 0xFF);
        }
        
        buf[0] = (packed >> 16) & 0xFF;
        buf[1] = (packed >> 8) & 0xFF;
        buf[2] = packed & 0xFF;
    }

    void PhysicalLayer::addLight(Coord3D position) {

        if (safeModeMB && lights.header.nrOfLights > 1023) {
            // MB_LOGW(ML_TAG, "Safe mode enabled, not adding lights > 1023");
            return;
        }

        if (pass == 1) {
            // MB_LOGD(ML_TAG, "%d,%d,%d", position.x, position.y, position.z);
            if (lights.header.nrOfLights < lights.nrOfChannels / 3) {
                packCoord3DInto3Bytes(&lights.channels[lights.header.nrOfLights*3], position.x, position.y, position.z);
            }
             
            lights.header.size = lights.header.size.maximum(position);
            lights.header.nrOfLights++;
        } else {
            for (VirtualLayer * layer: layerV) {
                //add the position in the virtual layer
                layer->addLight(position);
            }
            indexP++;
        }
    }

    void PhysicalLayer::addPin(uint8_t pinNr) {
        if (pass == 1) {
            MB_LOGD(ML_TAG, "addPin %d %d", pinNr, lights.header.nrOfLights);

            SortedPin previousPin;
            if (!sortedPins.empty()) {
                previousPin = sortedPins.back(); //get the last added pin
                if (previousPin.pin == pinNr) { //if the same pin, just increase the number of LEDs
                    previousPin.nrOfLights = lights.header.nrOfLights - previousPin.startLed;
                    sortedPins.back() = previousPin; //update the last added pin
                    return; //no need to add a new pin
                }
            } else {
                previousPin.startLed = 0; //first pin, start at 0
                previousPin.nrOfLights = 0;
            }

            //0 400
            //400 400
            //800 400

            SortedPin sortedPin;
            sortedPin.startLed = previousPin.startLed + previousPin.nrOfLights; //start at the end of the previous pin
            sortedPin.nrOfLights = lights.header.nrOfLights - sortedPin.startLed;
            sortedPin.pin = pinNr; //the pin number

            sortedPins.push_back(sortedPin);
        }
    }

    void PhysicalLayer::addLayoutPost() {
        if (pass == 1) {
            lights.header.size += Coord3D{1,1,1};
            MB_LOGD(ML_TAG, "pass %d #:%d s:%d,%d,%d", pass, lights.header.nrOfLights, lights.header.size.x, lights.header.size.y, lights.header.size.z);
            //send the positions to the UI _socket_emit
            MB_LOGD(ML_TAG, "pos from x(%d) to %d", lights.header.isPositions, lights.header.nrOfLights?2:3);
            lights.header.isPositions = lights.header.nrOfLights?2:3; //filled with positions, set back to 3 in ModuleEffects, or direct to 3 if no lights (effects will move it to 0)

            // initLightsToBlend();

            // if pins defined
            if (!sortedPins.empty()) {

                //sort the vector by the starLed
                std::sort(sortedPins.begin(), sortedPins.end(), [](const SortedPin &a, const SortedPin &b) {return a.startLed < b.startLed;});

                // ledsDriver.init(lights, sortedPins); //init the driver with the sorted pins and lights

            } //pins defined
        } else if (pass == 2) {
            MB_LOGD(ML_TAG, "pass %d %d", pass, indexP);
            for (VirtualLayer * layer: layerV) {
                //add the position in the virtual layer
                layer->addLayoutPost();
            }
        }
    }
    // an effect is using a virtual layer: tell the effect in which layer to run...

    // // to be called in setup, if more then one effect
    // void PhysicalLayer::initLightsToBlend() {
    //     lightsToBlend.reserve(lights.header.nrOfLights);

    //     for (uint16_t indexP = 0; indexP < lightsToBlend.size(); indexP++)
    //       lightsToBlend[indexP] = false;
    // }

#endif //FT_MOONLIGHT