/**
    @title     MoonLight
    @file      PhysicalLayer.cpp
    @repo      https://github.com/ewowi/MoonBase, submit changes to this file as PRs
    @Authors   https://github.com/ewowi/MoonBase/commits/main
    @Doc       https://ewowi.github.io/MoonBase/general/utilities/
    @Copyright © 2025 Github MoonBase Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
**/

#include "PhysicalLayer.h"

#include "VirtualLayer.h"

#include "Nodes.h"

PhysicalLayer::PhysicalLayer() {
        ESP_LOGD(TAG, "constructor");
        initPixelsToBlend();

        //create one layer - temporary
        layerV.push_back(new VirtualLayer());
        layerV[0]->layerP = this;
    }

    //run one loop of an effect
    bool PhysicalLayer::setup() {
        //runs the loop of all effects / nodes in the layer, we need to fill the effects in the layer...
        for (VirtualLayer * layer: layerV) {
            layer->setup();
        }
        return true;
    }

    //run one loop of an effect
    bool PhysicalLayer::loop() {
        //runs the loop of all effects / nodes in the layer, we need to fill the effects in the layer...
        for (VirtualLayer * layer: layerV) {
            layer->loop();
        }
        return true;
    }
    
    void PhysicalLayer::addPin(uint8_t pinNr) {
        ESP_LOGD(TAG, "addPin %d", pinNr);
    }

    void PhysicalLayer::addPixelsPre() {
        ESP_LOGD(TAG, "addPixelsPre");
        indexP = 0;
        //init mappingtable
        //dealloc pins

        //correct ledsP if > 255 (in case of circles)
        // maxFactor = max(max(max(fixSize2.x, fixSize2.y), fixSize2.z) / 256.0, 1.0);

        // set ledsPExtended
        // ledsPExtended.type = 1; //definition, stop effects loop
        // ledsPExtended.ledFactor *= maxFactor;
    
        for (VirtualLayer * layer: layerV) {
            //add the pixel in the virtual layer
            layer->addPixelsPre();
        }

        //for each effect node
        //  set virtual size based on physical start and end, ... scaling ...
        //  modifiers can change the size...
    }

    void PhysicalLayer::addPixel(Coord3D pixel) {
        // ESP_LOGD(TAG, "addPixel %d,%d,%d", pixel.x, pixel.y, pixel.z);

        //add the pixel in ledsP (for preview)
        // ledsP[indexP].r = pixel.x / maxFactor;
        // ledsP[indexP].g = pixel.y / maxFactor;
        // ledsP[indexP].b = pixel.z / maxFactor;

        for (VirtualLayer * layer: layerV) {
            //add the pixel in the virtual layer
            layer->addPixel(pixel);
        }
        //for each effect node
        //  modifiers can change the pixel...
        //  indexV = XYZUnprojected(pixel); //using virtual size first...
        //  mappingTable[indexV].addIndexP(effect Node, indexP);

        indexP++;
    }

    void PhysicalLayer::addPixelsPost() {
        ESP_LOGD(TAG, "addPixelsPost %d", indexP);
        //calculate physical size (pass1, pass2)

        for (VirtualLayer * layer: layerV) {
            //add the pixel in the virtual layer
            layer->addPixelsPost();
        }

        initPixelsToBlend();
        //driver init // alloc pins
    }

    // an effect is using a virtual layer: tell the effect in which layer to run...


    //run one loop of an effect
    bool PhysicalLayer::addNode(const char * animation) {

        Node *node = nullptr;
        if (equal(animation, "Solid")) {
            node = new SolidEffect();
        } else if (equal(animation, "Random")) {
            node = new RandomEffect();
        } else if (equal(animation, "Sinelon")) {
            node = new SinelonEffect();
        } else if (equal(animation, "Rainbow")) {
            node = new RainbowEffect();
        } else if (equal(animation, "Sinus")) {
            node = new SinusEffect();
        } else if (equal(animation, "Lissajous")) {
            node = new LissajousEffect();
        } else if (equal(animation, "Lines")) {
            node = new LinesEffect();
        } else if (equal(animation, "Panel16")) {
            node = new Panel16fixture();
        } else if (equal(animation, "Multiply")) {
            node = new MultiplyProjection();
        } else {
            //Done by live script (Yves)
        }

        if (node) {
            node->layerV = layerV[0];
            node->setup(); //run the setup of the effect
            layerV[0]->nodes.push_back(node); //add a new effect
        }

        return node != nullptr;
    }

    // to be called in setup, if more then one effect
    void PhysicalLayer::initPixelsToBlend() {
        pixelsToBlend.reserve(nrOfLeds);

        for (uint16_t indexP = 0; indexP < pixelsToBlend.size(); indexP++)
          pixelsToBlend[indexP] = false;
    }
