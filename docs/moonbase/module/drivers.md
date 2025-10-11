# Drivers module

<img width="320" src="https://github.com/user-attachments/assets/587bfb07-2da1-41fe-978e-43e431fd9517" />

## Overview

The Drivers module defines layers and drivers.

* Layout 🚥: define a **layout** (e.g. a panel 16x16), of your LEDs or lights. See [Layouts](https://moonmodules.org/MoonLight/layouts/)
* Driver ☸️: A driver sends light data to output, e.g. gpio pins (e.g. FastLED, Virtual and Physical driver) or over the network (e.g. Art-Net) or receives (light) data (e.g. audiosync to run sound reactive effects).

!!! tip

    Livescripts... A node can be precompiled in the firmware or defined by a live script loaded onto the File System (See File Manager).

## Controls

* Nodes: layouts, drivers and utilities will be shown as nodes with controls
    * Reorder: Nodes can be reordered, defining the order of executions (layouts, which layout is first in the chain of lights. Drivers and utilities: no difference)
    * Controls. A node can be switched on and off and has custom controls, which defines the parameters of the node

## Emoji coding:

* ☸️ Driver
* ♫ Audio reactive FFT based
* ♪ Audio reactive volume based

## Driver ☸️ nodes

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

### AudioSync ☸️ ♫

* listens to audio sent over the local network by WLED-AC or WLED-MM and allows sound reactive effects (♫) to use audio data (volume and bands (FFT))
