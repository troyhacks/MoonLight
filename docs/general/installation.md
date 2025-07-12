# MoonLight Installation

Install Moonlight onto an ESP32 using the MoonLight Web installer

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

* **Step 9**: After connecting, MoonLight will show up in a **web browser** (all browsers supported). A new install will take some time to show up (up to a minute or sometimes 2). On some browsers, e.g. Safari, MoonLight pops up showing Connection to device lost repeatedly (bug). In that case, close this browser window and open MoonLight in a browser using 4.3.2.1 as URL. You will see this screen, select WiFi Station.
  
   <img width="400" alt="Screenshot 2025-06-07 at 20 59 19" src="https://github.com/user-attachments/assets/3d8bddfb-9f95-4317-891c-be8b90880541" />

* **Step 10**: Enter your local **WiFi network credentials**:

   <img width="400" alt="Screenshot 2025-06-07 at 20 59 34" src="https://github.com/user-attachments/assets/38f6cb86-e44c-478e-9910-1b0614c99292" />

* **Step 11**: Press **Add Network** and apply settings. After connecting to your local WiFi network you will see this screen showing the new IP address of your MoonLight board. If you don't know the IP address of your board anymore you can go to step 3 and 4 and step 7 and check the IP address in the Serial logging. Advanced tip: Use [ESP32Instances](https://github.com/ewowi/ESP32Instances) to discover the ESP32 nodes on your network (using nodeJS and html)

   <img width="400" alt="Screenshot 2025-06-07 at 20 59 55" src="https://github.com/user-attachments/assets/3696f3b0-70f7-4be8-a310-948b003450b7" />

* **Step 12**: Go to your WiFi settings and connect to your local WiFi network and enter the IP address of your MoonLight board in a browser and go to **MoonLight Editor**. Add a layout Node by pressing the blue + button and select Panel 🚥. Scroll down to the Pin Control field and enter the pin number you connected the strip or panel to in Step 1. Add a driver node, FastLED driver is best to start with. Finally add an effect node. The monitor and a connected strip to the configured pin(s) should now show the effect.

   <img width="400" alt="Screenshot 2025-06-07 at 21 01 02" src="https://github.com/user-attachments/assets/5d4d581f-5586-43c0-a5ea-02930effc3d8" />

* **That's it**. You can now change effect nodes, or add nodes. For audio add the AudioSync node. More how to's will be added. At the moment:
    * Check the help pages on this documentation site, especially the [editor page](https://moonmodules.org/MoonLight/moonbase/module/editor/)
    * press the ? in MoonLight to go directly to the relevant page
   More how to's will be added. At the moment:
    * Live scripts: [how to run a live script](https://github.com/MoonModules/MoonLight/blob/main/docs/moonbase/module/liveScripts.md#how-to-run-a-live-script)

* Made using [esp-web-tools](https://esphome.github.io/esp-web-tools/)
* See also [ESP-Web-Tools-Tutorial](https://github.com/witnessmenow/ESP-Web-Tools-Tutorial/blob/main/README.md)