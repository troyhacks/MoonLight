# Network - WiFi Station

<img width="320" src="https://github.com/user-attachments/assets/f9967bac-eb73-4bf1-bb34-690f4ff4b28c" />

* **WiFi TX Power** 🌙: WiFi transmit power can be set to optimize WiFi behavior. Some boards are known to work better by setting transmit power to 8.5 dBM (So called LOLIN_WIFI_FIX).
    * Default is typically 20 dBm (100mW) - the maximum allowed
    * Common recommended settings:
	    * For most indoor applications: 15-17 dBm
	    * For battery-powered devices: 10-15 dBm
	    * For close-range applications: 8-12 dBm
    * 🚨: When the board is in AP mode, it is set to 8.5dBM as most boards work okay with this. WiFi is also initially set to 8.5dBM
