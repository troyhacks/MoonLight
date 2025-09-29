# Installation

## MoonLight Web installer

Install Moonlight onto an ESP32 using the MoonLight Web installer

**Step 1**: Get an **ESP32** and a **WS2812 LED-strip** or a [LED-panel](https://s.click.aliexpress.com/e/_EGx8Hhu). See [Hardware](https://moonmodules.org/MoonLight/gettingstarted/hardware/).

**Step 2**: Connect an ESP32 via USB

**Step 3**: Go to the [MoonLight Web Installer](https://raw.githack.com/MoonModules/MoonLight/refs/heads/main/firmware/installer/index.html). Select your ESP32-device, or if not listed, press others.

!!! info "Browser support"

    The Web Installer works on Google Chrome or Microsoft Edge or similar browsers, not on Safari.

**Step 4**: Connect to the ESP32-device

   <img width="350" alt="Screenshot 2025-06-07 at 20 54 17" src="https://github.com/user-attachments/assets/f092743f-a362-40da-b932-d31b203d966d" />

**Step 4**: Select **Install MoonLight**

   <img width="200" alt="Screenshot 2025-06-07 at 20 54 31" src="https://github.com/user-attachments/assets/a0a8d92c-dae2-4cfe-ac78-bc7cacb24724" />

**Step 6**: Optionally **erase** the ESP32-device

!!! tip "Erase"

    Erase if you have a new ESP32-device or want to start fresh. Do not erase if you want to update an existing MoonLight installation. Erase deletes all settings and requires to re-[connect to MoonLight](/#connect-moonlight).

   <img width="250" alt="Screenshot 2025-06-07 at 20 54 42" src="https://github.com/user-attachments/assets/35045317-520d-427c-894e-418693877831" />

**Step 7**: Confirm and **install**
  
   <img width="200" alt="Screenshot 2025-06-07 at 20 54 51" src="https://github.com/user-attachments/assets/25c05a6c-d2ef-41cb-b83c-fc40a60c6ccc" />
   <img width="200" alt="Screenshot 2025-06-07 at 20 55 00" src="https://github.com/user-attachments/assets/b2391752-51c4-400e-b95c-4fa865e93595" />
   <img width="200" alt="Screenshot 2025-06-07 at 20 57 30" src="https://github.com/user-attachments/assets/5e2ceefd-4c31-4b72-a228-f29373b677ac" />

**Step 8**: Optionally Press **Logs and Console**

   <img width="350" alt="Screenshot 2025-06-07 at 20 57 54" src="https://github.com/user-attachments/assets/9ac753dc-93b7-4f79-9419-0c81d1a4bc26" />

!!! tip "Logging"
    You can see the serial logging of the ESP32-device. In case you see nothing you can press reset device, now you should see the logging.

## Connect MoonLight

**Step 1**: Make a **WiFi connection** to the ESP32-device, it should present itself in the list of WiFi access points.
  
   <img width="200" alt="Screenshot 2025-06-07 at 20 58 22" src="https://github.com/user-attachments/assets/08894be5-e4ed-4ed8-b2ae-86a6ce5c9ef6" />

**Step 2**: After connecting, MoonLight will show up in a **web browser** (all browsers supported). You will see this screen, select WiFi Station.
  
   <img width="400" alt="Screenshot 2025-06-07 at 20 59 19" src="https://github.com/user-attachments/assets/3d8bddfb-9f95-4317-891c-be8b90880541" />

!!! warning "UI not showing when installing new version of MoonLight"

    If you ran previous versions of MoonLight, the UI might not show up if it has been changed since then. In this case it is needed to reload the UI from the ESP32-device

    * Chrome: Command Shift R (Mac) or Ctrl Shift R or Control Reload (Windows)
    * Safari: Reload from Origin : Option Shift R
    * After this not all UI elements might be immediately visible. If you see 'MoonLight loading ...' in the browser, just wait a bit (☕️)

    * Sometimes it migh be needed to clear the browser cache:
        * Chrome: Delete Browsing data / delete data (caching data)
        * Safari: Empty cache: Menu Develop (enable if not visible in the Safari Menu) / empty caches (Command Option E)

        * More info: [how to hard refresh your browser and clear cache](https://fabricdigital.co.nz/blog/how-to-hard-refresh-your-browser-and-clear-cache)

   * The device will show in 'Captive portal' mode. Alternatively you can close the captive portal and show it in a browser using [http://4.3.2.1](http://4.3.2.1)

**Step 3**: Enter your local **WiFi network credentials** and hostname:

   <img width="400" src="https://github.com/user-attachments/assets/81aab0c6-5d60-4ac9-8aee-54a7147be46e" />

!!! tip "Hostname"

    The hostname will be used to access your device. E.g. if the hostname is ML-home you can access it using [http://ml-home.local](http://ml-home.local)

**Step 4**: Press **Add Network** and apply settings. After connecting to your normal WiFi network you will see this screen showing the new IP address of your MoonLight ESP32-device. 

   <img width="400" alt="Screenshot 2025-06-07 at 20 59 55" src="https://github.com/user-attachments/assets/3696f3b0-70f7-4be8-a310-948b003450b7" />

**Step 5**: When connected to youe local WiFi network, enter the hostname in a browser (e.g. http://ml-home.local)

!!! tip "IP address"
    Alternatively you can use the IP address of the device to connect. 

!!! warning "reload UI"
    See step 2 if UI is not showing up: reload UI from ESP32-devices

## Setup MoonLight

**Step 1**: Add an effect

   <img width="350" src="https://github.com/user-attachments/assets/6cc3749c-983a-4659-9c63-fab173b4750c" />

**Step 2**: Add a layout and set pin(s): Monitor shows effect

   <img width="350" src="https://github.com/user-attachments/assets/d83277bd-4bc1-495c-ae21-85efe41ab8b1" />

* Add a layout Node by pressing the blue + button and select Panel 🚥. Scroll down to the Pin Control field and enter the pin number you connected the strip or panel to. 

!!! info "Monitor"
    The Monitor should now show the effect

**Step 3**: Add a driver: LEDs shows effect

   <img width="350" src="https://github.com/user-attachments/assets/67447f55-7a22-41ab-af8c-0a2e98462792" />

* Add a driver node
* In some cases restart the device to make layout changes effective is needed.
    
!!! tip "Choose driver"
    FastLED driver is best to start with if you have a normal (ws2812) LED strip or panel. Choose the Physical driver if you have more then 4 LED strips or panels or non standard LEDS (e.g. RGBW lights, curtains...).
  
**Step 4**: Press save (💾). Saves your setup to the file system, so if a device is restarted, your settings are still there.

   <img width="400" src="https://github.com/user-attachments/assets/17fe6f38-375b-4d96-923b-607897e7d4db" />

!!! info "Next steps"
    * You can now change effect or add effects, modifiers, layouts and drivers
    * Go to [MoonLigh Overview](https://moonmodules.org/MoonLight/moonlight/overview/)
    * press the ? in MoonLight to go directly to the relevant page

## Update MoonLight

* The preferred way to update MoonLight is via [System update](https://moonmodules.org/MoonLight/system/update/)

   <img width="350" src="https://github.com/user-attachments/assets/9d6452fc-9bcd-4efa-8839-30b3fd015276" />

   * Download from GitHub - Currently not working due to certificate issue... 🚧
   * Upload from file. From a [GitHub release](https://github.com/MoonModules/MoonLight/releases), or created by VSCode, see [Develop / Installation](https://moonmodules.org/MoonLight/develop/installation/)

* To install the latest release, you can also use the [MoonLight Web Installer](https://raw.githack.com/MoonModules/MoonLight/refs/heads/main/firmware/installer/index.html) (no need to erase the device if updating)
* Upload directly from VSCode, see [Develop / Installation](https://moonmodules.org/MoonLight/develop/installation/)