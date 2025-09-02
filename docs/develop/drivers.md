# Drivers

## Overview

## Initless drivers

Initless means there is no addLeds (like in FastLed) or initLed (like in physical and virtual driver): 

* a Context (see below) will be set
* Driver.show (see below) will use this context to set the right data to the right output.
* The context can dynamically change without needing to reboot or recompile! e.g. changes in pin allocations, leds per pin, RGB or RGBW, or DMX lights like moving heads.

The Art-Net driver is currently working like this, to be added to physical and virtual driver and parallel IO (P4).

The advantages of dynamic context change are:

* No need to recompile any changed configs (e.g. colorOrder is fully flexible, not a setting in platformio.ini)
* No need to restart while setting up a specific installation. Just change layouts until it works as intended.
* Allows for a flexible mix of different outputs e.g. send the first 1024 leds to physical driver, next to virtual driver, next to spi, next to one Art-Net device, next to another Art-Net device. 

### Context

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

### Driver.show

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

### MoonLight layout nodes

In MoonLight you can define one or more layout nodes telling which position lights have in the real world. E.g. a strip, or a panel (orientation, snake, ...) or a ring or any fantasy shape.
Each layout nodes addLights and addPins (will be renamed to addOutPut). Layouts can be reordered. The pins and lights arrays will be filled with the Layout nodes in the order they are defined.

Note: Above concept can also be offered in a library and imported in other projects like WLED, FastLED scripts etc.

### Notes on the I2S clockless driver

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
