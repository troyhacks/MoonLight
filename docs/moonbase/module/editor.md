# Editor module

<img width="396" alt="image" src="https://github.com/user-attachments/assets/965dd352-d7af-42a1-a72d-43da3b32a252" />

The editor allows you to define the tasks to run an **effect**  (e.g. bouncing balls), to **modify** the effect (e.g mirror), to send it to a **layout** (e.g. a panel 16x16), to drive LEDs (e.g. FastLED driver) and to run **supporting processes** (e.g. audiosync to run sound reactive effects and Art-Net to send light channels to an Art-Net controller).
Each task is defined as a node. A node can be precompiled in the firmware or defined by a live script loaded onto the File System (See File Manager). A node can be switched on and off and has custom controls, which defines the parameters of the node (e.g. effect speed).

Ultimately the nodes will be displayed in a graphical interface where nodes are connected by 'noodles' to define dependencies between nodes. For the time being nodes will be shown in a list.

Typically a node will define a layout (🚥), or an effect (🔥), or a modifier (💎) or a supporting process (☸️) but can also combine these tasks (experimental at the moment). To avoid duplication it's in most cases recommended to keep them separated so an effect can run on multiple layouts and a modifier can modify any effect. 

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

* **Effect** 🔥: An effect runs in a virtual layer (see above about mapping to a physical layer). Historically there are 1D, 2D and 3D effects. A 1D effect only fills LEDs in x space, leaving y and z blank. 2D also the y space, 3D all spaces. Future goal is that all effects fill all spaces (example is bouncing balls which was a 1D effect but has been made 2D). See also Modifiers which can take a 1D effect and make a 2D or 3D effect out of it: e.g. a 1D effect can be presented as a circle, or sphere.
    * An effect has a loop which is ran for each frame produced. In each loop, the lights in the virtual layer gets it's values using the setRGB function. For multichannel lights also functions as setWhite or (for Moving Heads) setPan, setTilt, setZoom etc. Also getRGB etc functions exists.
    * Multiple effects can be defined, they all run during one frame. In the future each effect will have a start and end coordinate so they can also be displayed on parts of the fixture.

* **Modifier** 💎: a modifier is an effect on an effect. It can change the size of the virtual layer, it can map the pixels other then 1:1 (e.g. mirror, multiply them or a 1D line can become a 2D circle) and it can change each light set in an effect during a loop (e.g. rotate the effect)
    * Multiple modifiers are allowed, e.g. to first rotate then mirror (or first mirror then rotate). The UI allows for reordering nodes.


See [Nodes](https://moonmodules.org/MoonLight/moonlight/nodes/) page for documentation on existing nodes