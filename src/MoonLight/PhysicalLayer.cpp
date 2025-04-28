/**
    @title     MoonLight
    @file      PhysicalLayer.cpp
    @repo      https://github.com/ewowi/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/ewowi/MoonLight/commits/main
    @Doc       https://ewowi.github.io/MoonLight/general/utilities/
    @Copyright © 2025 Github MoonLight Commit Authors
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

    bool PhysicalLayer::setup() {
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

        //calculate physical size (pass1, pass2)

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
    Node* PhysicalLayer::addNode(const char * animation) {

        Node *node = nullptr;
        if (equal(animation, "Solid")) {
            node = new SolidEffect();
        } else if (equal(animation, "Random🔥")) {
            node = new RandomEffect();
        } else if (equal(animation, "Sinelon🔥")) {
            node = new SinelonEffect();
        } else if (equal(animation, "Rainbow🔥")) {
            node = new RainbowEffect();
        } else if (equal(animation, "Sinus🔥")) {
            node = new SinusEffect();
        } else if (equal(animation, "Lissajous🔥")) {
            node = new LissajousEffect();
        } else if (equal(animation, "Lines🔥")) {
            node = new LinesEffect();
        } else if (equal(animation, "Panel🚥")) {
            node = new Panel16fixture();
        } else if (equal(animation, "Multiply💎")) {
            node = new MultiplyModifier();
        } else if (equal(animation, "Mirror💎")) {
            node = new MirrorModifier();
        } else if (equal(animation, "Pinwheel💎")) {
            node = new PinwheelModifier();
        #if FT_LIVESCRIPT
            } else {
                node = new LiveScriptNode();
        #endif
        }

        if (node) {
            node->constructor(layerV[0], animation); //pass the layer to the node
            node->setup(); //run the setup of the effect
            layerV[0]->nodes.push_back(node); //add the node to the layer
        }

        ESP_LOGD(TAG, "%s (s:%d)", animation, layerV[0]->nodes.size());

        return node;
    }

    bool PhysicalLayer::removeNode(const char * animation) {
        //find node in layerV[0]
        for (size_t i = 0; i < layerV[0]->nodes.size(); i++) {
            // ESP_LOGD(TAG, "%d %d", i, layerV[0]->nodes.size());
            Node *node = layerV[0]->nodes[i];
            // ESP_LOGD(TAG, "%s %s", node->animation, animation);
            if (equal(node->animation, animation)) {
                node->destructor();
                delete node;
                layerV[0]->nodes.erase(layerV[0]->nodes.begin() + i);
                ESP_LOGD(TAG, "remove node %s (s:%d)", animation, layerV[0]->nodes.size());
                return true;
            }
        }
        return false;
    }

    // to be called in setup, if more then one effect
    void PhysicalLayer::initPixelsToBlend() {
        pixelsToBlend.reserve(nrOfLeds);

        for (uint16_t indexP = 0; indexP < pixelsToBlend.size(); indexP++)
          pixelsToBlend[indexP] = false;
    }
