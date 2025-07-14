# Nodes

## Emoji coding:

* 🔥 Effect
* 🚥 Layout
* 💎 Modifier
* ☸️ Supporting node
* 🎨 Using palette
* 💡 WLED origin
* 💫 MoonLight origin
* ♫ Audio reactive FFT based
* ♪ Audio reactive volume based
* 🧊 3D

## Effect 🔥 Nodes
🚧
Precompiled effects can be found in [effects](https://github.com/MoonModules/MoonLight/blob/main/src/MoonLight/Effects.h)

### Solid 🔥💡

* Sends a solid RGB to all lights
* Controls: Red, Green, Blue, Brightness
* Usage: Can be used as background to other effects, order it before other effects

### PanTilt script

* Sends a beatsin to Pan and Tilt which can be sent to Moving Heads (add a Moving head layout node to configure the MHs)
* Controls: BPM, Middle Pan and Tilt, Range and invert
* Usage: Add this effect if moving heads are configured. RGB effects can be added separately e.g. wave to light up the moving heads in wave patterns
* See [E_PanTilt](https://github.com/MoonModules/MoonLight/blob/main/misc/livescripts/E_PanTilt.sc)
* Run script see [How to run a live script](https://moonmodules.org/MoonLight/moonbase/module/liveScripts/#how-to-run-a-live-script)

## Layout 🚥 Nodes
🚧
### SE16 🚥

Layout(s) for Stephan Electronics 16-Pin ESP32-S3 board, using the pins used on the board

* ledsPerPin: the number of LEDs connected to one pin
* pinsAreColumns: are the leds on a pin a row of the effect (width is 1 (or 2) x ledsPerPin). If not set the leds are a column (height is 1 (or 2) x ledsPerPin)
* mirroredPins: If set it is assumed that LEDs are connected with increasing positions on 8 pins on one side of the board and decreasing positions on the 8 pins of the other side of the board. The resulting size will have a width of 8 and the height (or width) will be 2 * ledsPerPin. If not set, the width will be 16 and the height (or width) = ledsPerPin

## Modifier 💎 Nodes
🚧

### Pinwheel 💎💡

Projects 1D/2D effects onto 2D/3D fixtures in a pinwheel pattern.

* **Swirl** option to bend the pinwheel.
* **Reverse** option.
* **Rotation Symmetry**: Controls the rotational symmetry of the pattern.
* **Petals** option to adjust the number of petals.
* **Ztwist** option for 3D fixtures to twist the pattern along the z-axis.
* The virtual layer width is the number of petals. The height is the distance from center to corner

### RippleYZ 💎💡💫

Takes lights of an effect and copies them to other lights. E.g. 1D effect will be rippled to 2D, 2D effect will be rippled to 3D

* shrink: shrinks the original size to towardsY and Z
* towardsY: copies X into Y
* towardsZ: copies XY into Z

Note: some effects already do this theirselves e.g. FreqMatrix runs on 1D but copies to 2D and 3D if size allows.

## Supporting ☸️ Nodes
🚧

### FastLED Driver ☸️

sends LED output to ESP32 gpio pins.

* Switch off to see the effect framerate in System Status/Metrics
* Switch on to see the effect framerate throttled by a LED driver in System Status/Metrics (800KHz, 256 leds, 24 bits is 130 fps theoretically - 120 practically)
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
    std::vector<uint16_t> hardware_outputs_universe_start = { 0,7,14,21,28,35,42,49 }; //7*170 = 1190 leds => last universe not completely used
```

Todo: 
* Add controls for other hardware_outputs
* MoonLight can also act as a receiving Art-Net controller, sending dmx commands (not implemented yet)

### AudioSync ☸️ ♫

* listens to audio sent over the local network by WLED-AC or WLED-MM and allows sound reactive effects (♫) to use audio data (volume and bands (FFT))
