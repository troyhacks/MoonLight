/**
    @title     MoonLight
    @file      PhysicalLayer.cpp
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/general/utilities/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
**/

#if FT_MOONLIGHT

#include "PhysicalLayer.h"

#include "VirtualLayer.h"

#include "Nodes.h"

PhysicalLayer::PhysicalLayer() {
        ESP_LOGD(TAG, "constructor");

        lights.header.type = ct_Leds;

        // initLightsToBlend();

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
        //runs the loop of all effects / nodes in the layer
        for (VirtualLayer * layer: layerV) {
            if (layer) layer->loop(); //if (layer) needed when deleting rows ...
        }
        return true;
    }
    
    void PhysicalLayer::addPin(uint8_t pinNr) {
        ESP_LOGD(TAG, "addPin %d", pinNr);
    }

    void PhysicalLayer::addLayoutPre() {
        lights.header.nrOfLights = 0; // for pass1 and pass2 as in pass2 virtual layer needs it
        ESP_LOGD(TAG, "pass %d %d", pass, lights.header.nrOfLights);

        if (pass == 1) {
            lights.header.size = {0,0,0};
            lights.header.type = ct_count; //in progress...
            delay(100); //wait to stop effects
            //dealloc pins
        } else {
            for (VirtualLayer * layer: layerV) {
                //add the lights in the virtual layer
                layer->addLayoutPre();
            }
        }
    }

    void PhysicalLayer::addLight(Coord3D position) {
        
        if (pass == 1) {
            // ESP_LOGD(TAG, "%d,%d,%d", position.x, position.y, position.z);
            if (lights.header.nrOfLights >= MAX_CHANNELS / sizeof(Coord3D)) {
                //send the positions to the UI _socket_emit
                //reset the index and continue...
            } else
                lights.positions[lights.header.nrOfLights] = {(uint16_t)position.x, (uint16_t)position.y, (uint16_t)position.z};

                lights.header.size = lights.header.size.maximum(position);
        } else {
            for (VirtualLayer * layer: layerV) {
                //add the position in the virtual layer
                layer->addLight(position);
            }
        }
        lights.header.nrOfLights++;
    }

    void PhysicalLayer::addLayoutPost() {
        if (pass == 1) {
            lights.header.size += Coord3D{1,1,1};
            ESP_LOGD(TAG, "pass %d #:%d s:%d,%d,%d (%d=%d+%d)", pass, lights.header.nrOfLights, lights.header.size.x, lights.header.size.y, lights.header.size.z, sizeof(Lights), sizeof(LightsHeader), sizeof(lights.leds));
            //send the positions to the UI _socket_emit
            lights.header.type = ct_Position; //filled with positions, set back to ct_Leds in Animations
        } else {
            ESP_LOGD(TAG, "pass %d %d", pass, lights.header.nrOfLights);
            for (VirtualLayer * layer: layerV) {
                //add the position in the virtual layer
                layer->addLayoutPost();
            }
            // initLightsToBlend();
        }

        //driver init // alloc pins
    }

    // an effect is using a virtual layer: tell the effect in which layer to run...


    //run one loop of an effect
    Node* PhysicalLayer::addNode(const char * animation, uint8_t index) {

        Node *node = nullptr;
        if (equal(animation, SolidEffect::name())) node = new SolidEffect();
        //alphabetically from here
        else if (equal(animation, BouncingBallsEffect::name())) node = new BouncingBallsEffect();
        else if (equal(animation, LinesEffect::name())) node = new LinesEffect();
        else if (equal(animation, LissajousEffect::name())) node = new LissajousEffect();
        else if (equal(animation, MovingHeadEffect::name())) node = new MovingHeadEffect();
        else if (equal(animation, RainbowEffect::name())) node = new RainbowEffect();
        else if (equal(animation, RandomEffect::name())) node = new RandomEffect();
        else if (equal(animation, RipplesEffect::name())) node = new RipplesEffect();
        else if (equal(animation, RGBWParEffect::name())) node = new RGBWParEffect();
        else if (equal(animation, SinelonEffect::name())) node = new SinelonEffect();
        else if (equal(animation, SinusEffect::name())) node = new SinusEffect();
        else if (equal(animation, SphereMoveEffect::name())) node = new SphereMoveEffect();
        else if (equal(animation, DMXLayout::name())) node = new DMXLayout();
        else if (equal(animation, PanelLayout::name())) node = new PanelLayout();
        else if (equal(animation, RingsLayout::name())) node = new RingsLayout();
        else if (equal(animation, MirrorModifier::name())) node = new MirrorModifier();
        else if (equal(animation, MultiplyModifier::name())) node = new MultiplyModifier();
        else if (equal(animation, PinwheelModifier::name())) node = new PinwheelModifier();
        #if FT_LIVESCRIPT
            else {
                LiveScriptNode *liveScriptNode = new LiveScriptNode();
                liveScriptNode->animation = animation; //set the (file)name of the script
                node = liveScriptNode;
        #endif
        }

        if (node) {
            node->constructor(layerV[0]); //pass the layer to the node
            node->setup(); //run the setup of the effect
            // layerV[0]->nodes.reserve(index+1);
            if (index >= layerV[0]->nodes.size())
                layerV[0]->nodes.push_back(node);
            else
                layerV[0]->nodes[index] = node; //add the node to the layer
        }

        ESP_LOGD(TAG, "%s (s:%d)", animation, layerV[0]->nodes.size());

        return node;
    }

    void PhysicalLayer::removeNode(Node *node) {
        ESP_LOGD(TAG, "remove node (s:%d)", layerV[0]->nodes.size());
        node->destructor();
        delete node;
        // layerV[0]->nodes[index] = nullptr;
    }

    // // to be called in setup, if more then one effect
    // void PhysicalLayer::initLightsToBlend() {
    //     lightsToBlend.reserve(lights.header.nrOfLights);

    //     for (uint16_t indexP = 0; indexP < lightsToBlend.size(); indexP++)
    //       lightsToBlend[indexP] = false;
    // }

#endif //FT_MOONLIGHT