---
hide:
  - navigation
  - toc
---

# MoonLight

<img width="500" alt="image" src="https://github.com/user-attachments/assets/2dee317b-70d0-4e25-be6a-779ff5fa94f0" />

[MoonLight](https://github.com/MoonModules/MoonLight) is software which runs on [ESP32 microcontrollers](https://www.espressif.com/en/products/socs/esp32) providing a web-browser interface to control lights in particular and any IOT service in general. ESP32 devices can be connected to your local network via WiFi or Ethernet, Moonlight can be controlled via a web browser selecting the IP-address or network name of the device.
MoonLight can be installed on ESP32 by means of a web installer or if you are a developer, via Visual Studio Code with the PlatformIO plugin installed.

MoonLight's core functionality is called MoonBase, Lighting specific functionality is called MoonLight. MoonLight is a stand alone product for end users controlling lights but Moonlight can also be used to create a custom IOT service by creating a github fork of MoonLight, rename it to the service, switching off the MoonLight feature and build your own functionality. This can be as simple as creating a smart plug or a timer or temperature sensor or as complex as for instance a greenhouse management system, using the ESP32 hardware interfaces (via GPIO ports) or it's network communication protocols.

Examples of Moonbase functionality are monitor and manage a device, manage WiFi, Firmware updates, System Status and Restart. On top of that there is a file manager and an instance manager.

MoonLight functionality is aimed at running effects on multiple lights. It's primarely focused on LED strips or LED panels but also DMX lights can be controlled via ArtNet. Depending on the type of ESP32 used and the amount of memory available, the number of LEDs can go up to 12288 at 120 FPS! Even more LEDs are possible, upon 65K at lower framerates. Also a large number of DMX lights can be controlled. MoonLight works with virtual and physical layers of lights and contains a node based model where nodes can be light layouts, effects and modifiers and run effects on a virtual layer and map the effect onto physical lights. 

<img width="400" src="https://github.com/user-attachments/assets/09143feb-1356-463c-bc23-6982ef3eeb42"/>

MoonLight is a [MoonModules.org](https://moonmodules.org) project. MoonModules is a group of lighting enthusiasts who also made WLED MM and contribute to WLED. Where WLED (MM) is aimed at 1D and 2D effects and LED setups, MoonLight is build to support 2D and 3D effects and multiple light setups. MoonLight has been built from scratch using the experience gained working on WLED (MM).

* Latest release can be found [here](https://github.com/MoonModules/MoonLight/releases). Release v0.5.5 is a developer release and works towards MoonLight 0.6.0 which is aimed at end users and is expected in September 2025. Containing presets controller pad, user friendly installer, physical and virtual driver for large displays, WLED audio sync and more.
* Progress can be tracked in the [Kanban board](https://github.com/users/MoonModules/projects/2/)
* Get started [here](https://moonmodules.org/MoonLight/general/gettingstarted/)
* Follow us on [Discord/MoonLight](https://discord.gg/TC8NSUSCdV) or [Reddit](https://reddit.com/r/moonmodules).
* See [Star-Mod-Base-Light-Moon-Svelte-Live](https://moonmodules.org/Star-Mod-Base-Light-Moon-Svelte-Live) how MoonLight evolved from StarMod, StarBase, StarLight, MoonBase. 
* Documentation see [MoonLightDocs](https://moonmodules.org/MoonLight/).
* We need developers: to create new layouts, effects and modifiers, to create ci-automations, to connect Midi controllers, to make web installers, to improve the UI or whatever good idea you have. Contact us on [Discord](https://discord.com/channels/700041398778331156/1203994211301728296) or [Reddit](https://reddit.com/r/moonmodules) if you want to help!
* If you appreciate this project, a star on the repo would be awesome: [![GitHub stars](https://img.shields.io/github/stars/MoonModules/MoonLight?style=social)](https://github.com/MoonModules/MoonLight/stargazers)

## Acknowledgements

MoonLight uses the following repositories:

* [theelims/ESP32-sveltekit](https://github.com/theelims/ESP32-sveltekit): MoonLight is a fork of ESP32-sveltekit. A simple and extensible framework for ESP32 based IoT projects with a feature-rich, beautiful, and responsive front-end build with Sveltekit, TailwindCSS and DaisyUI. It provides a powerful back end service, an amazing front end served from the ESP32 and an easy to use build chain to get everything going. See also [ESP32-sveltekit POC](https://github.com/theelims/ESP32-sveltekit/issues/68) where the idea of MoonLight on ESP32SvelteKit started
* [hoeken/PsychicHttp](https://github.com/hoeken/PsychicHttp): PsychicHttp is a webserver library for ESP32 + Arduino framework which uses the ESP-IDF HTTP Server library under the hood.
* [theelims/PsychicMqttClient](https://github.com/theelims/PsychicMqttClient): Fully featured async MQTT 3.1.1 client for ESP32 with support for SSL/TLS and MQTT over WS. Uses the ESP-IDF MQTT client library under the hood and adds a powerful but easy to use API on top of it. Supports MQTT over TCP, SSL with mbedtls, MQTT over Websocket and MQTT over Websocket Secure.
* [bblanchon/ArduinoJson](https://github.com/bblanchon/ArduinoJson): ArduinoJson is a C++ JSON library for Arduino and IoT (Internet Of Things).
* [FastLED/FastLED](https://github.com/FastLED/FastLED): FastLED is a robust and massively parallel-led driver. Due to it's incredibly small compile size, high end devices can drive upto ~20k on ESP32. Supports nearly every single LED chipset in existence. Background rendering means you can respond to user input while the leds render.
* [hpwit/ESPLiveScript](https://github.com/hpwit/ESPLiveScript): execute programs on the esp32 without having to load the code via any IDE (Arduino, VSCode)
* [hpwit/I2SClocklessLedDriver](https://github.com/hpwit/I2SClocklessLedDriver): This library is a new take on driving ws2812 leds with I2S on an esp32. It allows to drive up to 16 LED strips in parallel
* [/hpwit/I2SClocklessVirtualLedDriver](https://github.com/hpwit/I2SClocklessVirtualLedDriver): This library is a new take on driving ws2812 leds with I2S on an esp32. It allows to drive up to 120 LED strips !!! in parallel
* [netmindz/WLED-sync](https://github.com/netmindz/WLED-sync): Library to create WLED compatible projects that sync their audio

<img src="https://github.com/user-attachments/assets/c655d610-53eb-4dd3-8e9e-0cfa23b97bb4"/>

## License

MoonBase, MoonLight ⚖️ GPL-v3