# Effects module

<img width="320" src="https://github.com/user-attachments/assets/571fd676-92f7-4ea7-a521-fc298490695c" />

## Overview

The Effects module defines effects and modifiers. They run in a 'layer'. 

* Effect 🔥: An animation running in a coordinate space, e.g. a graphical equalizer or game of life. Effects can run in 1D, 2D or 3D coordinate spaces.
* Modifier 💎: An effect on an effect e.g. mirror, multiply or rotate. See [Modifiers](https://moonmodules.org/MoonLight/modifiers/)
* Layer: An area on a (LED) display. Effects and modifier run in this area. Each layer maps a coordinate space to the display

!!! info

    Until v0.6.0 there is only one layer, projected on the whole display. Multiple layers is planned after v0.6.0

!!! info

    Traditionally effects where mostly 1D running on LED strips, when LED matrices became available 2D effects where created. More and more LED art installations have a 3D shape so also support for 3D is desirable. MoonLight is build from the ground to support 3D. Basic idea is that a 1D effect is 3D where y and z is 0, 2D has z is 0. This means modifiying an effect to run in 3D is possible. This can be done using modifiers, but also the source code of an effect can be changed to support more dimensions (see development).

!!! tip

    Livescripts... A node can be precompiled in the firmware or defined by a live script loaded onto the File System (See File Manager).

## Controls

* Layer: Choose the layer - currently only one layer supported
    * Start, End and Brightness: read only for now, when multiple layers are implemented (>v0.6.0), these can be set per layer
* Nodes: effects and modifiers will be shown as nodes with controls
    * Reorder: Nodes can be reordered, defining the order of executions (effects: which effect on top of the other effect. Modifiers: which modifier is done first, e.g. circle then multiply will yield to a different result than multiply then circle)
    * Controls. A node can be switched on and off and has custom controls, which defines the parameters of the node (e.g. effect speed).

## Emoji coding:

* 🔥 Effect
* 🚨 Moving head color effect
* 🗼 Moving head move effect
* 💎 Modifier
* 🎨 Using palette
* 💡 WLED origin
* 💫 MoonLight origin
* ♫ Audio reactive FFT based
* ♪ Audio reactive volume based
* 🧊 also works on 3D

## Effect 🔥 Nodes
🚧
Precompiled effects can be found in [effects](https://github.com/MoonModules/MoonLight/blob/main/src/MoonLight/Nodes/Effects/Effects.h)

### Solid 🔥💡

* Sends a solid RGB to all lights
* Controls: Red, Green, Blue, Brightness
* Usage: Can be used as background to other effects, order it before other effects

### Scrolling text 🔥

* 🚧: Currently shows system info on leds. You need at least 16*6 pixels for that (Panel 16x16 recommended). Later custom texts will be possible
* Controls:
    * Speed: scrolling speed if the text is wider then the display width
    * Font: Different font sizes
    * Preset: one of the following: Auto, IP (.ddd), FPS (ddds), Time (HHMM), Uptime (s/m/h), Status 🛜 (AP/STA), Clients 🛜 (dC), Connected 🛜 (dCC)

### PanTilt script

* Sends a beatsin to Pan and Tilt which can be sent to Moving Heads (add a Moving head layout node to configure the MHs)
* Controls: BPM, Middle Pan and Tilt, Range and invert
* Usage: Add this effect if moving heads are configured. RGB effects can be added separately e.g. wave to light up the moving heads in wave patterns
* See [E_PanTilt](https://github.com/MoonModules/MoonLight/blob/main/misc/livescripts/E_PanTilt.sc)
* Run script see [How to run a live script](https://moonmodules.org/MoonLight/moonbase/module/liveScripts/#how-to-run-a-live-script)

🚨: some effects already do this theirselves e.g. FreqMatrix runs on 1D but copies to 2D and 3D if size allows.

