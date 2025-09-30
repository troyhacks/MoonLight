
# Installation

The development environment consists of

* VSCode
    * Platformio IDE
    * nodejs
    * git
    * python

* Github manager
    * GitKraken (recommended)
    * GitHub Desktop
    * No Github manager (not recommended)

## Summary

Install VSCode with the platformIO IDE extension. Download the MoonLight repo. Open it in VSCode and build or upload to an ESP32 device.

!!! info "supporting tools"

     * Optionally you need to install git and nodejs, run npm install and make sure python > v3.10 in VSCode.
     * Instead of Platformio IDE, the pioarduino IDE extension might be needed in VSCode

## Install Visual Studio Code

**Step 1**: Download the MoonLight repository

* Use [GitKraken](https://www.gitkraken.com/download) or GitHub Desktop (or manually)
    * Create a folder to download to: e.g. Developer/GitGub/MoonModules/MoonLight
    * Press + / New tab and select Clone a Repo 
    * Copy [MoonLight.git](https://github.com/MoonModules/MoonLight.git) and paste in the URL field of GitKraken
    * Press clone the repo

    <img width="320" src="https://github.com/user-attachments/assets/74928dac-d59b-4489-b97b-759c6d792b77" />

    * The main branch will be checked out per default. To run / test latest developments, switch to the dev branch

**Step 2**: Download Visual Studio Code

* Windows: download from the Microsoft Store

    <img width="320" src="https://github.com/user-attachments/assets/ff6be5d6-40b2-48ae-9f17-89fc5bcfd848" />

* MacOS: [Visual Studio download](https://code.visualstudio.com/download)

**Step 3**: Install PlatformIO IDE

* Open the MoonLight repository folder created in step 1
* In VSCode, search for the extension

    <img width="320" src="https://github.com/user-attachments/assets/d91ab6b0-aeeb-42ed-8a85-d608b88c6103" />

* Install the extension. Please note it can take a while before Configuring project is finished. Do not cancel. Don't change anything in the code yet. Drink a ☕️.

    <img width="320" src="https://github.com/user-attachments/assets/ea953f62-0d94-499c-92f0-491d31c2edff" />

!!! info "pioarduino IDE"

     PlatformIO IDE is default but as we are using latest esp-idf, pioarduino IDE is needed when PlatformIO IDE fails.

**Step 4**: Build MoonLight

* Press ☑️ in the bottom statusbar

    <img width="320" src="https://github.com/user-attachments/assets/4e6faf4a-f169-46e4-a2f8-5d4021516a04" />

* When git is not installed on your system, [download git](https://git-scm.com/downloads) and install, restart VSCode and press ☑️ again

    * Windows:

        <img width="320" src="https://github.com/user-attachments/assets/8d39c637-c7dc-4c34-b177-f8eb63d54863" />
    
    * MacOS

        <img width="320" src="https://github.com/user-attachments/assets/b6c27943-6046-4fd7-aab4-3c91544f761d" />

!!! info "Install git on MacOS"

    * Install [homebrew](https://brew.sh): will take a while: ☕️
    * Install git using homebrew (replace <user> with your user name):
     
    ```
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

    echo >> /Users/<user>/.zprofile
    echo 'eval "$(/opt/homebrew/bin/brew shellenv)"' >> /Users/<user>/.zprofile
    eval "$(/opt/homebrew/bin/brew shellenv)"

    brew install git
    ```

* If you compile for the first time it take some time to finish (☕️)

    <img width="320" src="https://github.com/user-attachments/assets/c5655c00-6b91-4745-ae22-39f8573ae05a" />
    <img width="320" src="https://github.com/user-attachments/assets/2d21319d-2e86-473e-9e5a-d500ecb462c8" />

!!! warning "MacOS: Python and LZMA"

    In some cases LZMA support must be installed

    <img width="320" height="50" alt="Screenshot 2025-09-29 at 13 58 07" src="https://github.com/user-attachments/assets/2eef658f-f297-46cd-bbd5-4d769dbf9e56" />

    * Run in VSCode / pio shell ([>_])

    ```
    brew install xz

    python -V
    ```

    * python -V should show a version > 3.10 (e.g. 3.11.7)

**Step 5**: Upload MoonLight to an esp32-device

* Connect an ESP32-device via USB and select the device using the (second) 🔌 icon in the staturbar. See [Hardware](https://moonmodules.org/MoonLight/gettingstarted/hardware/).   

!!! info "USB-to-serial chip drivers"

    In some cases, ESP32-devices won't show connected. See USB-to-serial chip drivers at [MoonLight Web Installer](https://raw.githack.com/MoonModules/MoonLight/refs/heads/main/firmware/installer/index.html) 

* Erase the device if it is a new device or the device has not been used for MoonModules before: go to 👽 in the left menu and select Erase Flash

    <img width="350" src="https://github.com/user-attachments/assets/322bb30e-2709-43de-bd68-80044c9a0673" />

* Press upload (➡️) in the status bar

    <img width="350" src="https://github.com/user-attachments/assets/e4cbda64-739c-410d-9cdc-d2645e24c7ba" />

* The firmware is now flashed to your ESP32 device, after flashing the ESP32 device will reboot

!!! tip "Serial Monitor"
    Recommended: Press PlatformIO:Serial Monitor to see the debug information produced (the first 🔌 on the VSCode statusbar)

    <img width="350" src="https://github.com/user-attachments/assets/c925d883-173b-4288-89d4-4770c2d86a02" />

## Connect and setup MoonLight

Before changing code, test if the current download of MoonLight is running fine. Follow the instructions in [Connect MoonLight](https://moonmodules.org/MoonLight/gettingstarted/installation/#connect-moonlight) and [Setup MoonLight](https://moonmodules.org/MoonLight/gettingstarted/installation/#setup-moonlight)

Technical info:

* MoonLight has a cache expiration of one year. However if you are developing or updating nightly builds cached UI code might be outdated.

    * First time connect to an ESP32-device is generally smoothly
    * If the UI has changed, reconnect to the same device might result in the UI not showing up. See the instructions 'UI not showing when installing new version of MoonLight' in [Connect MoonLight](https://moonmodules.org/MoonLight/gettingstarted/installation/#connect-moonlight)

* A MoonLight device can be accessed via it's IP address or via [http://ml-home.local](http://ml-home.local). The latter uses MDNS.

!!! tip "ESP32Devices"
    IF you want to know which MoonLight (or WLED) devices run on your network, use [ESP32Devices](https://github.com/ewowi/ESP32Devices) to discover the ESP32 nodes on your network

## Prepare for development

nodejs is needed if changes on the frontend (UI) are made (interface folder). On each file change, ☑️ or ➡️ will rebuild the UI using nodejs.

* Windows: if nodejs is not on your system you will get this error:

    <img width="640" height="221" alt="Screenshot 2025-09-25 212255" src="https://github.com/user-attachments/assets/f05ca8d2-6485-4965-8b75-edd4b468ddf4" />

    * download [nodejs](https://nodejs.org/en/download) if it is not run in admin mode you will get this:

    <img width="640" height="501" alt="Screenshot 2025-09-25 212842" src="https://github.com/user-attachments/assets/9945217f-7b4a-4b97-87df-3b35b61ccc0a" />

    * if this is the case, run an administrator command prompt and run the downloaded .msi file as follows:

    <img width="649" height="510" alt="Screenshot 2025-09-25 220442" src="https://github.com/user-attachments/assets/144cce1d-bc59-471c-ace3-aa38c52629e4" />
    <img width="640" height="498" alt="Screenshot 2025-09-25 214052" src="https://github.com/user-attachments/assets/affb20e8-785e-471d-98d9-855e5783ce5e" />

    * after succesful install of nodejs run ☑️ or ➡️ again. It might be needed to rebuild node_modules and package-lock.json if you see this

    <img width="640" height="688" alt="Screenshot 2025-09-25 214700" src="https://github.com/user-attachments/assets/b92ccba0-17e1-4434-929e-302ec8afd96e" />

    * Remove node_modules and package-lock.json - run as administrator if the OS complains!
    * Rebuild node_modules and package-lock.json by opening a VSCode shell using [>_] in the statusbar
``` 
cd interface 
npm install
```
    * If that give errors set execution policies as follows:

    <img width="640" height="398" alt="Screenshot 2025-09-25 215039" src="https://github.com/user-attachments/assets/629bc8de-d1ce-4d2d-9a92-e08bc9d31d4b" />

    * Press run ☑️ or ➡️ again it should now complete succesfully

    <img width="640" height="571" alt="Screenshot 2025-09-25 220023" src="https://github.com/user-attachments/assets/cd6754f4-d6df-446a-94fe-c3d7f491be59" />

* MacOS

    * Go to [nodejs download](https://nodejs.org/en/download)
    * Get a prebuilt Node.js® for macOS running a ARM64 architecture (for modern Mx MacBooks)
    * Choose maxOS Installer (.pkg)
    * Next Next Finish Done
    * Press run ☑️ or ➡️ again it should now complete succesfully (if needed restart VSCode)
    * If still errors, Remove node_modules and package-lock.json, cd interface, npm install and repeat previous step

!!! info "Ignore DragDropList error"
    This error will show up in the logging
    ```
    node_modules/svelte-dnd-list/DragDropList.svelte (595:3): Error when using sourcemap for reporting an error: Can't resolve original location of error.
    ```
    Just ignore it.

* Check serial output
  
    <img width="350" src="https://github.com/user-attachments/assets/c925d883-173b-4288-89d4-4770c2d86a02" />

* Go to [development](https://moonmodules.org/MoonLight/develop/development/) for further info on developing MoonBase / MoonLight functionality.

## UI development server

To ease the frontend development you can deploy the back end code on an ESP32 board and pass the websocket and REST API calls through the development server's proxy running on your computer.

This very much speeds up UI development as no flashing to ESP32 is required to test updated UI. Svaing an UI file is enough to see the results!!!

See [Setup Proxy for Development](https://moonmodules.org/MoonLight/gettingstarted/#setup-proxy-for-development) and [development-server](https://moonmodules.org/MoonLight/gettingstarted/#development-server) how to setup.

!!! tip "nodejs" 
     if nodejs is not installed yet: see [Prepare for development](https://moonmodules.org/MoonLight/develop/installation/#prepare-for-development) to install nodejs

After configuring the development server, a local webserver starts on [localhost:5173](http://localhost:5173/). 

## Release and merged firmware binaries

Firmware binaries come in 2 flavours: including boot and partition (merged) and MoonLight code only (release). They are stored in the build folder of the MoonLight repo and updated each time a build or upload (☑️ or ➡️) is done. Subfolder merged contains the first type, release the second type.

* Merged bins are used by the web installer, release bins by the [System update](https://moonmodules.org/MoonLight/system/update/) module (OTA). System update uses the bins stored in [GitHub releases](https://github.com/MoonModules/MoonLight/releases).
* Merged bins starts flashing on address 0x0, release bins on address 0x10000.
* All MoonLight partition schemes have a firmware size of 3MB. Smaller devices (e.g. ESP32-D0) have no OTA partition. System update is possible in this situation, but there is no fallback if update fails (need to flash using USB in that case) 🚧

!!! tip "flash firmware using esptool"
     * [>_] in the statusbar of vscode
     ```
     esptool --port /dev/cu.usbmodem11201 write-flash 0x0 ./build/merged/MoonLight_esp32-s3-devkitc-1-n16r8v_0-5-9_webflash.bin
     ```
     * optionally add erase-flash before write-flash
     * use ./build/release/MoonLight_esp32-s3-devkitc-1-n16r8v_0-5-9.bin and address 0x10000 to flash only the MoonLight partition

## Troubleshooting
* In general: first close and restart VSCode and run ☑️ or ➡️ again.
* python > 3.10: install python (3.11.13) in the VSCode shell (not in a normal os terminal)
* esptool.py not downloaded: deinstall platformIO IDE and install pioarduino IDE extensions (required for support of latest esp-idf 5.5)
* Run everything from within VSCode, close any alternative / external tools which (potentially) access esp32 devices. They can claim resources or send commands to the device which interfere with what we trying to accomplish here.
* Set WWWData.h right
    * Open platformIO new terminal (>_)
    * touch ./interface/src/app.html (or windows variant?) so the build process will be triggered to create a new WWWData.h
    * build the project (✔)
    * check in your github manager (gitkraken of github desktop) that a new WWWData.h is created
* If the ESP32 device AP is not showing up in your WiFi list it might be helpful to fully erase the ESP32 device before flashing (VSCode 👽, Erase flash)
* Sometimes the Serial log may show: [5817][W][WiFiGeneric.cpp:1408] setTxPower(): Neither AP or STA has been started. This is from setTxPower in APSettingsService. Delay has been added to prevent this.