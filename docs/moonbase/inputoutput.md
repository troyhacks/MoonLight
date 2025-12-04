# IO Module

<img width="320" height="441" alt="io11" src="https://github.com/user-attachments/assets/b5452d8e-400c-4a72-a195-d5b7a7764407" />
<img width="320" height="77" alt="io22" src="https://github.com/user-attachments/assets/99fd135c-8d36-463b-ae3a-f06119cb8ccb" />

Defines hardware specifics per ESP32 device 🆕
Currently the following boards are defined. Not all are supported yet 🚧

<Image>

For each board the following presets are defined:

* Modded: if any change to the default preset is made.
* Max Power: adjust the brightness to approach this max power, depending on the number of LEDs used.
* Pins: This module is the central place to assign functionality to gpio pins. Other modules and nodes use the pin assignments made here.

* Per pin:
    * GPIO = gpio_num;
    * Usage: Can be updated, see below
    * Summary
       * Valid (✅)
       * Output (💡)
       * RTC (⏰);
    * Level (HIGH, LOW, N/A)
    * DriveCap (WEAK, STRONGER, MEDIUM, STRONGEST, N/A)

<image>

## Pin usage

  * Supported
    * LEDs DIO 01 to 20
    * Voltage
    * Current
    * Infrared
    * Button On/Off
  * Planned soon
    * Battery
    * Relais brightness
    * DMX (in)
  * Planned later
    * I2S for microphone and line in
    * I2C

## Naming convention

- MCU: The ESP32 chip
- MCU-Board: MCU on a PCB
- Carrier Board: MCU-board + connectors...
- Device: All of the above in a box
