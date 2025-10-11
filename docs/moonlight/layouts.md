# Layouts

A layout (🚥) defines what lights are connected to MoonLight. It defines the coordinates of all lights (addLight) and assigns lights to the GPIO pins of the ESP32 (addPin) and how many channels each light has (normal LEDs 3: Red, Green and Blue). 

    * The **coordinates** of each light are defined in a 3D coordinate space where each coordinate range between 1 and 255. Currently a strip until 255 LEDs is supported, a panel until 128x96 LEDS and a cube max 20x20x20. 
        * Coordinates needs to be specified in the order the lights are wired so MoonLight knows which light is first, which is second etc.
        * If a 1D strip is longer, you can address more LEDs by pretending it is a 2D fixture, e.g. 32x32 to address a strip of 1024 LEDs. 
        * In the future we might look at redefining Coord3D where width is 12 bytes, height is 7 bytes and depth is 5 bytes allowing for max 4096 x 128 x 32 ...
    * Currently **pins** are by default driven by FastLED.show. FastLED needs all LEDs specifications be defined at compile time, e.g. LED type, color order, etc. Normally also the pin(s) need to be defined beforehand but all pins are predefined in the code to avoid this (at a cost of larger firmware). As we want to be able to define all LED specs in the UI instead of during compile, we need to see how / if this can be done.
    * **Multiple layout nodes** can be defined which will execute one after the other
    * MoonLight will use the layout definition to generate a **mapping** of a virtual coordinate space to a physical coordinate space. Most simple example is a panel which has a snake layout. The mapping will create a virtual layer where the snake layout is hidden.

## Layout 🚥 Nodes
🚧
!!! danger

    Set pins carefully. There is a check if a specific pin can be used for output / driving LEDs. See also the IO module. But not 100% garanteed. E.g pin 16 on esp32-d0-wrover crashes...

| Name | Controls | Preview | Remarks
| ---- | ----- | ---- | ---- |
| Panel | * width and height
    * orientation XY or YX: Vertical or horizontal
    * X++ and Y++: X++: starts at Top or bottom, Y++: starts left or right
    * snake: is the first axis in snake / serpentine layout?
    * pin: to do add more pins | | Defines a 2D panel with width and height |
| Panels | | | |
| Cube | * width and height and depth
    * orientation XYZ or YXZ, XZY or YZX, ZXY or ZYX: Vertical or horizontal or depth
    * X++ and Y++ and Z++: Z++ starts front or back
    * snake X, Y, Z: multidimensional snaking, good luck 😜
    * pin: to do add more pins | | Panel layout + depth |
| Rings | | | |
| Wheel | | | |
| HumanSizedCube | | | |
| SingleLine | | | |
| SingleRow | | | |
| SE16 | * ledsPerPin: the number of LEDs connected to one pin
* pinsAreColumns: are the LEDs on a pin a row of the effect (width is 1 (or 2) x ledsPerPin). If not set the LEDs are a column (height is 1 (or 2) x ledsPerPin)
* mirroredPins: If set it is assumed that LEDs are connected with increasing positions on 8 pins on one side of the board and decreasing positions on the 8 pins of the other side of the board. The resulting size will have a width of 8 and the height (or width) will be 2 * ledsPerPin. If not set, the width will be 16 and the height (or width) = ledsPerPin | | Layout(s) for Stephan Electronics 16-Pin ESP32-S3 board, using the pins used on the board |