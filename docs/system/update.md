# System Update

<img width="522" alt="image" src="https://github.com/user-attachments/assets/9d6452fc-9bcd-4efa-8839-30b3fd015276" />

This module supports Over the Air Updates (OTA).
There are 2 ways to update the firmware in this module:

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
