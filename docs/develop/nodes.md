# Nodes

## Creating new nodes

The nodes system is in principle a general mechanism. Currently it is implemented as a MoonLight feature, might move to a generic MoonBase feature in the future. Currently it is intertwined with MoonLight functionality supporting the Physical and Virtual layer model and supporting the 4 types of nodes: Layout, effect, modifier and supporting node.

The core node functionality supports the following

* setup() and loop()
* controls: each node has a variable number of flexible variables of different types (sliders/range, checkboxes, numbers etc). They are added with the setControl function in the setup()
    * Warning: type of controls
* Firmware or live scripts

Nodes are inspired by WLED usermods, further developed in StarBase and now in MoonLight (using the ESP32-Sveltekit infrastructure)

MoonLight specific

* Node types: it is recommended that a node is one of the 4 types as described above. However each node could perform functionality of all types. To recognize what a node does the emojis 🚥, 🔥, 💎 and ☸️ are used in the name. The function hasOnLayout() and hasModifier() indicate the specific functionality the node supports. They control when a physical to virtual mapping is recalculated
    * **hasOnLayout()**: a layout node specify the amount of position of lights controlled. E.g. a panel of 16x16 or a cube of 20x20x20. If hasOnLayout() is true you should implement onLayout calling addLight(position) and addPin() for all the lights. 
      * addPin() is needed if a LED driver is used to send the output to LED strips.
    * **hasModifier()**: a modifier node which manipulates virtual size and positions and lights using one or more of the functions modifySize, modifyPosition and modifyXYZ.
    * if the loop() function contains setXXX functions is used it is an **effect** node. It will contain for-loops iterating over each virtual ! light defined by layout and modifier nodes. The iteration will be on the x-axis for 1D effects, but also on the y- and z-axis for 2D and 3D effects. setRGB is the default function setting the RGB values of the light. If a light has more 'channels' (e.g. Moving heads) they also can be set. 
* Moving heads
    * **addLight** will show where the moving head will be on the stage. In general only an array of a few lights e.g. 4 moving heads in a row. A moving head effect will then iterate over 4 lights where each light might do something different (e.g. implement a wave of moving head movement)
    * You need to define **channelsPerLight** in the layout node setup() - (it is default 3 to support normal LEDs). Currently MoonLight only supports identical moving heads with the same channels. The first light starts at DMX 0 (+1), the second at DMX channelsPerLight (+1) the third on DMX 2*channelsPerLight (+1) and so on. (+1): DMX typically starts at address 1 while MoonLight internal starts with 0... 🚧. We are working on a solution to support different lights e.g a mix of 15 channel lights and 32 channel lights etc. You could set channelsPerLight to a higher number as the real lights channels, e.g. 32 so each lights DMX address starts at a multiple of 32.
    * **Layout**: The layout node also defines which functionality / channels the light support by defining **offsets**. Currently the following offsets are supported: offsetRGB, offsetWhite, offsetBrightness, offsetPan, offsetTilt, offsetZoom, offsetRotate, offsetGobo, offsetRGB1, offsetRGB2, offsetRGB3, offsetBrightness2 and need to be set in the setup() function.
    * The distinction between physical and virtual layer for moving heads is not useful if you have only 2-4 moving heads. However this is a standard MoonLight feature. It might become useful if you have like 8 (identical) moving heads, 4 left and 4 right of the stage, then you can add a mirror modifier and the virtual layer will only control 4 lights, which then will be mapped to 8 physical lights. In theory you can also have a cube of like 512 moving heads and then exotic modifiers like pinwheel could be used to really go crazy. Let us know when you have one of these setups 🚨
    * Moving heads will be controlled using the [ArtNed Node](https://moonmodules.org/MoonLight/moonlight/nodes/#art-net/). addPin is not needed for moving heads, although you might want to attach LEDs for a visual view of what is send to Art-Net.
    * Effect nodes **set light**: Currently setRGB, setWhite, setBrightness, setPan, setTilt, setZoom, setRotate, setGobo, setRGB1, setRGB2, setRGB3, setBrightness2 is supported. In the background MoonLight calculates which channel need to be filled with values using the offsets (using the setLight function).
    * If offsetBrightness is defined, the RGB values will not be corrected for brightness in [ArtNed](https://moonmodules.org/MoonLight/moonlight/nodes/#art-net/).

## Layout nodes

In MoonLight you can define one or more layout nodes telling which position lights have in the real world. E.g. a strip, or a panel (orientation, snake, ...) or a ring or any fantasy shape.
Each layout nodes addLights and addPins (will be renamed to addOutPut). Layouts can be reordered. Driver nodes will use these pins and the leds connected to it.

## Drivers

### Initless drivers

Initless means there is no addLeds (like in FastLed) or initLed (like in physical and virtual driver): 

* a Context (see below) will be set
* Driver.show (see below) will use this context to set the right data to the right output.
* The context can dynamically change without needing to reboot or recompile! e.g. changes in pin allocations, leds per pin, RGB or RGBW, or DMX lights like moving heads.

The Art-Net driver is currently working like this, to be added to physical and virtual driver and parallel IO (P4).

The advantages of dynamic context change are:

* No need to recompile any changed configs (e.g. colorOrder is fully flexible, not a setting in platformio.ini)
* No need to restart while setting up a specific installation. Just change layouts until it works as intended.
* Allows for a flexible mix of different outputs e.g. send the first 1024 leds to physical driver, next to virtual driver, next to spi, next to one Art-Net device, next to another Art-Net device. 

#### Context

Make sure the following is available before calling driver.show. Note: setup only sets variables the loop will use.

* channels[]: pka leds array (CRGB leds) but we now support any light: not only 3 or 4 channels
* channelsPerLed: a light in the channels array can be found by index * channelsPerLed.
* offsets within the channels (RGBW and more like pan and tilt)
* outputs[]: pins[] for leds drivers, outputs[] for Art-Net. Use generic name outputs[]
* lengths[]: nr of lights per output
* driver[]: shows for each output for which driver it is ... 🚧 as currenlty all drivers process all lights which sometimes is what you want (monitor on leds what is on moving heads) but not always

This is the current list of supported lights ranging from 3 channels per light (the classic rgb LEDs) to 32 channels per light. Currently pre defined: lights preset. In the future configurable

* RGB
* RBG
* GRB: default WS2812
* GBR
* BRG
* BGR
* RGBW: e.g. 4 channel par/dmx light
* GRBW: rgbw LED eg. SK6812
* GRB6: some LED curtains
* RGBWYP: 6 channel par/dmx light with UV etc
* MHBeeEyes150W-15 🐺: 15 channels moving head, see https://moonmodules.org/MoonLight/moonbase/module/drivers/#art-net
* MHBeTopper19x15W-32 🐺: 32 channels moving head
* MH19x15W-24: 24 channels moving heads

Based on the choosen value, the channels per light and the offsets will be set e.g. for GRB: header->channelsPerLight = 3; header->offsetRed = 1; header->offsetGreen = 0; header->offsetBlue = 2;. Drivers should not make this mapping, the code calling drivers should do.

The RGB and W offsets needs to be re-ordered and brightness corrected from the channel array. Not only Art-Net but  also a LEDs driver need to accept channel arrays with more then 4 channels per light. Eg GRB6 is a type of led curtain some of us have, which a Chinese manufacturer screwed up: 6 channels per light but only rgb is used, 3 channels do nothing

#### Driver.show

Called by loop function.

* loop over all outputs (or pins) which are meant for a specific driver (e.g. all outputs for artnet)
    * pre channel actions (...)
    * loop over all lights in the output
        * for each light
            * pre light actions (e.g. setup dma ...)
            * copy all channels to the buffer (for art-net) or to the memory area / dma / parallelIO ... so the driver can process it
                * find them in the channels array
            * correct RGBW offsets when present for brightness using LUT (__red_map etc in Yves his drivers)
            * send the buffer/memory (artnetudp.writeTo sends one universe, Yves drivers ..., Troy drivers ...)
            * post light actions (e.g. increase universe etc)
    * post channel actions (...)

For an example of the loop, see [Artnet loop](https://github.com/MoonModules/MoonLight/blob/fc6c4b6c52bf0a9e9a5de677c1ec5b6536fb7a16/src/MoonLight/Mods.cpp#L260-L293). Not splitten in output loop and lights loop yet (as now all drivers process all lights). Before and after the loop are pre /post output / light actions.

#### Notes on the I2S clockless driver

* nb_components -> channelsPerLed
* p_r, p_g, p_b, p_w -> offsetRed, Green, Blue, White
* stripSize[] -> lenghts[]

#### initLed

* setBrightness / setGamma -> done by MoonLight if brightness (or gamma) changes, driver.show expects the LUT to be set right
* setPins(Pinsq): called by MoonLight as soon as pins changes: add function driver.updatePins which also cleans up previous defined pins (I think there is no cleanup, so can be called repeatedly?)
* i2sInit(): not using context...: so this (and i2sReset...) can be called once in the setup...
    * i2sReset(): not using context...
    * i2sReset_DMA(): not using context...
    * i2sReset_FIFO(): not using context...
* initDMABuffers(): uses __NB_DMA_BUFFER, nb_components -> should be called by MoonLight as soon as dma buffer (would be very nice if this is a variable instead of a constant, so can be tweaked in MoonLight without recompiling) or lights preset (channelsPerLed) changes. Add function driver.updateLightsPreset(dma_buffer, channelsPerLight)...
    * putdefaultones: uses nb_components

#### showPixels

* loadAndTranspose(): uses this for all dma buffers. All Okay
    * loop over num_strips (outputs[].length)
    * loop over stripSize (lenghts[]), light by light: triggered by _I2SClocklessLedDriverinterruptHandler
        * fills secondPixel using LUT and RGB(W) offsets
        * transpose16x1_noinline2(secondpixel)
* i2sStart: uses dma buffers: Okay

## Archive

This page is 🚧, text below will be rewritten.

## Q&A

Collecting questions and answers (from [Discord](https://discord.com/channels/700041398778331156/1203994211301728296))
🚧 !!

* I'm therefore wondering if esplive only supports xtensa ASM and not RISCV? we're actually not supposed to enable esplivescript on anything else than the s3. For the moment it’s esp32 , esp32 s2 esp32s3. I am revamping the compiler for it to be more compact and less memory hungry. Once that is done I will work on creating risc assembly language so it can be used with the esp32 C family and also I want to do arm for the pi pico.
* Technically live scripts works on normal esp32, but MoonLight with live scripts is 103% flash size. I didn’t look into other partitioning (preferably keeping Ota working): note use esp32 16MB!!
* why do some animations allow me to specifiy the LED strip pin but not others?
* is there a wiki page I can read for me to understand pin mapping and LED strip physical format?  there are layout nodes 🚥 (defining how the lights layed out in a matrix, cube etc. There you can define pins) effect nodes 🔥 (obviously) and modifier nodes 💎(modify an effect)
* how does the lights control module interact with the animations module?
* what does "snake" mean for a moving head configuration?
* how do i specify which solid color i want? Added!
* getting a few "Software reset due to exception/panic" depending on the effects i set 😄 but that might be my PSU
* how I can specify the color order.... I see the define I want in fastled.h, i'm guessing it's passed somehow to the template through maybe the ML_CHIPSET define? oh.... it seems to require a modification of void LedsDriver::init
sorry for the spam.... it also looks to me that even with the ML_CHIPSET=SK6812 define there's no support for RGBW as on a small LED strip, setting a solid color of red for example, i see green & white - blue - red - green & white - blue - red (eg: it's forgetting to send 4x 8b per LED)
* is the monitor only available with the s3? I enabled "Monitor On" in the control tab oh that's interesting, the platform booted full white (when configured in solid animation) but went back to what was supposed to be when I started moving the control sliders. should be working on all platforms
* not sure if this has been flagged, but when changing a selected node type, the parameters of the previous node will stay displayed and when switching node types, I did manage to get LEDs frozen (impossible to get them ot update again)
it takes a platform reboot, and changing the movinghead number of LEDs in my case. it looks like some refresh isn't happening. even the platform reboot trick sometimes isn't happening... looking at the console output i'm seeing that my actions on the user interface aren't registered
* how complex would it be to map several LED strips on several IOs to a virtual 2d matrix? I'm currently looking at the code, more particularly void mapLayout() and it seems the layout is reset every time that function is called, so it is not possible to increase the display size
* [single line and row layouts](https://github.com/MoonModules/MoonLight/pull/19)
* personally i'd make 2 different menus in the esp32 GUI.. a layout is more something you set and forget about as it is tied to a physical install
* one layout to define all the lights is definitely a blocker for me (and I imagine more people) as physical installs have their own constraints. let me know what you think. I will add that possibility , I see ‘some’ usage, but I don’t understand why it is a blocker as in general the whole setup is a number of lights which you know in advance so then it is most clear to define that in one go ?


## Technical

* Nodes
    * See Nodes.h: class Node
        * name(): name of the node
        * hasFunctions: enables features of the Node (Layout, Modifier)
        * on: (de)activates the node
        * constructor: sets the corresponding layer
        * setup: if layout sets channelsPerLight and request map
    * Nodes manipulate the LEDs / channels array and the virtual to physical layer mappings.
    * specify which functions (layout, effect, modifier): One node in general implements one, but can also implement all three (e.g. Moving Head...  wip...)
        * layout
        * effect
        * modifier
    * Live scripts
        * See Nodes.h / nodes.cpp
    * Lights
        * Regular patterns (CRGB as default but also others like Moving Head ...)

* See [Modules](../modules)
* Upon changing a pin, driver.init will rerun (FastLED.addLeds, PD and VD driver.init)
* Uses ESPLiveScripts, see compileAndRun. compileAndRun is started when in Nodes a file.sc is choosen
    * To do: kill running scripts, e.g. when changing effects
* [Nodes.h](https://github.com/MoonModules/MoonLight/blob/main/src/MoonLight/Nodes.cpp): class Node (constructor, destructor, setup, loop, hasFunctions, map, modify, addControl(s), onUpdate)
* [Nodes.cpp](https://github.com/MoonModules/MoonLight/blob/main/src/MoonLight/Nodes.cpp): implement LiveScriptNode

    * An effect has a loop which is ran for each frame produced. In each loop, the lights in the virtual layer gets it's values using the setRGB function. For multichannel lights also functions as setWhite or (for Moving Heads) setPan, setTilt, setZoom etc. Also getRGB etc functions exists.