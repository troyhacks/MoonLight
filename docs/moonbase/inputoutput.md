# IO Module

<img width="320" height="441" alt="io11" src="https://github.com/user-attachments/assets/b5452d8e-400c-4a72-a195-d5b7a7764407" />
<img width="320" height="77" alt="io22" src="https://github.com/user-attachments/assets/99fd135c-8d36-463b-ae3a-f06119cb8ccb" />

Defines hardware specifics per ESP32 device 🆕
Currently the following boards are defined. Not all are supported yet 🚧

<Image>

For each board the following presets are defined:

* Modded: if any change to the default preset is made.
* Max Power in Watts: adjust the brightness to approach this max power, depending on the number of LEDs used. Default 10: 5V * 2A = 10W (so it runs fine on USB).
* Jumper1: If the board contains a jumper, it can define pin behaviour. Eg. select between Infrared and Ethernet.
* Pins: This module is the central place to assign functionality to gpio pins. Other modules and nodes use the pin assignments made here.

* Per pin:
    * GPIO = gpio_num;
    * Usage: See below
    * Index: specify first, second, third, ... usage output, e.g. LED D01 to LED D16
    * Summary
       * Valid (✅)
       * Output (💡)
       * RTC (⏰);
    * Level (HIGH, LOW, N/A)
    * DriveCap (WEAK, STRONGER, MEDIUM, STRONGEST, N/A)

<image>

## Pin usage

  * Supported
    * LEDs
    * Voltage
    * Current
    * Infrared
    * Button LightsOn: sets on/off in [Light Control](https://moonmodules.org/MoonLight/moonlight/lightscontrol/)
    * Relay LightsOn: sets on/off in [Light Control](https://moonmodules.org/MoonLight/moonlight/lightscontrol/)
    * SPI_SCK, SPI_MISO, SPI_MOSI, PHY_CS, PHY_IRQ: S3 Ethernet
  * Planned soon
    * Battery
    * DMX (in)
  * Planned later
    * I2S for microphone and line in
    * I2C

## Naming convention

- MicroController (MCU): The ESP32 chip
- MCU-Board (MCB): MCU on a PCB
- Carrier Board (CRB): board that the MCU-board plugs into (Or shield or controller board or interface board)
- Device (DVC): All of the above in a box with connectors

## Board details

### QuinLed boards

![Dig2Go](https://shop.allnetchina.cn/cdn/shop/products/Led_4.jpg?v=1680836018&width=1600){: style="width:100px"} 
![Dig Uno](https://quinled.info/wp-content/uploads/2020/02/QuinLED-Dig-Uno-v3_front.png){: style="width:100px"} 
![Dig Quad](https://quinled.info/wp-content/uploads/2021/11/QuinLED-Dig-Quad-AB_v3r1-2048x1154.png){: style="width:100px"}
![Dig Octa](https://quinled.info/wp-content/uploads/2024/10/20240924_141857-2048x1444.png){: style="width:100px"}

* Dig 2Go, Dig Uno, Dig Quad: Choose the esp32-d0 (4MB) board in the [MoonLight Installer](https://moonmodules.org/MoonLight/gettingstarted/installer/) 
* Dig Octa: Choose the esp32-d0-16mb board in the [MoonLight Installer](https://moonmodules.org/MoonLight/gettingstarted/installer/) 
* On first install, erase flash first (Especially when other firmware like WLED was on it) as MoonLight uses a partition scheme with 3MB of flash (no ota at the moment).
* You might need to reset your router if you first run WLED on the same MCU and no new IP is assigned.

!!! Tip "Dig Uno USB"
    Remove fuse to connect USB cable to flash the board.

### SE16 v1

![SE-16p](../firmware/installer/images/esp32-s3-stephanelec-16p.jpg){: style="width:100px"}

* Choose the esp32-s3-devkitc-1-n8r8v board in the [MoonLight Installer](https://moonmodules.org/MoonLight/gettingstarted/installer/) 
* Set jumper1 the same as you set it on the board: on: Infrared, off: Ethernet