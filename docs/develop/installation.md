
# Installation

The development environment consists of

* VSCode
    * Platformio IDE (or pioarduino IDE)
    * nodejs
    * git
    * python

* Github manager
    * GitKraken (recommended)
    * GitHub Desktop

## Summary

Install VSCode with the platformIO IDE extension (or pioarduino IDE). Make sure git and nodejs is installed. Download the MoonLight repo. Open it in VSCode and build or upload to an ESP32 device.

Optionally you need to run npm install and make sure python > v3.10 in VSCode.

## Install Visual Studio Code

* Download the MoonLight repository
    * Use [GitKraken](https://www.gitkraken.com/download) or GitHub Desktop (or manually)
        * Create a folder to download to: e.g. Developer/GitGub/MoonModules/MoonLight
        * Press + / New tab and select Clone a Repo 
        * Copy [MoonLight.git](https://github.com/MoonModules/MoonLight.git) and paste in the URL field of GitKraken
        * Press clone the repo

    <img width="320" src="https://github.com/user-attachments/assets/74928dac-d59b-4489-b97b-759c6d792b77" />

* Download Visual Studio Code
    * Windows: download from the Microsoft Store
    * MacOS: [Visual Studio download](https://code.visualstudio.com/download)

    <img width="320" src="https://github.com/user-attachments/assets/ff6be5d6-40b2-48ae-9f17-89fc5bcfd848" />

* Install PlatformIO IDE (or pioarduino IDE)
    * Open the  MoonLight repository folder created with GitKraken
    * PlatformIO IDE is default but as we are using latest esp-idf, pioarduino IDE is needed when PlatformIO IDE fails. For now, start with PlatformIO IDE (see later)
    * In VSCode, search for the extension

    <img width="320" src="https://github.com/user-attachments/assets/d91ab6b0-aeeb-42ed-8a85-d608b88c6103" />

    * Install the extension. Please note it can take a while before Configuring project is finished. Don't change anything in the code yet. Drink a coffee.

    <img width="320" src="https://github.com/user-attachments/assets/ea953f62-0d94-499c-92f0-491d31c2edff" />

* Build MoonLight
    * Press ☑️ in the bottom statusbar

    <img width="320" src="https://github.com/user-attachments/assets/4e6faf4a-f169-46e4-a2f8-5d4021516a04" />

    * When git is not installed on your system, install it, restart vscode and press ☑️ again

    <img width="320" src="https://github.com/user-attachments/assets/8d39c637-c7dc-4c34-b177-f8eb63d54863" />

    * If you compile for the first time it take some time to finish

    <img width="640" height="268" alt="Screenshot 2025-09-25 205255" src="https://github.com/user-attachments/assets/02aa87cc-a0b7-40cb-9d0e-99ea91d890ec" />

    <img width="640" height="344" alt="Screenshot 2025-09-25 205814" src="https://github.com/user-attachments/assets/c5655c00-6b91-4745-ae22-39f8573ae05a" />

    <img width="640" height="363" alt="Screenshot 2025-09-25 210504" src="https://github.com/user-attachments/assets/2d21319d-2e86-473e-9e5a-d500ecb462c8" />

* Upload MoonLight to an esp32-device
    * Connect an ESP32 device via USB (ESP32-S3 preferred) and select the device using the (second) 🔌 icon in the staturbar
        * Select esp32-d0 for a normal ESP32
        * Select esp32-s3-devkitc-1-n16r8v for an ESP32-S3 ([recommended](https://s.click.aliexpress.com/e/_DBAtJ2H) or similar)

       <img width="617" src="https://github.com/user-attachments/assets/349af246-30c7-45dd-92ed-4f2b3900557f" />

    * Erase the device if it is a new device or the device has not been used for MoonModules before 

    	<img width="350" src="https://github.com/user-attachments/assets/322bb30e-2709-43de-bd68-80044c9a0673" />

    * Press upload (➡️) in the status bar

	<img width="350" src="https://github.com/user-attachments/assets/e4cbda64-739c-410d-9cdc-d2645e24c7ba" />

    * The firmware is now flashed to your ESP32 device, after flashing the ESP32 device will reboot
    * Recommended: Press PlatformIO:Serial Monitor to see the debug information produced

## Connect and setup MoonLight

Before changing code, test if the current download of MoonLight is running fine.

* See [Connect MoonLight](https://moonmodules.org/MoonLight/gettingstarted/installation/#connect-moonlight)

    * MoonLight has a cache expiration of one year. However if you are developing or updating nightly builds cached UI code might be outdated.
    * Advanced tip: Use [ESP32Devices](https://github.com/ewowi/ESP32Devices) to discover the ESP32 nodes on your network (using nodeJS and html)
    * Check AP settings

	<img width="350" src="https://github.com/user-attachments/assets/a8d89d7a-d4e6-4443-b2c2-997c0ac89381" />
  
* See [Setup MoonLight](https://moonmodules.org/MoonLight/gettingstarted/installation/#setup-moonlight)

## Changing MoonLight code

* Changing the backend (c/c++)
    * lib folder for Sveltekit 
    * src folder for MoonBase and MoonLight
    * ☑️ and ➡️ to build and or upload, if frontend has not changed this will run succesfully

* Changing the frontend (UI)
    * See the interface folder
    * on each file change, ☑️ or ➡️ will rebuild the UI using node.js
    * if node.js is not on your system you will get this error:

    <img width="640" height="221" alt="Screenshot 2025-09-25 212255" src="https://github.com/user-attachments/assets/f05ca8d2-6485-4965-8b75-edd4b468ddf4" />

    * download [nodejs](https://nodejs.org/en/download) if it is not run in admin mode you will get this:

    <img width="640" height="501" alt="Screenshot 2025-09-25 212842" src="https://github.com/user-attachments/assets/9945217f-7b4a-4b97-87df-3b35b61ccc0a" />

    * in that case run an administrator command prompt and run the downloaded .msi file as follows:

    <img width="649" height="510" alt="Screenshot 2025-09-25 220442" src="https://github.com/user-attachments/assets/144cce1d-bc59-471c-ace3-aa38c52629e4" />
    <img width="640" height="498" alt="Screenshot 2025-09-25 214052" src="https://github.com/user-attachments/assets/affb20e8-785e-471d-98d9-855e5783ce5e" />

    * after succesful install of nodejs run ☑️ or ➡️ again. It might be needed to rebuild node_modules and package-lock.json if you see this

    <img width="640" height="688" alt="Screenshot 2025-09-25 214700" src="https://github.com/user-attachments/assets/b92ccba0-17e1-4434-929e-302ec8afd96e" />

    * Remove node_modules and package-lock.json - run as administrator if the OS complains!
    * Rebuild node_modules and package-lock.json by opening a vscode shell using [>_] in the statusbar
    * cd /interface 
    * npm install
    * If that give errors set execution policies as follows:

    <img width="640" height="398" alt="Screenshot 2025-09-25 215039" src="https://github.com/user-attachments/assets/629bc8de-d1ce-4d2d-9a92-e08bc9d31d4b" />

    * Press run ☑️ or ➡️ again it should now complete succesfully

    <img width="640" height="571" alt="Screenshot 2025-09-25 220023" src="https://github.com/user-attachments/assets/cd6754f4-d6df-446a-94fe-c3d7f491be59" />

    * Check serial output
  
	<img width="350" src="https://github.com/user-attachments/assets/c925d883-173b-4288-89d4-4770c2d86a02" />

## UI development server

Vite provides a development web server on your computer which connects to a running esp32-device (see [development-server](https://moonmodules.org/MoonLight/gettingstarted/#development-server)).

Activate it as follows:

    * configure vite.config.ts: set ip address of a running esp32-device
    * cd interface
    * npm run dev (see [changing MoonLight code](https://moonmodules.org/MoonLight/develop/installation/#changing-moonlight-code) to install nodejs)
    * A local webserver starts on [localhost](http://localhost:5173/). 
    * UI changes will directly be shown via this webserver without needing to flash it on an esp32-device.

## Troubleshooting
    * In general: first close and restart vscode and run ☑️ or ➡️ again.
    * python > 3.10: install python (3.11.13) in the vscode shell (not in a normal os terminal)
    * esptool.py not downloaded: deinstall platformIO IDE and install pioarduino IDE extensions (required for support of latest esp-idf 5.5)
    * Run everything from within VSCode, close any alternative / external tools which (potentially) access esp32 devices. They can claim resources or send commands to the device which interfere with what we trying to accomplish here.
    * Set WWWData.h right
        * Open platformIO new terminal (>_)
        * touch ./interface/src/app.html (or windows variant?) so the build process will be triggered to create a new WWWData.h
        * build the project (✔)
        * check in your github manager (gitkraken of github desktop) that a new WWWData.h is created
    * If the ESP32 device AP is not showing up in your WiFi list it might be helpful to fully erase the ESP32 device before flashing (vscode 👽, Erase flash)
    * Sometimes the Serial log may show: [  5817][W][WiFiGeneric.cpp:1408] setTxPower(): Neither AP or STA has been started. This is from setTxPower in APSettingsService. Delay has been added to prevent this.