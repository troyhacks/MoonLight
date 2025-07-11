# MoonLight Development

## Developing

* Read the [ESP32 Sveltekit docs](https://moonmodules.org/MoonLight/esp32sveltekit/)
* Read [Customizing Sveltekit](https://moonmodules.org/MoonLight/general/customizingsveltekit/)
* Instead of cloning the repo directly from MoonLight as described above, create a fork first so you have your own environment to make changes to. See also Pull Requests below.
* UI dev: configure vite.config.ts, go to interface folder, npm install, npm run dev. A local webserver starts on localhost. UI changes will directly be shown via this webserver
* Changes made to the UI are not always visible in the browser, issue with caching / max-age (WIP), clear the browser cache to see latest UI.
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
    *   -D FT_SLEEP=0
    *   -D FT_BATTERY=0

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
    * Note: coderabbit has been installed on this repo, adding intelligent comments in the PR
      
      <img width="350" alt="Screenshot 2025-04-15 at 15 02 33" src="https://github.com/user-attachments/assets/53bc8b2e-a078-46a5-b926-25d581ec8202" />
  
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
* [MoonLight Modules](https://moonmodules.org/MoonLight/moonbase/modules/) e.g. Light Control, Editor, Info, Channel View. They are subclasses of Modules.h/cpp and implement setupDefinition, onUpdate and optional loop. New modules need to be defined in main.cpp and added to menu.svelte. All further UI is generated by Module.svelte.
* **MoonLight Nodes**: the easiest and recommended way. See Effects.h, Layouts.h, Modifiers.h and Mods.h for examples. They match closest WLED usermods. Each node has controls, a setup and a loop and can be switched on and off. For specific purposes hasLayout and hasModifier can be set.

### Adding a Board Definition

Before starting, ensure you have the datasheet of your particular chip and board confirmed available. Many modules have near-identical markings that can hide varying hardware.

There are 3 files to consider when making a board definition.

	boards/BOARD_NAME.csv
	boards/BOARD_NAME.JSON
	variants/BOARD_NAME.ini

🚧