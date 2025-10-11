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

## Effect 🔥 Nodes
🚧
Precompiled effects can be found in [effects](https://github.com/MoonModules/MoonLight/blob/main/src/MoonLight/Nodes/Effects/Effects.h)

### MoonLight effects

| Name | Preview | Controls | Remarks
| ---- | ----- | ---- | ---- |
| Lines | | | |
| MoonMan | | | if USE_M5UNIFIED |
| Particles | | | |
| Rainbow | | | |
| Random | | | |
| Ripples | | | |
| RubiksCube | | | |
| ScrollingText | | Speed: scrolling speed if the text is wider then the display width, Font: Different font sizes, Preset: one of the following: Auto, IP (.ddd), FPS (ddds), Time (HHMM), Uptime (s/m/h), Status 🛜 (AP/STA), Clients 🛜 (dC), Connected 🛜 (dCC) | Shows system info on leds. You need at least 16*6 pixels for that (Panel 16x16 recommended) |
| Sinus | | | |
| SphereMove | | | |
| StarField | | | |
| Praxis | | | |
| Wave | | | |
| FixedRectangle | | | |

### MoonModules effects

| Name | Preview | Controls | Remarks
| ---- | ----- | ---- | ---- |
| GEQ3D | | | |
| PaintBrush | | | |
| GameOfLife | ![](https://raw.githubusercontent.com/scottrbailey/WLED-Utils/master/gifs/FX_172.gif) | <img width="320" src="https://github.com/user-attachments/assets/18498c30-b249-4390-bfdf-084deedbfc49" /> | |


### WLED effects

| Name | Preview | Controls | Remarks
| ---- | ----- | ---- | ---- |
| Solid | ![](https://raw.githubusercontent.com/scottrbailey/WLED-Utils/master/gifs/FX_000.gif) | <img width="320" src="https://github.com/user-attachments/assets/844bdbf1-b42b-4b5c-94f9-d1334bce2b10" /> | Sends a solid RGB to all lights. Can be used as background to other effects, order it before other effects |
| BouncingBalls | ![](https://raw.githubusercontent.com/scottrbailey/WLED-Utils/master/gifs/FX_091.gif) | <img width="320" src="https://github.com/user-attachments/assets/30aa2614-d891-4d39-a789-72026f7ab036" /> | |
| Blurz | ![](https://raw.githubusercontent.com/scottrbailey/WLED-Utils/master/gifs/FX_MM163.gif) | <img width="320" src="https://github.com/user-attachments/assets/b8f4111d-1f78-4ac0-a0f9-17909ad78599" /> | |
| DistortionWaves | | <img width="320" src="" /> | ![](https://raw.githubusercontent.com/scottrbailey/WLED-Utils/master/gifs/FX_124.gif) |
| FreqMatrix | | <img width="320" src="" /> | ![](https://raw.githubusercontent.com/scottrbailey/WLED-Utils/master/gifs/FX_138.gif) |
| GEQ | ![](https://raw.githubusercontent.com/scottrbailey/WLED-Utils/master/gifs/FX_MM139.gif) | <img width="320" src="https://github.com/user-attachments/assets/ca485d1e-e6e4-4b4a-910f-eed7fde7a21f" /> | |
| FreqSaws | | <img width="320" src="https://github.com/user-attachments/assets/78828181-c1b3-4d0f-85dc-356ced10fcd9"/> | |
| Lissajous | ![](https://raw.githubusercontent.com/scottrbailey/WLED-Utils/master/gifs/FX_MM176.gif) | <img width="320" src="https://github.com/user-attachments/assets/a28c19a7-012a-49bf-aff1-6b414c9d08dd" /> | |
| Noise2D | ![](https://raw.githubusercontent.com/scottrbailey/WLED-Utils/master/gifs/FX_146.gif) | | |
| NoiseMeter | ![](https://raw.githubusercontent.com/scottrbailey/WLED-Utils/master/gifs/FX_136.gif) | | |
| PopCorn | ![](https://raw.githubusercontent.com/scottrbailey/WLED-Utils/master/gifs/FX_095.gif) | | |
| Waverly | ![](https://raw.githubusercontent.com/scottrbailey/WLED-Utils/master/gifs/FX_MM165.gif) | | |

### Moving head effects

| Name | Preview | Controls | Remarks
| ---- | ----- | ---- | ---- |
| Troy1Color | | | |
| Troy1Move | | | |
| Troy2Color | | | |
| Troy2Move | | | |
| FreqColors | | | |
| WowiMove | | | |
| AmbientMove | | | |

🚨: some effects already do this theirselves e.g. FreqMatrix runs on 1D but copies to 2D and 3D if size allows.

