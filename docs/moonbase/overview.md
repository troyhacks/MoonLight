# MoonBase Overview

<img width="170" src="https://github.com/user-attachments/assets/87ed3c78-7a4e-4331-b453-47762cce64fa" />

* [File Manager](https://moonmodules.org/MoonLight/moonbase/filemanager/)
* [Devices](https://moonmodules.org/MoonLight/moonbase/module/devices/)
* [Tasks](https://moonmodules.org/MoonLight/moonbase/module/tasks/)
* [IO](https://moonmodules.org/MoonLight/moonbase/module/input/output/)

## Status Bar

<img width="306" src="https://github.com/user-attachments/assets/5568bcdf-8d12-430e-9801-3c851f4204b5" />
<img width="362" src="https://github.com/user-attachments/assets/2379c5c4-0b85-4810-aac3-d4e6c650a12b" />

* **Safe Mode**: MoonBase will jump into safe mode after a crash, see 🛡️ in the statusbar or [System Status](https://moonmodules.org/MoonLight/system/status/). In safe mode, MoonLight will disable the following features:
    * Add more then 1024 lights
    * Start LED drivers
    * Execute a live script

    Clicking on the 🛡️ icon allows you to restart. If the reason for crash has dissappeared the device will start normally (not in safe mode).

* **Restart needed**: e.g. when drivers are changed, a restart might be needed, see 🔄 in the statusbar. Clicking on the 🔄 icon allows you to restart

* **Save** and **Cancel** Changes are only saved after pressing the save button 💾. To undo changes, press the cancel button 🚫 and the last saved state will be restored.

* **Theme**: Light, Dark, Auto

* **Energy**: Show charging status (voltage and current 🚧)

* **New firmware**: New firmware available.
  
* **Wifi**: See ESP32-Sveltekit
