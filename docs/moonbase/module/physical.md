# Physical layer module

🆕 (in release v0.5.7 Virtual and physical layers modules are still in one module called editor)

<img width="396" alt="image" src="https://github.com/user-attachments/assets/965dd352-d7af-42a1-a72d-43da3b32a252" />

The Physical layer module allows you to define a **layout** (e.g. a panel 16x16), to drive LEDs (e.g. FastLED driver) and to run **supporting processes** (e.g. audiosync to run sound reactive effects and Art-Net to send light channels to an Art-Net controller).

Each task is defined as a node. A node can be precompiled in the firmware or defined by a live script loaded onto the File System (See File Manager). A node can be switched on and off and has custom controls, which defines the parameters of the node (e.g. effect speed).

Ultimately the nodes will be displayed in a graphical interface where nodes are connected by 'noodles' to define dependencies between nodes. For the time being nodes will be shown in a list.

Typically a node will define a modifier (💎) or a supporting process (☸️) but can also combine these tasks (experimental at the moment). To avoid duplication it's in most cases recommended to keep them separated so an effect can run on multiple layouts and a modifier can modify any effect. 

* **Layout** 🚥: a layout defines what lights are connected to MoonLight. It defines the coordinates of all lights (addLight) and assigns lights to the GPIO pins of the ESP32 (addPin) and how many channels each light has (normal LEDs 3: Red, Green and Blue). 
    * The **coordinates** of each light are defined in a 3D coordinate space where each coordinate range between 1 and 255. Currently a strip until 255 LEDs is supported, a panel until 128x96 LEDS and a cube max 20x20x20. 
        * Coordinates needs to be specified in the order the lights are wired so MoonLight knows which light is first, which is second etc.
        * If a 1D strip is longer, you can address more LEDs by pretending it is a 2D fixture, e.g. 32x32 to address a strip of 1024 LEDs. 
        * In the future we might look at redefining Coord3D where width is 12 bytes, height is 7 bytes and depth is 5 bytes allowing for max 4096 x 128 x 32 ...
    * Currently **pins** are by default driven by FastLED.show. FastLED needs all LEDs specifications be defined at compile time, e.g. LED type, color order, etc. Normally also the pin(s) need to be defined beforehand but all pins are predefined in the code to avoid this (at a cost of larger firmware). As we want to be able to define all LED specs in the UI instead of during compile, we need to see how / if this can be done.
    * 🆕 **Multiple layout nodes** can be defined which will execute one after the other
    * In the future, instead of pins, also **IP addresses and universes** can be specified to support sending lights data to Art-Net devices - e.g. [Pknight Art-Net DMX 512](https://s.click.aliexpress.com/e/_ExQK8Dc) to control DMX lights or [Art-Net LED controller](https://s.click.aliexpress.com/e/_Ex9uaOk) to control LED strips or panels over local network. Currently this is handled by the [Art-Net Node](https://moonmodules.org/MoonLight/moonlight/nodes/#art-net/).
    * Alternatively Moonlight supports 2 other drivers: [Physical Driver](https://github.com/hpwit/I2SClocklessLedDriver) and [Virtual Driver](https://github.com/ewowi/I2SClocklessVirtualLedDriver). Firmware ending with PD or VD will have this enabled. Code has been included but not tested yet (tested in StarLight), this is planned for June/July. 
        * The physical driver is an alternative to the FastLED driver and does not need LED specifications defined at compile time, all can be controlled via UI. 
        * The virtual driver is another beast and with the help of shift registers allows for driving 48 panels of 256 LEDs each at 100 FPS!.
    * MoonLight will use the layout definition to generate a **mapping** of a virtual coordinate space to a physical coordinate space. Most simple example is a panel which has a snake layout. The mapping will create a virtual layer where the snake layout is hidden.

## Emoji coding:

* 🚥 Layout
* ☸️ Supporting node
* ♫ Audio reactive FFT based
* ♪ Audio reactive volume based

## Layout 🚥 Nodes
🚧

### PanelLayout 🚥 🆕

* Defines a 2D panel with width and height
* Controls:
    * width and height
    * orientation XY or YX: Vertical or horizontal
    * X++ and Y++: X++: starts at Top or bottom, Y++: starts left or right
    * snake: is the first axis in snake / serpentine layout?
    * pin: to do add more pins

### CubeLayout 🚥 🆕

* Panel layout + depth
* Controls:
    * width and height and depth
    * orientation XYZ or YXZ, XZY or YZX, ZXY or ZYX: Vertical or horizontal or depth
    * X++ and Y++ and Z++: Z++ starts front or back
    * snake X, Y, Z: multidimensional snaking, good luck 😜
    * pin: to do add more pins

### SE16 🚥

Layout(s) for Stephan Electronics 16-Pin ESP32-S3 board, using the pins used on the board

* ledsPerPin: the number of LEDs connected to one pin
* pinsAreColumns: are the LEDs on a pin a row of the effect (width is 1 (or 2) x ledsPerPin). If not set the LEDs are a column (height is 1 (or 2) x ledsPerPin)
* mirroredPins: If set it is assumed that LEDs are connected with increasing positions on 8 pins on one side of the board and decreasing positions on the 8 pins of the other side of the board. The resulting size will have a width of 8 and the height (or width) will be 2 * ledsPerPin. If not set, the width will be 16 and the height (or width) = ledsPerPin

## Supporting ☸️ Nodes
🚧

### FastLED Driver ☸️

sends LED output to ESP32 gpio pins.

* Switch off to see the effect framerate in System Status/Metrics
* Switch on to see the effect framerate throttled by a LED driver in System Status/Metrics (800KHz, 256 LEDs, 24 bits is 130 fps theoretically - 120 practically)
* Will move to driver node later ...

### Physical driver ☸️

Implemented but not tested

### Virtual driver ☸️

Not implemented yet

### Art-Net ☸️

This node sends the content of the Lights array in Art-Net compatible packages to an Art-Net controller specified by the IP address provided.

* Controller IP: The last segment of the IP address within your local network, of the the hardware Art-Net controller.
* FPS Limiter: set the max frames per second Art-Net packages are send out (also all the other nodes will run at this speed).

Example of compatible controllers can be found [here](https://moonmodules.org/hardware/). Both Art-Net LED controllers and Art-Net DMX controllers can be used as output.

The node supports this setup:
```cpp
    std::vector<uint16_t> hardware_outputs = {1024,1024,1024,1024,1024,1024,1024,1024};
    std::vector<uint16_t> hardware_outputs_universe_start = { 0,7,14,21,28,35,42,49 }; //7*170 = 1190 LEDs => last universe not completely used
```

Todo: 
* Add controls for other hardware_outputs
* MoonLight can also act as a receiving Art-Net controller, sending dmx commands (not implemented yet)

### AudioSync ☸️ ♫

* listens to audio sent over the local network by WLED-AC or WLED-MM and allows sound reactive effects (♫) to use audio data (volume and bands (FFT))
