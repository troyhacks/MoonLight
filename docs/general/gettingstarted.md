# Getting started

## Installation - Developer

* Open GitKraken Press + / New tab and select Clone a Repo (Or another git management tool)
* Select the folder on your local drive where to copy to (e.g. /github/MoonModules)
* Copy [MoonBase.git](https://github.com/ewowi/MoonBase.git) or [MoonLight.git](https://github.com/MoonModules/MoonLight.git) and paste in the URL field of GitKraken
* Press clone the repo
* Open VSCode
* Install the PlatformIO IDE extension
* Open the folder created with GitKraken
* Connect an ESP32 or an ESP32-S3 with USB to your computer
* On the status bar select the env to flash and the board to flash to
    * Select esp32dev for a normal ESP32
    * Select esp32-s3-devkitc-1-n16r8v for an ESP32-S2 ([recommended](https://s.click.aliexpress.com/e/_DBAtJ2H) or similar)

   <img width="617" alt="image" src="https://github.com/user-attachments/assets/349af246-30c7-45dd-92ed-4f2b3900557f" />

* Press PlaformIO:Upload (->) on the statusbar
* The firmware is now flashed to your board, after flashing the board will reboot
* Recommended: Press PlatformIO:Serial Monitor to see the debug information produced

## Installation - End user

[install](https://github.com/ewowi/MoonBase/blob/main/docs/general/index.html)

* release 0.5.4, for latest build follow installation developer
* This is WIP, looks like issue with erasing the board and setting the partitions right ...
* Made using [esp-web-tools](https://esphome.github.io/esp-web-tools/)

## Configuration

* In case of a newly flashed board, a Wifi access point (AP) will be created. Go to your Wifi settings on your computer and find the new Wifi AP (starts with MoonBase, MoonLight or ESPSvelteKit)
    * If the board AP is not showing up in your WiFi list it might be helpful to fully erase the board before flashing (vscode 👽, Erase flash)
* Connect to the AP (no password needed), a captive portal will show with a welcome screen.
    * There seems to be an issue in the captive portal showing Connection to device lost repeatedly. In that case, close the captive portal and open the app in a brower using 192.168.4.1
* Go to the menu and select Wifi / Wifi Station
* Press (+) and enter the SSID and the password of your Wifi Network
* Press Add Network and Apply settings
* The board will reconnect to your Wifi Network
* Go back to your Wifi Network on your computer
* Find out the new IP of the board, or in your Wifi Network settings or by looking at the log in the Serial Monitor (see above)
* (sometimes it takes a while to load pages for the first time, might be related to caching of javascript, on the issuelist)

## Developing

* Read the [ESP32 Sveltekit docs](https://ewowi.github.io/MoonBase/esp32sveltekit/)
* Read [Customizing Sveltekit](https://ewowi.github.io/MoonBase/general/customizingsveltekit/)
* UI dev: configure vite.config.ts, go to interface folder, npm install, npm run dev. A local webserver starts on localhost. UI changes will directly be shown via this webserver
* Changes made to the UI are not always visible in the browser, issue with caching / max-age (WIP), clear the browser cache to see latest UI.
* Want to make changes: fork the repo and submit pull requests, see [creating-a-pull-request-from-a-fork](https://docs.github.com/en/pull-requests/collaborating-with-pull-requests/proposing-changes-to-your-work-with-pull-requests/creating-a-pull-request-from-a-fork):
    * Login to your own github account
    * Fork: go to [ewowi/MoonBase](https://github.com/ewowi/MoonBase/) and press Fork, uncheck 'Copy the main branch only' and press Create Fork. You will be moved to your fork of MoonBase
    * Press xx Branches, press New Branch, give it a name e.g. background-script and press Create new Branch, click on background-script
      
      <img width="90" alt="image" src="https://github.com/user-attachments/assets/588d0854-bac1-4b70-8931-ba6db4c94248" />

    * Go to the file you want to change e.g. [ModuleAnimations.h](https://github.com/ewoudwijma/MoonBase/blob/background-script/src/custom/ModuleAnimations.h), press edit and make the changes. E.g. change executable.execute("main") to executable.executeAsTask("main")
    * Press Commit Changes..., enter a commit message and an extended description, Press Commit Changes
    * Go back to the homepage of your fork [myfork/MoonBase](https://github.com/ewoudwijma/MoonBase). There is a message inviting to create a Pull Request. Press Compare & pull request.
      
      <img width="350" alt="Screenshot 2025-04-15 at 14 59 15" src="https://github.com/user-attachments/assets/410aa517-99eb-4907-b1a3-db7f38abb194" />
  
    * Add a title and Description to the Pull Request and press Create Pull Request
    * Note: coderabbit has been installed on this repo, adding intelligent comments in the PR
      
      <img width="350" alt="Screenshot 2025-04-15 at 15 02 33" src="https://github.com/user-attachments/assets/53bc8b2e-a078-46a5-b926-25d581ec8202" />
  
    * The upstream repo can now process this PR

## MoonBase specific

* Connect a 256 leds panel to the board
* Search for FastLED.addLeds in the code and update to the pin you use to drive LEDS, reflash the code to the board
* Go to the UI in the browser
* Go to Custom / Files and create or upload Live scripts 
* Go to Custom / Module Animations and select the 'hardcoded animations' (Random, Sinelon, Rainbow), you should see it on your panel
* Select any of the Live scripts you uploaded, check the Serial Output for results. (No led output in current version, see below)
* Open the Edit area and change things in the code, see the Serial Output for results

<img width="350" alt="image" src="https://github.com/user-attachments/assets/56bdd019-927b-40cc-9199-9bc6344f8d8b" />


* To do
    * Find a way to present feedback to the UI (e.g. error messages as comments in the sc file)
    * Extend the code in [ModuleAnimations.h](https://github.com/ewowi/MoonBase/blob/main/src/custom/ModuleAnimations.h) to support led animations ('homework assignment')
    * see ModuleAnimation.h (Server)

### Current script supported

```cpp
void setup() {
  printf("Run Live Script good afternoon\n");
}
void main() {
  setup();
}
```

### Homework assignment

```cpp
void setup() {
  printf("Run Live Script good morning\n");
}
void loop() {
  fadeToBlackBy(40);
  leds[random16(255)] = CRGB(0, 0, 255);
}
void main() {
  setup();
  while (true) {
    loop();
    sync(); //or show??
  }
}
```
