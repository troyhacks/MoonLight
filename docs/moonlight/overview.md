# MoonLight Overview

<img width="163" src="https://github.com/user-attachments/assets/1afd0d4b-f846-4d5b-8cc9-8fa8586c405b" />

* [Lights control](https://moonmodules.org/MoonLight/moonbase/module/lightsControl/)
* [Effects](https://moonmodules.org/MoonLight/moonbase/module/effects/)
* [Drivers](https://moonmodules.org/MoonLight/moonbase/module/drivers/)
* [Live Scripts](https://moonmodules.org/MoonLight/moonbase/module/liveScripts/)
* [Channels](https://moonmodules.org/MoonLight/moonbase/module/channels/)
* [MoonLight info](https://moonmodules.org/MoonLight/moonbase/module/moonLightInfo/)

## Status Bar

<img width="306" height="70" alt="image" src="https://github.com/user-attachments/assets/5568bcdf-8d12-430e-9801-3c851f4204b5" />

* **Safe Mode**: MoonBase will jump into safe mode after a crash, see 🛡️ in the statusbar or [System Status](https://moonmodules.org/MoonLight/system/status/). In safe mode, MoonLight will disable the following features:
    * Add more then 1024 lights
    * Start LED drivers
    * Execute a live script

    Clicking on the 🛡️ icon allows you to restart. If the reason for crash has dissappeared the device will start normally (not in safe mode).

* **Restart needed**: e.g. when drivers are changed, a restart might be needed, see 🔄 in the statusbar. Clicking on the 🔄 icon allows you to restart

* **Save** and **Cancel** Changes are only saved after pressing the save button 💾. To undo changes, press the cancel button 🚫 and the last saved state will be restored.

* **Wifi**: See ESP32-Sveltekit

* **Battery**: See ESP32-Sveltekit

* **Sleep mode**: See ESP32-Sveltekit
