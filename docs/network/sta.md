# WiFi Station

<img width="320" src="https://github.com/user-attachments/assets/f9967bac-eb73-4bf1-bb34-690f4ff4b28c" />

* **WiFi TX Power** 🌙: WiFi transmit power can be set to optimize WiFi behavior. Some boards are known to work better by setting transmit power to 8.5 dBM (So called LOLIN_WIFI_FIX).
    * Default is typically 20 dBm (100mW) - the maximum allowed
    * Common recommended settings:
	    * For most indoor applications: 15-17 dBm
	    * For battery-powered devices: 10-15 dBm
	    * For close-range applications: 8-12 dBm
    * 🚨: When the board is in AP mode, it is set to 8.5dBM as most boards work okay with this. WiFi is also initially set to 8.5dBM

## MoonLight Analytics

MoonLight sends **anonymous** usage data to Google Analytics when it restarts.

The data collected consists of:

* Country – e.g. France, determined via ip-api.com; only the country field is extracted.
* Firmware – e.g. esp32-s3-devkitc-1-n16r8v
* Board model – e.g. QuinLED DigQuad, based on board presets (not yet implemented)
* MoonLight version – e.g. 0.6.0

No other information is collected or stored.

We use this anonymous data to understand how MoonLight is being used, identify which hardware and versions are most common, and prioritize future development and improvements accordingly.

The data is sent only to Google Analytics, which processes it to provide aggregated usage insights.

You can **opt out** at any time by disabling Track analytics in the settings.