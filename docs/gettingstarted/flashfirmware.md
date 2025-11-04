# Flash Firmware

Flashing the firmware is easy and doesn't require you to install any development tools. You can do this through a compatible browser which supports webserial like Chrome. To flash the firmware you need physical access to both push buttons to make the board enter its bootloader mode. It is best done before assembling the board in a case and can be done without a battery connected.

1. Plug in your Noisy Cricket to an USB port and wait for the USB port to enumerate.
2. Place the board in bootloader mode by press and holding the `Boot` button, press the `Reset` button and then release the `Boot` button.
3. Hit "Install" and select the correct COM port (USB JTAG/serial debug unit).
4. After the installation has completed press the `Reset` button once again.

<p class="button-row">
<esp-web-install-button manifest='../firmware/installer/manifest_REV03.json'>
  <button slot="activate" class="md-button md-button--primary">INSTALL</button>
  <span slot="unsupported">Use Chrome Desktop</span>
  <span slot="not-allowed">Not allowed to use this on HTTP!</span>
</esp-web-install-button>
</p>

Powered by [ESP Web Tools](https://esphome.github.io/esp-web-tools/)

<script type="module" src="https://unpkg.com/esp-web-tools@10/dist/web/install-button.js?module"></script>
