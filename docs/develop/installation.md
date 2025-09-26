
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
        * Select esp32dev for a normal ESP32
        * Select esp32-s3-devkitc-1-n16r8v for an ESP32-S3 ([recommended](https://s.click.aliexpress.com/e/_DBAtJ2H) or similar)

       <img width="617" src="https://github.com/user-attachments/assets/349af246-30c7-45dd-92ed-4f2b3900557f" />

    * Press upload (➡️) in the status bar

    <img width="640" height="348" alt="Screenshot 2025-09-25 210622" src="https://github.com/user-attachments/assets/a55b69a1-7732-4bb5-9afe-4e55518db651" />

    * The firmware is now flashed to your board, after flashing the board will reboot
    * Recommended: Press PlatformIO:Serial Monitor to see the debug information produced

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
    * Rebuild node_modules and package-lock.json by opening a vscode shell using [>_] in the statusbar, going to the interface folder and press npm install
    * If that give errors set exetution policies as follows:

    <img width="640" height="398" alt="Screenshot 2025-09-25 215039" src="https://github.com/user-attachments/assets/629bc8de-d1ce-4d2d-9a92-e08bc9d31d4b" />

    * Press run ☑️ or ➡️ again it should now complete succesfully

    <img width="640" height="571" alt="Screenshot 2025-09-25 220023" src="https://github.com/user-attachments/assets/cd6754f4-d6df-446a-94fe-c3d7f491be59" />

* Troubleshooting
    * In general: first close and restart vscode and run ☑️ or ➡️ again.
    * python > 3.10: install python (3.11.13) in the vscode shell (not in a normal os terminal)
    * esptool.py not downloaded: deinstall platformIO IDE and install pioarduino IDE extensions

## Preconditions for succesful build/upload and browser access:

Normally / Ideally this is not needed, but it can happen that not things are missing / not updated in a commit or merge (especially)

* Set platform.json right
    * cd /interface
    * run npm install
* Set WWWData.h right
    * Open platformIO new terminal (>_)
    * touch ./interface/src/app.html (or windows variant?) so the build process will be triggered to create a new WWWData.h
    * build the project (✔)
    * check in your github manager (gitkraken of github desktop) that a new WWWData.h is created
* Deinstall platformio IDE and install pioarduino IDE (required for support of latest esp-idf 5.5)
* Follow the steps 'In pictures' below.
* Connect to your local WiFi network. Use the AP of the browser only to connect to your network.
    * Don't use the device AP for further testing.
* Empty browser caches
    * Check [how to hard refresh your browser and clear cache](https://fabricdigital.co.nz/blog/how-to-hard-refresh-your-browser-and-clear-cache)
    * MoonLight has a cache expiration of one year. However if you are developing or updating nightly builds cached UI code might be outdated.
    * Chrome: 
        * Delete Browsing data / delete data
        * Chrome on Mac: Command Shift R
        * Chrome on Windows: Ctrl Shift R or Control Reload
    * Safari: 
        * Empty cache: Develop (enable if not done yet) / empty caches (Command Option E)
        * Reload from Origin : Option Shift R
    * After this not all UI elements might be immediately visible. Just wait a bit.
* Run everything from within VSCode, close any alternative / external tools which (potentially) access esp32 boards. They can claim resources or send commands to the board which interfere with what we trying to accomplish here.
  
## In pictures:

* Step 1: Erase ESP32 device:
  
	<img width="350" src="https://github.com/user-attachments/assets/322bb30e-2709-43de-bd68-80044c9a0673" />

* Step 2: Flash to ESP32 device
  
	<img width="350" src="https://github.com/user-attachments/assets/e4cbda64-739c-410d-9cdc-d2645e24c7ba" />

* Step 3: Check serial output
  
	<img width="350" src="https://github.com/user-attachments/assets/c925d883-173b-4288-89d4-4770c2d86a02" />

* Step 4: Connect to the WiFi AP of the ESP32 device: captive portal or via http://4.3.2.1
  
	<img width="350" src="https://github.com/user-attachments/assets/20b9f7fe-ab3d-4b8d-b20a-478129d79c6f" />

* Step 5: Check AP settings

	<img width="350" src="https://github.com/user-attachments/assets/a8d89d7a-d4e6-4443-b2c2-997c0ac89381" />
  
* Step 6: Change hostname and enter WiFi credentials

	<img width="350" src="https://github.com/user-attachments/assets/81aab0c6-5d60-4ac9-8aee-54a7147be46e" />

* Step 7: Add an effect

	<img width="350" src="https://github.com/user-attachments/assets/6cc3749c-983a-4659-9c63-fab173b4750c" />

* Step 8: Add a layout and set pin(s): Monitor shows effect

	<img width="350" src="https://github.com/user-attachments/assets/d83277bd-4bc1-495c-ae21-85efe41ab8b1" />

* Step 9: Add a driver: LEDs shows effect

	<img width="350" src="https://github.com/user-attachments/assets/67447f55-7a22-41ab-af8c-0a2e98462792" />
  
* Issues
    * If the board AP is not showing up in your WiFi list it might be helpful to fully erase the board before flashing (vscode 👽, Erase flash)
    * Sometimes the Serial log may show: [  5817][W][WiFiGeneric.cpp:1408] setTxPower(): Neither AP or STA has been started. This is from setTxPower in APSettingsService. Delay has been added to prevent this.

## Pull Requests

* Want to make changes: fork the repo and submit pull requests, see [creating-a-pull-request-from-a-fork](https://docs.github.com/en/pull-requests/collaborating-with-pull-requests/proposing-changes-to-your-work-with-pull-requests/creating-a-pull-request-from-a-fork):

    * Login to your own github account
    * Fork: go to [MoonModules/MoonLight](https://github.com/MoonModules/MoonLight/) and press Fork, uncheck 'Copy the main branch only' and press Create Fork. You will be moved to your fork of MoonLight
    * Press Branches, press New Branch, give it a name e.g. background-script and press Create new Branch, click on background-script
      
      <img width="90" src="https://github.com/user-attachments/assets/588d0854-bac1-4b70-8931-ba6db4c94248" />

    * Go to the file you want to change press edit and make the changes. E.g. change executable.execute("main") to executable.executeAsTask("main")
    * Press Commit Changes..., enter a commit message and an extended description, Press Commit Changes
    * Go back to the homepage of your fork [myfork/MoonLight](https://github.com/ewoudwijma/MoonLight). There is a message inviting to create a Pull Request. Press Compare & pull request.
      
      <img width="350" alt="Screenshot 2025-04-15 at 14 59 15" src="https://github.com/user-attachments/assets/410aa517-99eb-4907-b1a3-db7f38abb194" />
  
    * Add a title and Description to the Pull Request and press Create Pull Request
  
    * The upstream MoonLight repo can now process this PR

## Adding functionality

### UI
🚧

* nodejs
* /interface folder
* npm install and npm run dev
* WWWData.h

### Server

There are 3 levels to add functionality:

* **Standard ESP32-Sveltekit code**, e.g. Connections, Wifi and System. MoonBase files is also made using standard sveltekit as example but contains a few components used in MoonLight modules. Might be rewriteen as MoonLight Module in the future.
* [MoonLight Modules](https://moonmodules.org/MoonLight/moonbase/modules/) e.g. Lights Control, Effects, Info, Channels. They are subclasses of Modules.h/cpp and implement setupDefinition, onUpdate and optional loop. New modules need to be defined in main.cpp and added to menu.svelte. All further UI is generated by Module.svelte.
* **MoonLight Nodes**: the easiest and recommended way. See Effects.h, Layouts.h, Modifiers.h and Mods.h for examples. They match closest WLED usermods. Each node has controls, a setup and a loop and can be switched on and off. For specific purposes hasLayout and hasModifier can be set.

### Adding a Board Definition

Before starting, ensure you have the datasheet of your particular chip and board confirmed and available. Many modules have near-identical markings that can hide varying hardware.

There are 3 files to consider when making a board definition.

	boards/BOARD_NAME.csv
	boards/BOARD_NAME.JSON
	firmware/BOARD_TYPE_NAME.ini (e.g. esp32dev, esp32-s3), contains different boards

### Technical notes

#### Live Scripts

* Uses ESPLiveScripts, see compileAndRun. compileAndRun is started when in Nodes a file.sc is choosen
    * To do: kill running scripts, e.g. when changing effects

#### File Manager

## Technical

* filesState: all files on FS
* folderList: all files in a folder
* editableFile: current file
* getState / postFilesState: get filesState and post changes to files (update, delete, new)
* addFile / addFolder: create new items
* breadcrumbs(String): folder path as string array and as string, stored in localStorage
* folderListFromBreadCrumbs: create folderList of current folder
* handleEdit: when edit button pressed: navigate back and forward through folders, edit current file
* confirmDelete: when delete button pressed
* socket files / handleFileState (->folderListFromBreadCrumbs)

Using component FileEdit, see [Components](https://moonmodules.org/MoonLight/components/#fileedit)

