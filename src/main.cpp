/**
 *   ESP32 SvelteKit
 *
 *   A simple, secure and extensible framework for IoT projects for ESP32 platforms
 *   with responsive Sveltekit front-end built with TailwindCSS and DaisyUI.
 *   https://github.com/theelims/ESP32-sveltekit
 *
 *   Copyright (C) 2018 - 2023 rjwats
 *   Copyright (C) 2023 - 2024 theelims
 *
 *   All Rights Reserved. This software may be modified and distributed under
 *   the terms of the LGPL v3 license. See the LICENSE file for details.
 **/

#include <ESP32SvelteKit.h>
#include <LightMqttSettingsService.h>
#include <LightStateService.h>
#include <PsychicHttpServer.h>

#if FT_ENABLED(FT_FILEMANAGER)
    #include "custom/FilesService.h"
#endif

#if FT_ENABLED(FT_LIVEANIMATION)
    #include "custom/ModuleAnimations.h"
    #include "custom/ModuleDemo.h"
#endif

#define SERIAL_BAUD_RATE 115200

PsychicHttpServer server;

ESP32SvelteKit esp32sveltekit(&server, 120);

LightMqttSettingsService lightMqttSettingsService = LightMqttSettingsService(&server,
                                                                             &esp32sveltekit);

LightStateService lightStateService = LightStateService(&server,
                                                        &esp32sveltekit,
                                                        &lightMqttSettingsService);

#if FT_ENABLED(FT_FILEMANAGER)
    FilesService filesService = FilesService(&server, &esp32sveltekit);
#endif
    
#if FT_ENABLED(FT_LIVEANIMATION)
    #if FT_ENABLED(FT_FILEMANAGER)
        ModuleAnimations moduleAnimations = ModuleAnimations(&server, &esp32sveltekit, &filesService);
        ModuleDemo moduleDemo = ModuleDemo(&server, &esp32sveltekit, &filesService);
    #else
        ModuleAnimations moduleAnimations = ModuleAnimations(&server, &esp32sveltekit);
        ModuleDemo moduleDemo = ModuleDemo(&server, &esp32sveltekit);
    #endif
#endif

void setup()
{
    // start serial and filesystem
    Serial.begin(SERIAL_BAUD_RATE);

    delay(1000); // to capture all the serial output

    // start ESP32-SvelteKit
    esp32sveltekit.begin();

    #if FT_ENABLED(FT_FILEMANAGER)
        filesService.begin();
    #endif

    // load the initial light settings
    lightStateService.begin();
    // start the light service
    lightMqttSettingsService.begin();

    #if FT_ENABLED(FT_LIVEANIMATION)
        moduleAnimations.begin();
        moduleDemo.begin();
    #endif
}

void loop()
{
    esp32sveltekit.lps++;
    #if FT_ENABLED(FT_LIVEANIMATION)
        moduleAnimations.loop();
    #endif

    while (!runInLoopTask.empty())
    {
        ESP_LOGD("Main", "Running queued function");
        runInLoopTask.back()(); //calls the last function
        runInLoopTask.pop_back();
    }
}
