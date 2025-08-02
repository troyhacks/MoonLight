# Effects module

🆕 (in release v0.5.7 Effects and drivers modules are still combined in one module called editor)

<img width="396" alt="image" src="https://github.com/user-attachments/assets/965dd352-d7af-42a1-a72d-43da3b32a252" />

The Effects module allows you to define the tasks to run an **effect** (e.g. bouncing balls), to **modify** the effect (e.g mirror).
Each task is defined as a node. A node can be precompiled in the firmware or defined by a live script loaded onto the File System (See File Manager). A node can be switched on and off and has custom controls, which defines the parameters of the node (e.g. effect speed).

Ultimately the nodes will be displayed in a graphical interface where nodes are connected by 'noodles' to define dependencies between nodes. For the time being nodes will be shown in a list.

Typically a node will define a layout (🚥), or an effect (🔥), or a modifier (💎) but can also combine these tasks (experimental at the moment). To avoid duplication it's in most cases recommended to keep them separated so an effect can run on multiple layouts and a modifier can modify any effect. 

* **Effect** 🔥: An effect runs in a virtual layer (see above about mapping to a physical layer). Historically there are 1D, 2D and 3D effects. A 1D effect only fills LEDs in x space, leaving y and z blank. 2D also the y space, 3D all spaces. Future goal is that all effects fill all spaces (example is bouncing balls which was a 1D effect but has been made 2D). See also Modifiers which can take a 1D effect and make a 2D or 3D effect out of it: e.g. a 1D effect can be presented as a circle, or sphere.
    * An effect has a loop which is ran for each frame produced. In each loop, the lights in the virtual layer gets it's values using the setRGB function. For multichannel lights also functions as setWhite or (for Moving Heads) setPan, setTilt, setZoom etc. Also getRGB etc functions exists.
    * Multiple effects can be defined, they all run during one frame. In the future each effect will have a start and end coordinate so they can also be displayed on parts of the fixture.

* **Modifier** 💎: a modifier is an effect on an effect. It can change the size of the virtual layer, it can map the pixels other then 1:1 (e.g. mirror, multiply them or a 1D line can become a 2D circle) and it can change each light set in an effect during a loop (e.g. rotate the effect)
    * Multiple modifiers are allowed, e.g. to first rotate then mirror (or first mirror then rotate). The UI allows for reordering nodes.

## Emoji coding:

* 🔥 Effect
* 💎 Modifier
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

### Scrolling text 🔥 🆕

* WIP: Currently shows system info on leds. You need at least 16*6 pixels for that (Panel 16x16 recommended). Later custom texts will be possible
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

