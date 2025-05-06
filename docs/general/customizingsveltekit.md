# Customizing sveltekit

### Customize UI and app specific

This is a checklist, More info on most of the items can be found in the ESP32-Sveltekit specific documentation [ESP32 SvelteKit](https://moonmodules.org/MoonLight/esp32sveltekit/), [Build Tools](https://moonmodules.org/MoonLight/gettingstarted/), [Front end](https://moonmodules.org/MoonLight/sveltekit/) and [Back End](https://moonmodules.org/MoonLight/statefulservice/)

* {custom} = MoonLight or name of a forked repo

* docs/media/
    * add {custom}-logo.png (used in mkdocs.yml)
    * replace favicon.png
* factory_settings.ini
    * FACTORY_AP_SSID=\"{custom}-#{unique_id}\"
    * FACTORY_AP_PASSWORD=\"\" (recommendation)
    * FACTORY_NTP_TIME_ZONE_LABEL=\"Europe/Amsterdam\"
* package.json
    * name = "{custom}"
    * version: "0.5.5",
* intrerface/source/lib/assets/logo.png
    * replace logo
* interface/source/routes/+layout.ts
    * title: '{custom}'
    * github:
    * copyright
    * appname: '{custom}'
* interface/source/routes/+page.svelte
    * Welcome to {custom}
    * Intro message
    * href="/" 
    * Start {custom}
* interface/source/routes/menu.svelte
    * const discord = { href: 'https://discord.gg/TC8NSUSCdV', active: true };
* interface/static/manifest.json
    * name: "{custom}"
* interface/static/favicon.png
    * replace favicon.png
* lib/framework/APSettingsService.h
    * FACTORY_AP_SSID "{custom}-#{unique_id}"
    * FACTORY_AP_PASSWORD ""
* mkdocs.yml
    * site_name: {custom}
    * nav: {custom}
    * repo_name and repo_url
    * theme logo: media/{custom}-logo.png
    * analytics: provider: google property: G-R6QYDG0126
    * Copyright
* platformio.ini
    * description = {custom}
    * add [custom] build_flags and lib_deps
    * APP_NAME=\"{custom}\" ;
    * APP_VERSION=\"0.x.0\"
    * CORE_DEBUG_LEVEL=4
* README.md
    *  Custom intro
* vite.config.ts
    * Set target: 'http://192.168.1.xxx'
* setup custom code
    * src/custom
    * interface/src/routes/moonbase
    * interface/src/lib/components/moonbase
    * interface/src/lib/types/moonbase_model.ts
    * interface/src/lib/stores/moonbase_utilities.ts
* Github repo
    * change license
    * change description
    * change webhook
