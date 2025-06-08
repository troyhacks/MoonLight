# Pull-Requests

<img width="300" src="https://github.com/user-attachments/assets/8750fc86-cbde-46ef-9392-9d6810340b52" />


MoonLight has made a number of commits which are general purpose and could be added to upstream [ESP32 SvelteKit](https://github.com/theelims/ESP32-sveltekit). Pending changes are ready to add in pull-requests as they are branched from the upstream repo.
Below lists are ordered in terms of likelyhood to be accepted:

## Pending - Ready to PR

* 🐛 [bug-metadata-parent-wifi](https://github.com/theelims/ESP32-sveltekit/commit/8573d90784f4fbf28de1d30be24b7421e965d0b4): avoid console.log errors
* ⚠️ [components-small-fixes](https://github.com/theelims/ESP32-sveltekit/commit/da2c641eea81799f683bbc452d708d4d159e357e): avoid console.log errors
* ⚠️ [compile-warnings](https://github.com/theelims/ESP32-sveltekit/commit/8a4dc4171c37d04754744f76513c2d2a08b0662d): role and tab index
* 💡 [service-name-string](https://github.com/theelims/ESP32-sveltekit/commit/05a7bd9a12999e087d0b05a62859d263679cc76a): Allow service names to be non string literals (e.g. done in MoonBase-Modules)
    * ⚠️ [EventSocket emitEvent: use String type](https://github.com/theelims/ESP32-sveltekit/commit/54c4a44eb95be2fe344bb78f022c8afcbbd8c731)
* ⚠️ [no-emit-no-clients](https://github.com/MoonModules/MoonLight/commit/c024c2ff656511c67625b3dce3642d6560724482)
* 💡 [system-status-metrics](https://github.com/theelims/ESP32-sveltekit/commit/352cfe3e376b25f7470ad4f764cdf54f7069c645): use max instead of first
* 💡 [help-to-docs](https://github.com/theelims/ESP32-sveltekit/commit/2c2d2fae5c37b220bc61dfb1ba6655485de6547f): Help link to github.io docs e.g. [animations](https://moonmodules.org/MoonLight/moonbase/module/animations)
* 💡 [System metrics](https://moonmodules.org/MoonLight/system/metrics/)
    * Loops per second (performance)
* 💡 [System status](https://moonmodules.org/MoonLight/system/status/)
    * Loops per second (performance)
* 💡 [menu-href-unique](https://github.com/theelims/ESP32-sveltekit/commit/92acbd046e478bccf7eec469e0dab5dcda53c0ae): Allow multiple modules using same Module.svelte
* 💡 [measure-battery-pin](https://github.com/theelims/ESP32-sveltekit/commit/bcd3abd1f981d48d7a76c5ee71fc99f4fe54eb08)

## Pending - WIP

* [File Manager](https://moonmodules.org/MoonLight/moonbase/files/)
* [Instances](https://moonmodules.org/MoonLight/moonbase/module/instances/)
* [MoonBase-Modules](https://moonmodules.org/MoonLight/moonbase/modules/)

## Submitted

* None ATM

## Merged into ESP32-Sveltekit

* [Expands menu on selected subitem](https://github.com/theelims/ESP32-sveltekit/pull/77)
* [Add file.close in fileHandler handleRequest](https://github.com/theelims/ESP32-sveltekit/pull/73)
* [Refactor System Status and Metrics](https://github.com/theelims/ESP32-sveltekit/pull/78)
    * Add free_psram, used_psram and psram_size in Analytics (models.ts) and analytics_data (analytics.ts) and show in UI (SystemMetrics.svelte)
    * Send psram data only if psramFound (SystemStatus.svelte, AnalyticsService.h, SystemStatus.cpp)
* [Wifi: Multiple edits bug resolved](https://github.com/theelims/ESP32-sveltekit/pull/81)
* [ESPD_LOGx: replace first argument with TAG and define TAG as 🐼](https://github.com/theelims/ESP32-sveltekit/pull/85)

### Other improvements

* Add esp32-s3-devkitc-1-n16r8v and LOLIN_WIFI_FIX in pio.ini (including boards folder)
* Add Monitor
    * socket.ts: add else listeners.get("monitor")?.forEach((listener) => listener(new Uint8Array(message.data)));
    * EventSocket.cpp: add void EventSocket::emitEvent with char * argument
* Add MoonBase / MoonLight specific functionality
* ESP32SvelteKit.cpp: 
    * CPU load (and main.cpp)
    * comment response.addHeader("Cache-Control", "public, immutable, max-age=31536000");
* interface/source/routes/+layout.svelte
    * Don't show if captive portal: {#if (!window.location.href.includes("192.168.4.1") && $page.data.features.monitor)}
* main.cpp: esp_log_set_vprintf(my_vprintf); WIP
* ci pio
* run in loopTask to avoid stack size crashes in httpd
* updatedItems (to see what specifically has been updated)