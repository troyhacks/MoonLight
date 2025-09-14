# Nodes

## Creating new nodes

The nodes system is in principle a general mechanism. Currently it is implemented as a MoonLight feature, might move to a generic MoonBase feature in the future. Currently it is intertwined with MoonLight functionality supporting the Physical and Virtual layer model and supporting the 4 types of nodes: Layout, effect, modifier and supporting node.

The core node functionality supports the following

* setup() and loop()
* controls: each node has a variable number of flexible variables of different types (sliders/range, checkboxes, numbers etc). They are added with the setControl function in the setup()
    * Warning: type of controls
* Firmware or live scripts

MoonLight specific

* Node types: it is recommended that a node is one of the 4 types as described above. However each node could perform functionality of all types. To recognize what a node does the emojis 🚥, 🔥, 💎 and ☸️ are used in the name. The variables hasLayout and hasModifier indicate the specific functionality the node supports. They control when a physical to virtual mapping is recalculated
    * **hasLayout**: a layout node specify the amount of position of lights controlled. E.g. a panel of 16x16 or a cube of 20x20x20. If hasLayout is defined you should implement addLayout calling addLight(position) and addPin() for all the lights. 
      * addPin() is needed if a LED driver is used to send the output to LED strips.
    * **hasModifier**: a modifier node which manipulates virtual size and positions and lights using one or more of the functions modifySize, modifyPosition and modifyXYZ.
    * if the loop() function contains setXXX functions is used it is an **effect** node. It will contain for-loops iterating over each virtual ! light defined by layout and modifier nodes. The iteration will be on the x-axis for 1D effects, but also on the y- and z-axis for 2D and 3D effects. setRGB is the default function setting the RGB values of the light. If a light has more 'channels' (e.g. Moving heads) they also can be set. 
* Moving heads
    * **addLight** will show where the moving head will be on the stage. In general only an array of a few lights e.g. 4 moving heads in a row. A moving head effect will then iterate over 4 lights where each light might do something different (e.g. implement a wave of moving head movement)
    * You need to define **channelsPerLight** in the layout node setup() - (it is default 3 to support normal LEDs). Currently MoonLight only supports identical moving heads with the same channels. The first light starts at DMX 0 (+1), the second at DMX channelsPerLight (+1) the third on DMX 2*channelsPerLight (+1) and so on. (+1): DMX typically starts at address 1 while MoonLight internal starts with 0... 🚧. We are working on a solution to support different lights e.g a mix of 15 channel lights and 32 channel lights etc. You could set channelsPerLight to a higher number as the real lights channels, e.g. 32 so each lights DMX address starts at a multiple of 32.
    * **Layout**: The layout node also defines which functionality / channels the light support by defining **offsets**. Currently the following offsets are supported: offsetRGB, offsetWhite, offsetBrightness, offsetPan, offsetTilt, offsetZoom, offsetRotate, offsetGobo, offsetRGB1, offsetRGB2, offsetRGB3, offsetBrightness2 and need to be set in the setup() function.
    * The distinction between physical and virtual layer for moving heads is not useful if you have only 2-4 moving heads. However this is a standard MoonLight feature. It might become useful if you have like 8 (identical) moving heads, 4 left and 4 right of the stage, then you can add a mirror modifier and the virtual layer will only control 4 lights, which then will be mapped to 8 physical lights. In theory you can also have a cube of like 512 moving heads and then exotic modifiers like pinwheel could be used to really go crazy. Let us know when you have one of these setups 🚨
    * Moving heads will be controlled using the [ArtNed Node](https://moonmodules.org/MoonLight/moonlight/nodes/#art-net/). addPin is not needed for moving heads, although you might want to attach LEDs for a visual view of what is send to Art-Net.
    * Effect nodes **set light**: Currently setRGB, setWhite, setBrightness, setPan, setTilt, setZoom, setRotate, setGobo, setRGB1, setRGB2, setRGB3, setBrightness2 is supported. In the background MoonLight calculates which channel need to be filled with values using the offsets (using the setLight function).
    * If offsetBrightness is defined, the RGB values will not be corrected for brightness in [ArtNed](https://moonmodules.org/MoonLight/moonlight/nodes/#art-net/).

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
    
    * See [Modules](../modules.md)
    * Upon changing a pin, driver.init will rerun (FastLED.addLeds, PD and VD driver.init)
    * Uses ESPLiveScripts, see compileAndRun. compileAndRun is started when in Nodes a file.sc is choosen
        * To do: kill running scripts, e.g. when changing effects
    * [Nodes.h](https://github.com/MoonModules/MoonLight/blob/main/src/MoonLight/Nodes.cpp): class Node (constructor, destructor, setup, loop, hasFunctions, map, modify, addControl(s), updateControl)
    * [Nodes.cpp](https://github.com/MoonModules/MoonLight/blob/main/src/MoonLight/Nodes.cpp): implement LiveScriptNode
    
    ### Mapping model (🚧)
    
    <img width="500" src="https://github.com/user-attachments/assets/6f76a2d6-fce1-4c72-9ade-ee5fbd056c88" />
    
    * Multiple Nodes can be created (1)
        * Each node can have controls (compare controls in WLED / StarLight) ✅
        * Each node can run precompile code or Live scripts (with or without loop) ✅
        * Each node has a type:
            * Layout: tell where each light is in a 1D/2D/3D physical coordinate space (based on StarLight fixtures) ✅
            * Effect: 
                * run an effect in a virtual coordinate space ✅
                * in the physical space if you want to run at highest performance, e.g. a random effect doesn't need to go through mappings ✅
            * Modifier: Mirror, rotate, etc, multiple modfiers allowed (projection in StarLight) 🚧
                * A modifier can also map lights dimensions to effect dimensions: change the lights to a 1D/2D/3D virtual coordinate space
                    * e.g. if the light is a globe, you can map that to 2D using mercator projection mapping
                    * if the light is 200x200 you can map it to 50x50
                    * if the light is 2D, a 1D effect can be shown as a circle or a bar (as WLED expand1D)
            * Driver show: show the result on Leds (using FastLED, hpwit drivers), Art-Net, DDP, ...
    * Future situation: Nodes and noodles (2)
        * Replace the nodes table (1) by a graphical view (2)
    * Virtual Layer (MappingTable) (3)
        * Array of arrays. Outer array is virtual lights, inner array is physical lights. ✅
        * Implemented efficiently using the StarLight PhysMap struct ✅
        * e.g. [[],[0],[1,2],[3,4,5],[6,7,8,9]] ✅
            * first virtual light is not mapped to a physical light
            * second virtual light is mapped to physical light 0
            * third virtual light is mapped to physical lights 1 and 2
            * and so on
        * Virtual lights can be 1D, 2D or 3D. Physical lights also, in any combination
            * Using x + y * sizeX + z * sizeX * sizeY 🚧
        * set/getLightColor functions used in effects using the MappingTable ✅
        * Nodes manipulate the MappingTable and/or interfere in the effects loop 🚧
        * A Virtual Layer mapping gets updated if a layout, mapping or dimensions change 🚧
        * An effect uses a virtual layer. One Virtual layer can have multiple effects. ✅
    * Physical layer
        * Lights.header and Lights.channels. CRGB leds[] is using lights.channels (acting like leds[] in FASTLED) ✅
        * A Physical layer has one or more virtual layers and a virtual layer has one or more effects using it. ✅
    * Presets/playlist: change (part of) the nodes model
    
    ✅: Done
    
    ### Example

    ```json
    {
  "nodes": [
    {
      "nodeName": "Lissajous 🔥🎨💡",
      "on": true,
      "controls": [
        {
          "name": "xFrequency",
          "type": "range",
          "default": 64,
          "p": 1065414703,
          "value": 64
        },
        {
          "name": "fadeRate",
          "type": "range",
          "default": 128,
          "p": 1065414704,
          "value": 128
        },
        {
          "name": "speed",
          "type": "range",
          "default": 128,
          "p": 1065414705,
          "value": 128
        }
      ]
    },
    {
      "nodeName": "Random 🔥",
      "on": true,
      "controls": [
        {
          "name": "speed",
          "type": "range",
          "default": 128,
          "p": 1065405731,
          "value": 128
        }
      ]
    }
  ]
}
```