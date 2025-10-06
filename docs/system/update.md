# System Update

<img width="320" src="https://github.com/user-attachments/assets/d7f76c8a-5290-4723-b286-704c574d6beb" />

This module supports Over the Air Updates (OTA).
There are 2 ways to update the firmware in this module:

<img width="362" src="https://github.com/user-attachments/assets/cf72875f-02cf-4c78-97a0-56b2a1723d71" />
* Using GitHub releases

    * List of firmware releases is showed here
    * Select one of the releases to install. In general it is advised to install the most recent version.
    * If there is a new release, the app will notify it's availability.
    * See here for details on releases: [releases](https://github.com/MoonModules/MoonLight/releases)

* Manually via Upload
    * Select choose file, Press Upload if asked for Are you sure?. 

When starting the update, a progress indicator is shown, upon completion the device will restart with the new firmware.

!!! warning "Uptate on small devices not possible"
    On some devices firmware update using above methods is not possible (yet) due to a limited amount of file size or memory (ESP32-D0 devices). Use the [Installer](https://moonmodules.org/MoonLight/gettingstarted/installation/) procedure instead (without erasing the device)
