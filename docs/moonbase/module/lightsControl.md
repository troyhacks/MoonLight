# Lights Control module

<img width="399" alt="image" src="https://github.com/user-attachments/assets/a79f12b6-9cd6-4d98-8d75-14f663a6da93" />

Controls:

* On: lights on or off
* Brightness: brightness of the LEDs when on
* RGB Sliders: control each color separately.
* Palette: Global palette setting. Effects with the palette icon 🎨 use this palette setting.
* Presets: Control pad style, store or retrieve a set of nodes with their controls. WIP
* Monitor On: sends LED output the monitor in the UI.

A MoonLight instance can expose itself as an Art-Net device (DMX mode) where above controls will be the channels to control by Art-Net controllers (it can be an Art-Net DMX receiver but also an Art-Net LEDs receiver, and it can be an Art-Net sender - See Art-Net Module) - WIP (only Art-Net sender is currently implemented)

Light Controls is the interface to control lights for the UI, but also for all protocols eg. HA, DMX, Hardware buttons, displays etc
e.g. a DMX controller, can control presets, but not individual preset details.
