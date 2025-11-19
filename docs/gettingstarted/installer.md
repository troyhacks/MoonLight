# MoonLight Installer

Flashing the firmware is easy and doesn't require you to install any development tools. You can do this through a compatible browser which supports webserial like Chrome. 

<script type="module" src="https://unpkg.com/esp-web-tools@10/dist/web/install-button.js?module"></script>

Release 0.6.0, 7 November 2025

This installer installs <a href="https://github.com/MoonModules/MoonLight" target="_blank">MoonModules/MoonLight</a> on an ESP32 microcontroller.

See <a href="https://moonmodules.org/MoonLight/gettingstarted/hardware" target="_blank">MoonLight Hardware</a> for more info

For a step-by-step instruction, go to <a href="https://moonmodules.org/MoonLight/gettingstarted/installation" target="_blank">MoonLight installation</a>

## Installer

* Plug in your ESP32 Microcontroller to an USB port and wait for the USB port to enumerate, use an USB cable which supports data transfer
* Choose your board and press Connect and follow the steps, select the correct COM port (USB JTAG/serial debug unit).
* Most recommended boards are listed first. If your board isn't listed here, try 'others'
* Some boards: Place the board in bootloader mode by press and holding the `Boot` button on the ESP32, press the `Reset` button on the ESP32 and then release the `Boot` button.
* After the installation has completed press the `Reset` button on the ESP32 once again.


| Name | Image | Flash | Shop | USB Driver |
|------|-------|-------|------|------------|
| esp32-s3-atoms3r - 🆕 ! | ![esp32-s3-atoms3r](../firmware/installer/images/esp32-s3-atoms3r.jpg){: style="width:100px"} | <esp-web-install-button manifest="https://moonmodules.org/MoonLight/firmware/installer/manifest_esp32-s3-atoms3r.json"></esp-web-install-button> | [M5Stack store](https://shop.m5stack.com/products/atoms3r-dev-kit){:target="_blank"} | |
| esp32-s3-devkitc-1-n8r8v | ![esp32-s3-devkitc-1-n8r8v](../firmware/installer/images/esp32-s3-devkitc-1-n8r8v.jpg){: style="width:100px"} | <esp-web-install-button manifest="https://moonmodules.org/MoonLight/firmware/installer/manifest_esp32-s3-devkitc-1-n8r8v.json"></esp-web-install-button> | | |
| esp32-s3-devkitc-1-n16r8v | ![esp32-s3-devkitc-1-n16r8v](../firmware/installer/images/esp32-s3-devkitc-1-n8r8v.jpg){: style="width:100px"} | <esp-web-install-button manifest="https://moonmodules.org/MoonLight/firmware/installer/manifest_esp32-s3-devkitc-1-n16r8v.json"></esp-web-install-button> | [Ali*](https://s.click.aliexpress.com/e/_DBAtJ2H){:target="_blank"} | |
| esp32-s3-stephanelec-16p | ![esp32-s3-stephanelec-16p](../firmware/installer/images/esp32-s3-stephanelec-16p.jpg){: style="width:100px"} | <esp-web-install-button manifest="https://moonmodules.org/MoonLight/firmware/installer/manifest_esp32-s3-stephanelec-16p.json"></esp-web-install-button> | | |
| esp32-s3-zero-n4r2 | ![esp32-s3-zero-n4r2](../firmware/installer/images/esp32-s3-zero-n4r2.jpg){: style="width:100px"} | <esp-web-install-button manifest="https://moonmodules.org/MoonLight/firmware/installer/manifest_esp32-s3-zero-n4r2.json"></esp-web-install-button> | [Ali*](https://s.click.aliexpress.com/e/_EukjHX8){:target="_blank"} | |
| esp32-d0-wrover | ![esp32-d0-wrover](../firmware/installer/images/esp32-d0-wrover.jpg){: style="width:100px"} | <esp-web-install-button manifest="https://moonmodules.org/MoonLight/firmware/installer/manifest_esp32-d0-wrover.json"></esp-web-install-button> | [Ali*](https://a.aliexpress.com/_EzhPi6g){:target="_blank"} | |
| esp32-d0-wrover-moonbase | ![esp32-d0-wrover](../firmware/installer/images/esp32-d0-wrover.jpg){: style="width:100px"} | <esp-web-install-button manifest="https://moonmodules.org/MoonLight/firmware/installer/manifest_esp32-d0-wrover-moonbase.json"></esp-web-install-button> | [Ali*](https://a.aliexpress.com/_EzhPi6g){:target="_blank"} | |
| esp32-d0-16mb | ![esp32-d0-16mb](../firmware/installer/images/esp32-d0-16mb.jpg){: style="width:100px"} | <esp-web-install-button manifest="https://moonmodules.org/MoonLight/firmware/installer/manifest_esp32-d0-16mb.json"></esp-web-install-button> | [Serg74](https://www.tindie.com/products/serg74/esp32-wroom-usb-c-d1-mini32-form-factor-board/){:target="_blank"} | |
| esp32-d0 | ![esp32-d0](../firmware/installer/images/esp32-d0.jpg){: style="width:100px"} | <esp-web-install-button manifest="https://moonmodules.org/MoonLight/firmware/installer/manifest_esp32-d0.json"></esp-web-install-button> | | |
| esp32-d0-moonbase | ![esp32-d0](../firmware/installer/images/esp32-d0.jpg){: style="width:100px"} | <esp-web-install-button manifest="https://moonmodules.org/MoonLight/firmware/installer/manifest_esp32-d0-moonbase.json"></esp-web-install-button> | | |
| esp32-p4-nano | ![esp32-p4-nano](../firmware/installer/images/esp32-p4-nano.jpg){: style="width:100px"} | <esp-web-install-button manifest="https://moonmodules.org/MoonLight/firmware/installer/manifest_esp32-p4-nano.json"></esp-web-install-button> | [Waveshare](https://www.waveshare.com/esp32-p4-nano.htm){:target="_blank"} | |
| esp32-p4-olimex 🚧 | ![esp32-p4-olimex](../firmware/installer/images/esp32-p4-olimex.jpg){: style="width:100px"} | <esp-web-install-button manifest="https://moonmodules.org/MoonLight/firmware/installer/manifest_esp32-p4-olimex.json"></esp-web-install-button> | [Olimex](https://www.olimex.com/Products/IoT/ESP32-P4/ESP32-P4-DevKit/open-source-hardware){:target="_blank"} | |
| esp32-c3-devkitm-1 | ![esp32-c3-devkitm-1](../firmware/installer/images/esp32-c3-devkitm-1.jpg){: style="width:100px"} | <esp-web-install-button manifest="https://moonmodules.org/MoonLight/firmware/installer/manifest_esp32-c3-devkitm-1.json"></esp-web-install-button> | [Ali*](https://s.click.aliexpress.com/e/_EIl7NKw){:target="_blank"} | |
| esp32-c3-supermini | ![esp32-c3-supermini](../firmware/installer/images/esp32-c3-supermini.jpg){: style="width:100px"} | <esp-web-install-button manifest="https://moonmodules.org/MoonLight/firmware/installer/manifest_esp32-c3-supermini.json"></esp-web-install-button> | [Ali*](https://s.click.aliexpress.com/e/_EIl7NKw){:target="_blank"} | |
| others | ![others](../firmware/installer/images/others.jpg){: style="width:100px"} | <esp-web-install-button manifest="https://moonmodules.org/MoonLight/firmware/installer/manifest.json"></esp-web-install-button> | | |

* MoonBase firmware is MoonBase only, no MoonLight included (basis for other IOT projects)

## USB-to-serial chip drivers

In case your computer does not recognise your connected ESP32, you might need to install the right USB-to-serial chip drivers.
Below are the drivers for common chips used in ESP devices. See the images below to see what chip your ESP32 has.

  * CP210x drivers: [Windows & Mac]("https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers")
  * CH342, CH343, CH9102 drivers: [Windows]("https://www.wch.cn/downloads/CH343SER_ZIP.html"), [Mac]("https://www.wch.cn/downloads/CH34XSER_MAC_ZIP.html") (download via blue button with download icon)</li>
  * CH340, CH341 drivers: [Windows]("https://www.wch.cn/downloads/CH341SER_ZIP.html"), [Mac]("https://www.wch.cn/downloads/CH341SER_MAC_ZIP.html") (download via blue button with download icon)</li>

    <img src="https://moonmodules.org/MoonLight/firmware/installer/images/cp210x-ch34x.jpg" height="200"/>
    <img src="https://moonmodules.org/MoonLight/firmware/installer/images/ch9102.jpg" height="200"/>

Powered by [ESP Web Tools](https://esphome.github.io/esp-web-tools/)

See also [ESP-Web-Tools-Tutorial]("https://github.com/witnessmenow/ESP-Web-Tools-Tutorial/blob/main/README.md")

*: Affiliate link
