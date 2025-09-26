
# Develop Overview

* [Start developing for MoonLight](https://moonmodules.org/MoonLight/develop/overview/)
    * [ESP32 Sveltekit](https://moonmodules.org/MoonLight/esp32sveltekit/)

* [Install Development environment](https://moonmodules.org/MoonLight/develop/installation/)

## Where you can help

* Improve the web installer
* Improve the UI (Svelte 5, DaisyUI 5, TailWind 4)
  * Mutli row layout
  * Set Dark / light theme or auto
* Tune FastLED, add FastLED 2D effects
* CI automation (Generate daily builds in Github and use by installer)

Contact us in the MoonLight channels on [Discord MoonModules](https://discord.gg/4rSTthvKHe)

## Developing

* Read the [ESP32 Sveltekit docs](https://moonmodules.org/MoonLight/esp32sveltekit/)
* Read [Customizing Sveltekit](https://moonmodules.org/MoonLight/develop/customizingsveltekit/)
* Instead of cloning the repo directly from MoonLight as described above, create a fork first so you have your own environment to make changes to. See also Pull Requests below.
* UI dev: configure vite.config.ts, go to interface folder, npm install, npm run dev. A local webserver starts on localhost. UI changes will directly be shown via this webserver
* Changes made to the UI are not always visible in the browser, issue with caching / max-age (🚧), clear the browser cache to see latest UI.
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
    *   ~~-D FT_SLEEP=1~~ enabled!
    *   ~~-D FT_BATTERY=1~~ enabled!

