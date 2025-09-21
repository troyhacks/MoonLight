
# Develop Overview

* [Start developing for MoonLight](https://moonmodules.org/MoonLight/develop/overview/)
    * [ESP32 Sveltekit](https://moonmodules.org/MoonLight/esp32sveltekit/)

## Where you can help

* Improve the web installer
* Improve the UI (Svelte 5, DaisyUI 5, TailWind 4)
  * Mutli row layout
  * Set Dark / light theme or auto
* Tune FastLED, add FastLED 2D effects
* CI automation (Generate daily builds in Github and use by installer)

Contact us in the MoonLight channels on [Discord MoonModules](https://discord.gg/4rSTthvKHe)

## Developing

* Read the [ESP32 Sveltekit docs](https://moonmodules.org/MoonLight/esp32sveltekit/)
* Read [Customizing Sveltekit](https://moonmodules.org/MoonLight/develop/customizingsveltekit/)
* Instead of cloning the repo directly from MoonLight as described above, create a fork first so you have your own environment to make changes to. See also Pull Requests below.
* UI dev: configure vite.config.ts, go to interface folder, npm install, npm run dev. A local webserver starts on localhost. UI changes will directly be shown via this webserver
* Changes made to the UI are not always visible in the browser, issue with caching / max-age (🚧), clear the browser cache to see latest UI.
* Serial Log shows which code is from which library using emoji:

   <img width="500" alt="Screenshot 2025-06-07 at 12 09 06" src="https://github.com/user-attachments/assets/9ac673d3-6303-40ee-b2a0-26a0befbda01" />

    * 🐼: ESP-SvelteKit
    * 🔮: PsychicHTTP
    * 🐸: Live Scripts
    * 🌙: MoonBase
    * 💫: MoonLight
    🌙 and 💫 is also used in code comments of ESP32-SvelteKit to show where changes to upstream have been made.
* The following ESP32-SvelteKit features have been switched off in the default builts (they can be switched on if you want to use them, see [features.ini](https://github.com/MoonModules/MoonLight/blob/main/features.ini))
    *   -D FT_SECURITY=0
    *   ~~-D FT_SLEEP=1~~ enabled!
    *   ~~-D FT_BATTERY=1~~ enabled!

## Installation - Developer

* Open GitKraken (or any other GitHub client). Press + / New tab and select Clone a Repo (Or another git management tool)
* Select the folder on your local drive where to copy to (e.g. /github/ewowi)
* Copy [MoonLight.git](https://github.com/MoonModules/MoonLight.git) and paste in the URL field of GitKraken
* Press clone the repo
* Open VSCode
* Install the PlatformIO IDE extension, if not already done so.
* Open the folder created with GitKraken
* Connect an ESP32 or an ESP32-S3 with USB to your computer
* On the status bar select the env to flash and the board to flash to
    * Select esp32dev for a normal ESP32
    * Select esp32-s3-devkitc-1-n16r8v for an ESP32-S3 ([recommended](https://s.click.aliexpress.com/e/_DBAtJ2H) or similar)

   <img width="617" src="https://github.com/user-attachments/assets/349af246-30c7-45dd-92ed-4f2b3900557f" />

* Press PlaformIO:Upload (->) on the statusbar
* The firmware is now flashed to your board, after flashing the board will reboot
* Recommended: Press PlatformIO:Serial Monitor to see the debug information produced

* Issues
    * If the board AP is not showing up in your WiFi list it might be helpful to fully erase the board before flashing (vscode 👽, Erase flash)
    * Sometimes the Serial log may show: [  5817][W][WiFiGeneric.cpp:1408] setTxPower(): Neither AP or STA has been started. This is from setTxPower in APSettingsService. Delay has been added to prevent this.

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

