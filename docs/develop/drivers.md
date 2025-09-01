# Drivers

## Overview

## Initless drivers

Applies to Art-Net (currently working like this), to be added to physical and virtual driver.

The main idea is that a context will be set and can dynamically change. Driver.show will use these variables to et the right data to the right output.

Initless means there is no addLeds (like in FastLed) or initLeds (like in physical and virtual driver)

The main advantage of this is:

* No need to recompile any changed configs (e.g. colorOrder is fully flexible, not a setting in platformio.ini)
* No need to restart while setting up a specific installation. Just change layouts until it works as intended.
* Allows for a flexible mix of different outputs e.g. send the first 1024 leds to physical driver, next to virtual driver, next to spi, next to one Art-Net device, next to another Art-Net device. 

### context

make sure the following is available before calling driver.show. Note: setup only sets variables the loop will use.

* channelsPerLed
* offsets within the channels (RGBW and more like pan and tilt)
* outputs[]: pins for leds drivers, outputs for Art-Net
* lengths[]: nr of lights per output
* driver[]: shows for each output for which driver it is ... 🚧 as currenlty all drivers process all lights which sometimes is what you want (monitor on leds what is on moving heads) but not always

### driver.show

Called by loop function.

* loop over all outputs (or pins) which are meant for a specific driver (e.g. all outputs for artnet)
    * pre channel actions (...)
    * loop over all lights in the output
        * for each light
            * pre light actions (e.g. setup dma ...)
            * copy all channels to the buffer (for art-net) or to the memory area / dma / parallelIO ... so the driver can process it
                * find them in the channels array (pka leds array but we now support any light: not only 3 or 4 channels)
            * correct RGBW offsets when present for brightness using LUT (__red_map etc in Yves his drivers)
            * send the buffer/memory (artnetudp.writeTo sends one universe, Yves drivers ..., Troy drivers ...)
            * post light actions (e.g. increase universe etc)
    * post channel actions (...)

### MoonLight layout nodes

In MoonLight you can define one or more layout nodes telling which position lights have in the real world. E.g. a strip, or a panel (orientation, snake, ...) or a ring or any fantasy shape.
Each layout nodes addLights and addPins (will be renamed to addOutPut). Layouts can be reordered. The pins and lights arrays will be filled with the Layout nodes in the order they are defined.

Note: Above concept can also be offered in a library and imported in other projects like WLED, FastLED scripts etc.