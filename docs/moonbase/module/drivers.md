# Drivers module

<img width="320" src="https://github.com/user-attachments/assets/587bfb07-2da1-41fe-978e-43e431fd9517" />

## Overview

The Drivers module allows you to define a **layout** (e.g. a panel 16x16), to drive LEDs (e.g. FastLED driver) and to run **supporting processes** (e.g. audiosync to run sound reactive effects and Art-Net to send light channels to an Art-Net controller).

!!! tip

    Livescripts... A node can be precompiled in the firmware or defined by a live script loaded onto the File System (See File Manager).

## Controls

* Nodes: layouts, drivers and utilities will be shown as nodes with controls
    * Reorder: Nodes can be reordered, defining the order of executions (layouts, which layout is first in the chain of lights. Drivers and utilities: no difference)
    * Controls. A node can be switched on and off and has custom controls, which defines the parameters of the node

## Details

* **Layout** 🚥: a layout defines what lights are connected to MoonLight. It defines the coordinates of all lights (addLight) and assigns lights to the GPIO pins of the ESP32 (addPin) and how many channels each light has (normal LEDs 3: Red, Green and Blue). 
    * The **coordinates** of each light are defined in a 3D coordinate space where each coordinate range between 1 and 255. Currently a strip until 255 LEDs is supported, a panel until 128x96 LEDS and a cube max 20x20x20. 
        * Coordinates needs to be specified in the order the lights are wired so MoonLight knows which light is first, which is second etc.
        * If a 1D strip is longer, you can address more LEDs by pretending it is a 2D fixture, e.g. 32x32 to address a strip of 1024 LEDs. 
        * In the future we might look at redefining Coord3D where width is 12 bytes, height is 7 bytes and depth is 5 bytes allowing for max 4096 x 128 x 32 ...
    * Currently **pins** are by default driven by FastLED.show. FastLED needs all LEDs specifications be defined at compile time, e.g. LED type, color order, etc. Normally also the pin(s) need to be defined beforehand but all pins are predefined in the code to avoid this (at a cost of larger firmware). As we want to be able to define all LED specs in the UI instead of during compile, we need to see how / if this can be done.
    * **Multiple layout nodes** can be defined which will execute one after the other
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
!!! danger

    Set pins carefully. There is a check if a specific pin can be used for output / driving LEDs. See also the IO module. But not 100% garanteed. E.g pin 16 on esp32-d0-wrover crashes...

### PanelLayout 🚥

* Defines a 2D panel with width and height
* Controls:
    * width and height
    * orientation XY or YX: Vertical or horizontal
    * X++ and Y++: X++: starts at Top or bottom, Y++: starts left or right
    * snake: is the first axis in snake / serpentine layout?
    * pin: to do add more pins

### CubeLayout 🚥

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

### Art-Net ☸️

This node sends the content of the Lights array in Art-Net compatible packages to an Art-Net controller specified by the IP address provided.
The following devices have been tested and are recommended:

* Driving DMX fixtures: [PKNight ArtNet2-CR021R](https://s.click.aliexpress.com/e/_ExRrKe4): 2 universes, max 512 channels per universe. Used to drive the Light Presets for DMX lights / moving heads (see below)
* Driving LEDs: [Club Lights 12 Pro Artnet Controller - CL12P](https://s.click.aliexpress.com/e/_Ex9uaOk): 12 output leds controller, max 8 universes per channel. Max 512 channels per universe. Select IC-Type UCS2903

Controls:

* **Max Power**: max amount of power in watts to send to LEDs. Default 10: 5V * 2A = 10W (so it runs fine on USB)
* **Light preset**: Defines the channels per light and color order
    * RGB to BGR for 3 lights per channel, RGB lights, GRB is default
    * GRBW for LEDs with white channel like SK6812.
    * RGBW for Par/DMX Lights
    * GRB6 for LED curtains with 6 channels per light (only rgb used)
    * RGBWYP: Compatible with [DMX 18x12W LED RGBW/RGBWUAV](https://s.click.aliexpress.com/e/_EJQoRlM) (RGBW is 4x18=72 channels, RGBWUAV is 6x18=104 channels). 🚧: currently setup to have the first 36 lights 4 channel RGBW, after that 6 channel RGBWYP ! Used for 18 channel light bars
    * MH* for Moving Heads lights
        * MHBeTopper19x15W-32: [BeTopper / Big Dipper](https://betopperdj.com/products/betopper-19x15w-rgbw-with-light-strip-effect-moving-head-light)
        * MHBeeEyes150W-15: [Bee eyes](https://a.co/d/bkTY4DX)
        * MH19x15W-24: [19x15W Zoom Wash Lights RGBW Beam Moving Head](https://s.click.aliexpress.com/e/_EwBfFYw)
    * 🚨: Currently, if using multiple drivers, all drivers need the same Light preset !!
* **Controller IP**: The last segment of the IP address within your local network, of the the hardware Art-Net controller.
* **Port**: The network port added to the IP address, 6454 is the default for Art-Net.
* **FPS Limiter**: set the max frames per second Art-Net packages are send out (also all the other nodes will run at this speed).
    * Art-Net specs recommend about 44 FPS but higher framerates will work mostly (up to until ~130FPS tested)
* **Nr of outputs**: Art-Net LED controllers can have more then 1 output (e.g. 12)
    * CR021R: max 2 outputs
    * CL12P: Max 12 outputs
* **Universes per output**: How many universes can each output handle. This determines the maximum number of lights an output can drive (nr of universe x nr of channels per universe / channels per light)
    * CR021R: 1 universe
    * CL12P: Max 8 universes. 🚨: Set the number also on the controller! 
* **Channels per ouput**: each output can drive a maximum number of channels, determined by the universes per output
    * CR021R: 512 channels
    * CL12P: max 8 * 512. Each color in a LED is one channel: For RGB max 170 LEDs is 510 channels per universe, for RGBW max 128 LEDs per universe is 512 channels per universe => max 1360 RGB LEDs and 1024 RGBW LEDs per output. 🚨: Set channels per universe to 510 for RGB and 512 for RGBW (no proof yet it makes a difference ...) on the controller. The real number of channels per output can be less then the amount of universes available. e.g. if each output drives one 256 LED RGB panel, channels per output is 768. One package (= one universe) sends 170 LEDs (510 channels) and the second 86 LEDs / 256 channels. The next package for the next panel on the next output will then be in the first universe for that output (so unused universes for a channel will be skipped)

🚨: Dmx channels count from 1 to 512. At the moment we start counting from 0..511.

### FastLED Driver ☸️

sends LED output to ESP32 gpio pins.

* Switch off to see the effect framerate in System Status/Metrics
* Switch on to see the effect framerate throttled by a LED driver in System Status/Metrics (800KHz, 256 LEDs, 24 bits is 130 fps theoretically - 120 practically)
* **Chipset**: FastLED chipset defined (for FastLED hardcoded in the firmware ...)
* **FastLED**: FastLED version used (301001 is 3.10.1)
* **FastLED I2S**: Is the I2S driver used: Used on ESP32-S3 boards (temporary, will move to Physical driver), other boards use RMT5
* **Color order**: FastLED color order (for FastLED hardcoded in the firmware ...), will not work on I2S, see [FastLED issue 1966](https://github.com/FastLED/FastLED/issues/1966)

### Physical driver ☸️

Sends led signals to the pins / outputs as defined in the layout nodes. This is an alternative to the FastLED driver and uses I2S as the protocol / perephiral to drive the pins.

* **Max Power**and **Light preset**: See Art-Net driver. 
    * 🚨: Currently, if using multiple drivers, all drivers need the same Light preset !!
    * 🚨: Only the first 4 channels of the DMX type light presets will be shown (🚧)

### Virtual driver ☸️ 🚧

Not implemented yet

### HUB75 driver ☸️ 🚧

Not implemented yet

### AudioSync ☸️ ♫

* listens to audio sent over the local network by WLED-AC or WLED-MM and allows sound reactive effects (♫) to use audio data (volume and bands (FFT))
