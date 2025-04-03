# Animations module

<img width="517" alt="image" src="https://github.com/user-attachments/assets/022fde4d-9a3b-456c-ade5-e18219e5a5d5" />

## Functional

* On: lights on or off
* Brightness: brightness of the LEDs when on
* driverOn: sends LED output to ESP32 gpio pins.
    * Switch off to see the effect framerate in System Status/Metrics
    * Switch on to see the effect framerate throttled by a LED driver in System Status/Metrics (800KHz, 256 leds, 24 bits is 130 fps theoretically - 120 practically)
* Nodes: One or more processes, can be fixture definitions, mappings, effects, projections. Currently all nodes are effects.
* Scrips: Running Live scripts (WIP)

<img width="498" alt="Screenshot 2025-03-29 at 14 12 01" src="https://github.com/user-attachments/assets/3a5a3743-c0a4-4456-96cb-f4abd0d01450" />


## Technical

* See [Modules](modules.md)

### Server

[ModuleAnimations.h](https://github.com/ewowi/MoonBase/blob/main/src/custom/ModuleAnimations.h)

### UI

Generated
