# Lights Control module

<img width="399" src="https://github.com/user-attachments/assets/7f6406cf-b9b1-4f66-8b49-8b220d5370a5" />

Controls:

* On: lights on or off
* Brightness: brightness of the LEDs when on
* RGB Sliders: control each color separately.
* Palette: Global palette setting. Effects with the palette icon 🎨 use this palette setting.
* Presets: Store the current effects or retrieve earlier saved presets. 64 slots available:
    * Blue: Empty preset
    * Green: Saved preset 
        * Hover with the mouse to see effects and modifiers in the preset
        * Double click on a saved preset to delete the preset.
    * Red: Selected preset
    * Note: Save (💾) or cancel (🚫 effects first, before storing them as a preset!
* Monitor On: sends LED output to the monitor.

Light Controls is the interface to control lights for the UI, but also for all protocols eg. HA, DMX, Hardware buttons, displays etc
e.g. a DMX controller, can control presets, but not individual preset details.
