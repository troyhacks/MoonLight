
# MoonLight info

<img width="320" src="https://github.com/user-attachments/assets/cdc7ec72-0c50-4369-ad8d-f1fd0900e074" />

Shows info about the physical and virtual layer

* **NrOfLights**: the nr of lights defined in the layer
* **Channels per light**: e.g. normal RGB strip/panel is 3 channels per light
* **Max channels**: Max channels availeble (61440x3 for boards with PSRAM (120 pins x 512 LEDs), 4096x3 for other boards -> 12288 / 4096 RGB LEDs)
* **Size**: the outer bounds of the fixture, e.g. for a 16x16 panel it is 16x16x1
* **Nodes**: the total number of effects, modifiers, layouts and drivers
* **Layers**: The virtual layers defined (currently only 1)
    * **NrOfLights and size**: virtual layer can differ from the physical layer (.e.g when mirroring it is only half)
    * **Mapping table#**: nr of entries in the mapping table, is the same is nr of virtual pixels
    * **nrOfZeroLights**: the number of lights which are not mapped to a physical pixel (color is stored in the mapping table)
    * **nrOfOneLight**: The number of lights which have a 1:1 mapping between physical and virtual (if no modifier all is phys)
    * **Mapping table indexes**: The number of physical lights which are in a 1:many mapping
    * **nrOfMoreLights**: the number of virtual lights which are in a 1:many mapping
    * **Nodes#**: The number of nodes assigned to a virtual layer (currently all)
