# Effects module

<img width="320" src="https://github.com/user-attachments/assets/571fd676-92f7-4ea7-a521-fc298490695c" />

## Overview

The Effects module defines effects and modifiers. They run in a 'layer'. 

* Effect 🔥: An animation running in a layer, e.g. a graphical equalizer or game of life. Effects can run in 1D, 2D or 3D coordinate spaces.
* Modifier 💎: An effect on an effect e.g. mirror, multiply or rotate. See [Modifiers](https://moonmodules.org/MoonLight/modifiers/)
* Layer: An area on a (LED) display. Effects and modifier run in this area. Each layer maps a coordinate space to the display

!!! info "One layer"

    Until v0.6.0 there is only one layer, projected on the whole display. Multiple layers is planned after v0.6.0

!!! info "3D"

    MoonLight is build from the ground to support 3D. First there where LED strips, later LED matrices and now also 3D shapes like spheres or cubes, making it suitable for art-installations. 1D and 2D objects can be seen as 3D objects where some dimensions have a width of 1. Most effects are 1D or 2D, they will slowly be adapted to support also 3D, furthermore modifiers can be used to project 1D on 2D or 2D on 3D layouts.

## Controls

* Layer: Choose the layer - currently only one layer supported
    * Start, End and Brightness: read only for now, when multiple layers are implemented (>v0.6.0), these can be set per layer
* Effects and modifiers will be shown as nodes with controls
    * Reorder: Nodes can be reordered, defining the order of executions
        * effects: which effect on top of the other effect. 
        * Modifiers: which modifier is done first, e.g. circle then multiply will yield to a different result than multiply then circle
    * Controls. A node can be switched on and off and has custom controls, which defines the parameters of the node (e.g. effect speed).

## Effect 🔥 Nodes

Below is a list of effects in MoonLight

### MoonLight effects

These effects are first created in MoonLight

| Name | Preview | Controls | Remarks
| ---- | ----- | ---- | ---- |
| Lines | ![lines](https://github.com/user-attachments/assets/6ddd0d7b-bbb2-4b44-8858-c4f444268321) | | To test a layout |
| FreqSaws | | <img width="320" src="https://github.com/user-attachments/assets/78828181-c1b3-4d0f-85dc-356ced10fcd9"/> | |
| MoonMan | | | only on devices supporting M5 library, e.g. AtomS3R |
| Particles | | | |
| Rainbow | | | |
| Random | | | |
| Ripples | | | |
| Rubiks Cube | | | |
| Scrolling Text | | Speed: scrolling speed if the text is wider then the display width, Font: Different font sizes, Preset: one of the following: Auto, IP (.ddd), FPS (ddds), Time (HHMM), Uptime (s/m/h), Status 🛜 (AP/STA), Clients 🛜 (dC), Connected 🛜 (dCC) | Shows system info on leds. You need at least 16*6 pixels for that (Panel 16x16 recommended) |
| Sinus | | | |
| Sphere Move | | | |
| StarField | | | |
| Praxis | | | |
| Wave | | | |
| Fixed Rectangle | | | To test a layout |

### MoonModules effects

The followingg effects are created by the MoonModules team, mostly first in WLED-MM and ported to MoonLight

| Name | Preview | Controls | Remarks
| ---- | ----- | ---- | ---- |
| GEQ3D | | | |
| PaintBrush | | | |
| GameOfLife | ![](https://raw.githubusercontent.com/scottrbailey/WLED-Utils/master/gifs/FX_172.gif) | <img width="320" src="https://github.com/user-attachments/assets/18498c30-b249-4390-bfdf-084deedbfc49" /> | |


### WLED effects

The following effects originated from WLED, most have been enhanced in WLED-MM and ported to MoonLight

| Name | Preview | Controls | Remarks
| ---- | ----- | ---- | ---- |
| Solid | ![](https://raw.githubusercontent.com/scottrbailey/WLED-Utils/master/gifs/FX_000.gif) | <img width="320" src="https://github.com/user-attachments/assets/844bdbf1-b42b-4b5c-94f9-d1334bce2b10" /> | Sends a solid RGB to all lights. Can be used as background to other effects, order it before other effects |
| Bouncing Balls | ![](https://raw.githubusercontent.com/scottrbailey/WLED-Utils/master/gifs/FX_091.gif) | <img width="320" src="https://github.com/user-attachments/assets/30aa2614-d891-4d39-a789-72026f7ab036" /> | |
| Blurz | ![](https://raw.githubusercontent.com/scottrbailey/WLED-Utils/master/gifs/FX_MM163.gif) | <img width="320" src="https://github.com/user-attachments/assets/b8f4111d-1f78-4ac0-a0f9-17909ad78599" /> | |
| Distortion Waves | ![](https://raw.githubusercontent.com/scottrbailey/WLED-Utils/master/gifs/FX_124.gif) | |
| FreqMatrix | ![](https://raw.githubusercontent.com/scottrbailey/WLED-Utils/master/gifs/FX_138.gif) | |
| GEQ | ![](https://raw.githubusercontent.com/scottrbailey/WLED-Utils/master/gifs/FX_MM139.gif) | <img width="320" src="https://github.com/user-attachments/assets/ca485d1e-e6e4-4b4a-910f-eed7fde7a21f" /> | |
| Lissajous | ![](https://raw.githubusercontent.com/scottrbailey/WLED-Utils/master/gifs/FX_MM176.gif) | <img width="320" src="https://github.com/user-attachments/assets/a28c19a7-012a-49bf-aff1-6b414c9d08dd" /> | |
| Noise 2D | ![](https://raw.githubusercontent.com/scottrbailey/WLED-Utils/master/gifs/FX_146.gif) | | |
| Noise Meter | ![](https://raw.githubusercontent.com/scottrbailey/WLED-Utils/master/gifs/FX_136.gif) | | |
| PopCorn | ![](https://raw.githubusercontent.com/scottrbailey/WLED-Utils/master/gifs/FX_095.gif) | | |
| Waverly | ![](https://raw.githubusercontent.com/scottrbailey/WLED-Utils/master/gifs/FX_MM165.gif) | | |

### Moving head effects

The following effects are created in MoonLight, supporting driving Moving Heads over Art-Net.

| Name | Preview | Controls | Remarks
| ---- | ----- | ---- | ---- |
| Troy1 Color | | | |
| Troy1 Move | | | |
| Troy2 Color | | | |
| Troy2 Move | | | |
| FreqColors | | | |
| Wowi Move | | | |
| Ambient Move | | | |

🚨: some effects already do this theirselves e.g. FreqMatrix runs on 1D but copies to 2D and 3D if size allows.

