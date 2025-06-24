# Getting started

## Installation - Web installer

* **Step 1**: Get an **ESP32 or ESP32-S3** (preferred, support large setups and Live Scripts). Boards we use are [ESP32](https://s.click.aliexpress.com/e/_EyrmQyw) or [ESP32-S3](https://s.click.aliexpress.com/e/_DBAtJ2H), but most of the ESP32 and ESP32-S3 boards will work. Get a **WS2812 LED-strip** or a [LED-panel](https://s.click.aliexpress.com/e/_EGx8Hhu) (preferred to see 2D effects). Connect the strip or panel to the board on a suitable pin (e.g. Pin 2 or 16).

* **Step 2**: Connect an ESP32(-S3) via USB

* **Step 3**: Click this link to go to the web installer: [ESP Web Installer for MoonLight v0.5.6](https://raw.githack.com/MoonModules/MoonLight/refs/heads/main/docs/general/installer/index.html). The Web Installer works on Google Chrome or Microsoft Edge or similar browsers. Safari does not support installing things on ESP devices. Driver software might be needed to have the webinstaller recognise the board. The installer detects the board automatically. If you try to install and no boards are detected, a screen will popup inviting you to install the driver. Press Connect. You should see the ESP32 board (if not sure disconnect and connect and see which serial port is used):

   <img width="350" alt="Screenshot 2025-06-07 at 20 54 17" src="https://github.com/user-attachments/assets/f092743f-a362-40da-b932-d31b203d966d" />

* **Step 4**: Connect to the board, then select **Install** MoonLight

   <img width="200" alt="Screenshot 2025-06-07 at 20 54 31" src="https://github.com/user-attachments/assets/a0a8d92c-dae2-4cfe-ac78-bc7cacb24724" />

* **Step 5**: Choose to **erase** the board first or not. Erase if you have a new board or want to start fresh. Do not erase if you want to update an existing MoonLight installation. Erase deletes all settings and requires to re-connect to WiFi.

   <img width="250" alt="Screenshot 2025-06-07 at 20 54 42" src="https://github.com/user-attachments/assets/35045317-520d-427c-894e-418693877831" />

* **Step 6**: Confirm and **install**
  
   <img width="200" alt="Screenshot 2025-06-07 at 20 54 51" src="https://github.com/user-attachments/assets/25c05a6c-d2ef-41cb-b83c-fc40a60c6ccc" />
   <img width="200" alt="Screenshot 2025-06-07 at 20 55 00" src="https://github.com/user-attachments/assets/b2391752-51c4-400e-b95c-4fa865e93595" />
   <img width="200" alt="Screenshot 2025-06-07 at 20 57 30" src="https://github.com/user-attachments/assets/5e2ceefd-4c31-4b72-a228-f29373b677ac" />

* **Step 7**: Optionally Press **Logs and Console**

   <img width="350" alt="Screenshot 2025-06-07 at 20 57 54" src="https://github.com/user-attachments/assets/9ac753dc-93b7-4f79-9419-0c81d1a4bc26" />

   You can see the serial logging of the board. In case you see nothing you can press reset board, now you should see the logging.

* **Step 8**: Make a **WiFi connection** to the board, it should present itself in the list of WiFi access points.
  
   <img width="200" alt="Screenshot 2025-06-07 at 20 58 22" src="https://github.com/user-attachments/assets/08894be5-e4ed-4ed8-b2ae-86a6ce5c9ef6" />

* **Step 9**: After connecting, MoonLight will show up in a **web browser** (all browsers supported). A new install will take some time to show up (up to a minute or sometimes 2). On some browsers, e.g. Safari, MoonLight pops up showing Connection to device lost repeatedly (bug). In that case, close this browser window and open MoonLight in a brower using 192.168.4.1 as URL. You will see this screen, select WiFi Station.
  
   <img width="400" alt="Screenshot 2025-06-07 at 20 59 19" src="https://github.com/user-attachments/assets/3d8bddfb-9f95-4317-891c-be8b90880541" />

* **Step 10**: Enter your local **WiFi network credentials**:

   <img width="400" alt="Screenshot 2025-06-07 at 20 59 34" src="https://github.com/user-attachments/assets/38f6cb86-e44c-478e-9910-1b0614c99292" />

* **Step 11**: Press **Add Network** and apply settings. After connecting to your local WiFi network you will see this screen showing the new IP address of your MoonLight board. If you don't know the IP address of your board anymore you can go to step 3 and 4 and step 7 and check the IP address in the Serial logging. Advanced tip: Use [ESP32Instances](https://github.com/ewowi/ESP32Instances) to discover the ESP32 nodes on your network (using nodeJS and html)

   <img width="400" alt="Screenshot 2025-06-07 at 20 59 55" src="https://github.com/user-attachments/assets/3696f3b0-70f7-4be8-a310-948b003450b7" />

* **Step 12**: Go to your WiFi settings and connect to your local WiFi network and enter the IP address of your MoonLight board in a browser and go to **MoonLight Editor**. Add a layout Node by pressing the blue + button and select Panel 🚥. Scroll down to the Pin Control field and enter the pin number you connected the strip or panel to in Step 1. After setting the right pin the strip or panel should show lights.

   <img width="400" alt="Screenshot 2025-06-07 at 21 01 02" src="https://github.com/user-attachments/assets/5d4d581f-5586-43c0-a5ea-02930effc3d8" />

* **That's it**. You can now change effect nodes, or add nodes. For audio add the AudioSync node. More how to's will be added. At the moment:
    * Check the help pages on this documentation site, especially the [editor page](https://moonmodules.org/MoonLight/moonbase/module/editor/)
    * press the ? in MoonLight to go directly to the relevant page
   More how to's will be added. At the moment:
    * Live scripts: [how to run a live script](https://github.com/MoonModules/MoonLight/blob/main/docs/moonbase/module/liveScripts.md#how-to-run-a-live-script)

* Made using [esp-web-tools](https://esphome.github.io/esp-web-tools/)
* See also [ESP-Web-Tools-Tutorial](https://github.com/witnessmenow/ESP-Web-Tools-Tutorial/blob/main/README.md)

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

## MoonLight specific

* Buy some hardware e.g.:
    * [ESP32-S3 N16R8](https://s.click.aliexpress.com/e/_DBAtJ2H)
    * [256 led panel](https://s.click.aliexpress.com/e/_EIKoYrg)
    * [Expansion board 44pin](https://s.click.aliexpress.com/e/_EJhmlIE)
* For latest DMX support 
    * [Pknight Artnet DMX 512](https://s.click.aliexpress.com/e/_ExQK8Dc)
    * [Toy Moving heads](https://s.click.aliexpress.com/e/_Eju7k6Q) (warning: projected lights show the individual rgb colors)
    * [Recommended moving heads](https://moonmodules.org/hardware/#moving-heads)
  
   <img width="350" src="https://github.com/user-attachments/assets/1623a751-5f4b-463b-a6f3-a642c2bc52bf"/>
   
* Live Scripts: Go to MoonBase / Files and create or upload Live scripts.
    * Examples: [Live scripts](https://github.com/MoonModules/MoonLight/tree/main/misc/livescripts)
    * Back to Editor, select any of the Live scripts you uploaded, check the Led Panel and / or Serial Output for results
    * Open the Edit area and change things in the code, see the Serial Output for results

<img width="350" src="https://github.com/user-attachments/assets/56bdd019-927b-40cc-9199-9bc6344f8d8b" />

* Issues
    * If the board AP is not showing up in your WiFi list it might be helpful to fully erase the board before flashing (vscode 👽, Erase flash)
    * Sometimes the Serial log may show: [  5817][W][WiFiGeneric.cpp:1408] setTxPower(): Neither AP or STA has been started. This is from setTxPower in APSettingsService. Delay has been added to prevent this. 


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
