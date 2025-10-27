# Installation

<iframe width="560" height="315" src="https://www.youtube.com/embed/7DQOEWa-Kwg" frameborder="0" allowfullscreen></iframe>

## MoonLight Web installer

Install Moonlight onto an ESP32 microcontroller using the MoonLight Web installer

**Step 1**: Get an **ESP32** and a **WS2812 LED-strip** or **LED-panel**. See [Hardware](https://moonmodules.org/MoonLight/gettingstarted/hardware/).

**Step 2**: Connect the ESP32 via USB

**Step 3**: Go to the [MoonLight Web Installer](https://raw.githack.com/MoonModules/MoonLight/refs/heads/main/firmware/installer/index.html). Select your ESP32-device, or if not listed, press others.

!!! info "Browser support"

    The Web Installer works on Google Chrome or Microsoft Edge or similar browsers, not on Safari.

**Step 4**: Connect to the ESP32-device

   <img width="350" src="https://github.com/user-attachments/assets/f092743f-a362-40da-b932-d31b203d966d" />

**Step 5**: Select **Install MoonLight**

   <img width="200" alt="Screenshot 2025-06-07 at 20 54 31" src="https://github.com/user-attachments/assets/a0a8d92c-dae2-4cfe-ac78-bc7cacb24724" />

**Step 6**: Optionally **erase** the ESP32-device

!!! tip "Erase"

    Erase if you have a new ESP32-device or want to start fresh. Do not erase if you want to update an existing MoonLight installation. Erase deletes all settings and requires to re-[connect to MoonLight](/#connect-moonlight).

   <img width="250" src="https://github.com/user-attachments/assets/35045317-520d-427c-894e-418693877831" />

**Step 7**: Confirm and **install**
  
   <img width="200" src="https://github.com/user-attachments/assets/25c05a6c-d2ef-41cb-b83c-fc40a60c6ccc" />
   <img width="200" src="https://github.com/user-attachments/assets/b2391752-51c4-400e-b95c-4fa865e93595" />
   <img width="200" src="https://github.com/user-attachments/assets/5e2ceefd-4c31-4b72-a228-f29373b677ac" />

**Step 8**: Optionally Press **Logs and Console**

   <img width="350" alt="Screenshot 2025-06-07 at 20 57 54" src="https://github.com/user-attachments/assets/9ac753dc-93b7-4f79-9419-0c81d1a4bc26" />

!!! tip "Logging"
    You can see the serial logging of the ESP32-device. In case you see nothing you can press reset device, now you should see the logging.

## Connect MoonLight

**Step 1**: Make a **WiFi connection** to the ESP32-device, it should present itself in the list of WiFi access points of your computer, phone or tablet.

   <img width="200" src="https://github.com/user-attachments/assets/9a146e3c-1a53-4906-ad2a-d70215efcf4b" />

**Step 2**: After connecting, MoonLight will show up in a **web browser** (all browsers supported). You will see this screen, select WiFi Station.

   <img width="400" src="https://github.com/user-attachments/assets/f480356d-bf56-4a6b-b6f2-0be49fa27db3" />

   * The device will show in 'Captive portal' mode. Alternatively you can close the captive portal and show it in a browser using [http://4.3.2.1](http://4.3.2.1)

!!! warning "UI not showing when installing new version of MoonLight"

    If you ran previous versions of MoonLight, the UI might not show up if it has been changed since then. In this case it is needed to reload the UI from the ESP32-device (use [http://4.3.2.1](http://4.3.2.1))

    * Chrome: Command Shift R (Mac) or Ctrl Shift R or Control Reload (Windows)
    * Safari: Reload from Origin : Option Shift R
    * After this not all UI elements might be immediately visible. If you see 'MoonLight loading ...' in the browser, just wait a bit (☕️)

    * Sometimes it migh be needed to clear the browser cache:
        * Chrome: Delete Browsing data / delete data (caching data)
        * Safari: Empty cache: Menu Develop (enable if not visible in the Safari Menu) / empty caches (Command Option E)

        * More info: [how to hard refresh your browser and clear cache](https://fabricdigital.co.nz/blog/how-to-hard-refresh-your-browser-and-clear-cache)

**Step 3**: Enter your local **WiFi network credentials** and hostname:

   <img width="400" src="https://github.com/user-attachments/assets/fef84240-9d9d-4a4f-acba-16e6e179305e" />

!!! tip "Hostname"

    The hostname will be used to access your device. E.g. if the hostname is ML-home you can access it using [http://ml-home.local](http://ml-home.local)

**Step 4**: Press **Add Network** and apply settings. After connecting to your normal WiFi network you will see this screen showing the new IP address of your MoonLight ESP32-device. 

   <img width="400" src="https://github.com/user-attachments/assets/e924ebe0-49a1-47f6-84f7-786cdadc4d19" />

**Step 5**: When connected to your local WiFi network, enter the IP address of the new device. Alternatively enter the hostname.

!!! info "Using hostname"
    * Restart the device to make the hostname known to your network (go to system status, scroll down and press restart)
    * Enter the hostname in your browser e.g. [http://ml-home.local](http://ml-home.local)

!!! warning "reload UI"
    See step 2 ⚠️ if UI is not showing up, use [http://ml-home.local](http://ml-home.local) (replace ml-home with the hostname) or the IP address.

## Setup MoonLight

**Step 1**: Add an effect

   <img width="350" src="https://github.com/user-attachments/assets/2c8a8b75-c429-4038-a5af-adc82b11c9de" />

**Step 2**: Add a layout and set pin(s)

   <img width="350" src="https://github.com/user-attachments/assets/8d8b9c2d-bed3-439e-a145-2bb204639c6c" />

* Add a layout Node by pressing the blue + button and select on off the 🚥 options (Panel 🚥 is a safe bet to start with). Scroll down to the Pin Control field and enter the pin number(s) you connected the strip or panel to. 

!!! info "Monitor"
    The Monitor should now show the effect

**Step 3**: Add a driver

   <img width="350" src="https://github.com/user-attachments/assets/e7fab24f-3803-4aa5-8638-459cca8a9caf" />

* Add a driver node by pressing the blue + button and select one of the ☸️ options.
    
!!! info "Strip or panel"
    The LEDs on your strip or panel should now show the effect

!!! tip "Choose driver"
    FastLED driver is best to start with if you have a normal (ws2812) LED strip or panel. Choose the Physical driver if you have more then 4 LED strips or panels or non standard LEDS (e.g. RGBW lights, curtains...). Other drivers (Virtual, Hub,) not supported yet.
    In some cases restart the device to make layout changes effective is needed.
  
**Step 4**: Press save (💾). Saves your setup to the file system, so if a device is restarted, your settings are still there.

!!! info "Next steps"
    * You can now change effect or add effects, modifiers, layouts and drivers
    * Go to [MoonLigh Overview](https://moonmodules.org/MoonLight/moonlight/overview/)
    * press the ? in MoonLight to go directly to the relevant page

## Update MoonLight

* The preferred way to update MoonLight is via [System update](https://moonmodules.org/MoonLight/system/update/)

    <img width="350" src="https://github.com/user-attachments/assets/523ea32f-88f8-4994-8d23-9541dce67ba1" />

    * Download from GitHub
    * Upload from file. From a [GitHub release](https://github.com/MoonModules/MoonLight/releases), or created by VSCode, see [Develop / Installation](https://moonmodules.org/MoonLight/develop/installation/)

* To install the latest release, you can also use the [MoonLight Web Installer](https://raw.githack.com/MoonModules/MoonLight/refs/heads/main/firmware/installer/index.html) (no need to erase the device if updating)
* Upload directly from VSCode, see [Develop / Installation](https://moonmodules.org/MoonLight/develop/installation/)
