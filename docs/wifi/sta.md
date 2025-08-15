# WiFi - Station

<img width="350" src="https://github.com/user-attachments/assets/0be091ab-1bd6-46c9-a766-e8b6b4cbb616" />

* **WiFi TX Power** 🌙: WiFi transmit power can be set to optimize WiFi behavior. Some boards are known to work better by setting transmit power to 8.5 dBM (So called LOLIN_WIFI_FIX).
    * Default is typically 20 dBm (100mW) - the maximum allowed
    * Common recommended settings:
	    * For most indoor applications: 15-17 dBm
	    * For battery-powered devices: 10-15 dBm
	    * For close-range applications: 8-12 dBm
    * Note: When the board is in AP mode, it is set to 8.5dBM as most boards work okay with this. WiFi is also initially set to 8.5dBM