# Virtual layers module

<img width="396" alt="image" src="https://github.com/user-attachments/assets/965dd352-d7af-42a1-a72d-43da3b32a252" />

The Virtual layers module allows you to define the tasks to run an **effect** (e.g. bouncing balls), to **modify** the effect (e.g mirror).
Each task is defined as a node. A node can be precompiled in the firmware or defined by a live script loaded onto the File System (See File Manager). A node can be switched on and off and has custom controls, which defines the parameters of the node (e.g. effect speed).

Ultimately the nodes will be displayed in a graphical interface where nodes are connected by 'noodles' to define dependencies between nodes. For the time being nodes will be shown in a list.

Typically a node will define a layout (🚥), or an effect (🔥), or a modifier (💎) but can also combine these tasks (experimental at the moment). To avoid duplication it's in most cases recommended to keep them separated so an effect can run on multiple layouts and a modifier can modify any effect. 

* **Effect** 🔥: An effect runs in a virtual layer (see above about mapping to a physical layer). Historically there are 1D, 2D and 3D effects. A 1D effect only fills LEDs in x space, leaving y and z blank. 2D also the y space, 3D all spaces. Future goal is that all effects fill all spaces (example is bouncing balls which was a 1D effect but has been made 2D). See also Modifiers which can take a 1D effect and make a 2D or 3D effect out of it: e.g. a 1D effect can be presented as a circle, or sphere.
    * An effect has a loop which is ran for each frame produced. In each loop, the lights in the virtual layer gets it's values using the setRGB function. For multichannel lights also functions as setWhite or (for Moving Heads) setPan, setTilt, setZoom etc. Also getRGB etc functions exists.
    * Multiple effects can be defined, they all run during one frame. In the future each effect will have a start and end coordinate so they can also be displayed on parts of the fixture.

* **Modifier** 💎: a modifier is an effect on an effect. It can change the size of the virtual layer, it can map the pixels other then 1:1 (e.g. mirror, multiply them or a 1D line can become a 2D circle) and it can change each light set in an effect during a loop (e.g. rotate the effect)
    * Multiple modifiers are allowed, e.g. to first rotate then mirror (or first mirror then rotate). The UI allows for reordering nodes.


See [Nodes](https://moonmodules.org/MoonLight/moonlight/nodes/) page for documentation on existing nodes