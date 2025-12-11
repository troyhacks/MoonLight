# IO Module

<img width="320" height="441" alt="io11" src="../../media/moonbase/inputoutput.png" />

Defines hardware specifics per ESP32 device 🆕

Currently the following boards are defined. Not all are supported yet 🚧

<img width="320" height="441" alt="io11" src="../../media/moonbase/inputoutputpresets.png" />

For each board the following presets are defined:

* Modded: if any change to the default preset is made.
* Max Power in Watts: adjust the brightness to approach this max power, depending on the number of LEDs used. Default 10: 5V * 2A = 10W (so it runs fine on USB). Used by LED drivers, see [Drivers](../../moonlight/drivers/)
* Jumper1: If the board contains a jumper, it can define pin behaviour. Eg. select between Infrared and Ethernet.
* Pins: This module is the central place to assign functionality to gpio pins. Other modules and nodes use the pin assignments made here.
    * GPIO = gpio_num;
    * Usage: See below
    * Index: specify first, second, third, ... usage output, e.g. LED D01 to LED D16
    * Summary
       * Valid (✅)
       * Output (💡)
       * RTC (⏰);
    * Level (HIGH, LOW, N/A)
    * DriveCap (WEAK, STRONGER, MEDIUM, STRONGEST, N/A)

## Pin usage

  * Supported
    * LEDs: Used by LED drivers to setup LED outputs, see [Drivers](../../moonlight/drivers/)
    * Voltage and Current: Sets energy monitoring, see [System status](../../system/status) and , see [System Metrics](../../system/metrics)
    * Infrared: Used by IR driver, see [Drivers](../../moonlight/drivers/)
    * Button LightsOn: sets on/off in [Light Control](../../moonlight/lightscontrol/)
    * Relay LightsOn: sets on/off in [Light Control](../../moonlight/lightscontrol/)
    * SPI_SCK, SPI_MISO, SPI_MOSI, PHY_CS, PHY_IRQ: S3 Ethernet, Used by the Ethernet module, see [Ethernet](../../network/ethernet/)
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

* [Dig 2Go](https://quinled.info/quinled-dig2go/), [Dig Uno](https://quinled.info/pre-assembled-quinled-dig-quad/), [Dig Quad](https://quinled.info/quinled-dig-octa/): Choose the esp32-d0 (4MB) board in the [MoonLight Installer](../../gettingstarted/installer/) 
* [Dig Octa](https://quinled.info/quinled-dig-octa/): Choose the esp32-d0-16mb board in the [MoonLight Installer](../../gettingstarted/installer/) 
* On first install, erase flash first (Especially when other firmware like WLED was on it) as MoonLight uses a partition scheme with 3MB of flash (currently no OTA support).
* You might need to reset your router if you first run WLED on the same MCU and no new IP is assigned.

!!! Tip "Dig Uno USB"
    Remove fuse to connect USB cable to flash the board.

### MyHome-Control ESP32-P4 shield

![ESP32-P4 shield](https://shop.myhome-control.de/thumbnail/87/41/c2/1762031307/WLED_ESP32_P4_Shield_02_1920x1326.jpg?ts=1762031315){: style="width:320px"}

[ESP32-P4 shield](https://shop.myhome-control.de/en/ABC-WLED-ESP32-P4-shield/HW10027)

### SE16 v1

![SE-16p](../firmware/installer/images/esp32-s3-stephanelec-16p.jpg){: style="width:100px"}

* Choose the esp32-s3-devkitc-1-n8r8v board in the [MoonLight Installer](../../gettingstarted/installer/) 
* Set jumper1 the same as you set it on the board: on: Infrared, off: Ethernet