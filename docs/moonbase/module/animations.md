# Animations module

<img width="517" src="https://github.com/user-attachments/assets/88e00192-a029-43c7-974c-15d786b48c0e" />

## Functional

* On: lights on or off
* Brightness: brightness of the LEDs when on
* driverOn: sends LED output to ESP32 gpio pins.
    * Switch off to see the effect framerate in System Status/Metrics
    * Switch on to see the effect framerate throttled by a LED driver in System Status/Metrics (800KHz, 256 leds, 24 bits is 130 fps theoretically - 120 practically)
* Nodes: One or more processes, can be fixture definitions, mappings, effects, projections. Currently all nodes are effects.
    * Nodes can have arguments
* Scrips: Running Live scripts (WIP)

<img width="498" alt="Screenshot 2025-03-29 at 14 12 01" src="https://github.com/user-attachments/assets/3a5a3743-c0a4-4456-96cb-f4abd0d01450" />

## Technical

* See [Modules](../modules.md)
* Uses ESPLiveScripts, see compileAndRun. compileAndRun is started when in Nodes a file.sc animation is choosen
    * To do: kill running scripts, e.g. when changing effects
* To do: use Nodes arguments as arguments to scripts or hardcoded effects

### Mapping model (to do)

<img width="500" src="https://github.com/user-attachments/assets/6f76a2d6-fce1-4c72-9ade-ee5fbd056c88" />

* Multiple Nodes can be defined (1)
    * Each node can have properties (compare controls in WLED / StarLight)
    * Each node can run precompile code or Live scripts (with or without loop)
    * Each node has a type
        * Fixture definition: tell where each pixture is in a 1D/2D/3D physical coordinate space (based on StarLight)
        * Fixture mapping: change the fixture to a 1D/2D/3D virtual coordinate space
        * Effect: run an effect in (part of) the virtual coordinate space
        * Modifier: Mirror, rotate, etc, multiple projections allowed (projection in StarLight)
        * Driver show: show the result on Leds (using FastLED, hpwit drivers), ArtNet, DDP, ...
* Future situation: Nodes and noodles (2)
    * Replace the nodes table (1) by a graphical view (2)
* Mapping model (3)
    * Take the StarLight PhysMap as a start
        * 2-bytes to 3-bytes to allow for > 16384 leds
        * Mapping table is logical pixels
        * ledsP is physical pixels (as in FASTLED)
        * 1:0, 1:1, 1:many : each logical pixel can be mapped to no, 1 or many physical pixels)
    * Nodes manipulate the mapping model and/or interfere in the effects loop
* Presets/playlist: change (part of) the nodes model

### Server

[ModuleAnimations.h](https://github.com/ewowi/MoonBase/blob/main/src/custom/ModuleAnimations.h)

### UI

Generated
