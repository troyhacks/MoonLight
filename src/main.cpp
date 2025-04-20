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

#if FT_ENABLED(FT_MOONBASE)
    #include "MoonBase/FilesService.h"
    #include "MoonBase/ModuleInstances.h"
    #include "MoonBase/ModuleDemo.h"

    #if FT_ENABLED(FT_MOONLIGHT)
        #include "MoonLight/ModuleAnimations.h"
    #endif
#endif

#define SERIAL_BAUD_RATE 115200

#undef TAG
#define TAG "🌙"

PsychicHttpServer server;

ESP32SvelteKit esp32sveltekit(&server, 120);

LightMqttSettingsService lightMqttSettingsService = LightMqttSettingsService(&server,
                                                                             &esp32sveltekit);

LightStateService lightStateService = LightStateService(&server,
                                                        &esp32sveltekit,
                                                        &lightMqttSettingsService);

#if FT_ENABLED(FT_MOONBASE)
    FilesService filesService = FilesService(&server, &esp32sveltekit);
    ModuleInstances moduleInstances = ModuleInstances(&server, &esp32sveltekit, &filesService);
    ModuleDemo moduleDemo = ModuleDemo(&server, &esp32sveltekit, &filesService);

    #if FT_ENABLED(FT_MOONLIGHT)
        ModuleAnimations moduleAnimations = ModuleAnimations(&server, &esp32sveltekit, &filesService);
    #endif
#endif
    
void setup()
{
    // start serial and filesystem
    Serial.begin(SERIAL_BAUD_RATE);

    delay(1000); // to capture all the serial output

    // start ESP32-SvelteKit
    esp32sveltekit.begin();

    #if FT_ENABLED(FT_MOONBASE)
        filesService.begin();
        moduleInstances.begin();
        moduleDemo.begin();

        #if FT_ENABLED(FT_MOONLIGHT)
            moduleAnimations.begin();
        #endif
    #endif

    // load the initial light settings
    lightStateService.begin();
    // start the light service
    lightMqttSettingsService.begin();

    // String shortString = "Hello world";
    // String longString = "Hello world, this is a long string that is longer than 32 characters";
    // Char<32> averageChar; averageChar = "Hello world";

    // String microString = "Hi";
    // ESP_LOGD(TAG, "microString: %s", microString.c_str());
    // ESP_LOGD(TAG, "microString length: %d", microString.length());
    // ESP_LOGD(TAG, "microString size: %d", sizeof(microString));

    // ESP_LOGD(TAG, "shortString: %s", shortString.c_str());
    // ESP_LOGD(TAG, "longString: %s", longString.c_str());
    // ESP_LOGD(TAG, "averageChar: %s", averageChar.c_str());
    // ESP_LOGD(TAG, "shortString length: %d", shortString.length());
    // ESP_LOGD(TAG, "longString length: %d", longString.length());
    // ESP_LOGD(TAG, "averageChar length: %d", averageChar.length());
    // ESP_LOGD(TAG, "shortString size: %d", sizeof(shortString));
    // ESP_LOGD(TAG, "longString size: %d", sizeof(longString));
    // ESP_LOGD(TAG, "averageChar size: %d", sizeof(averageChar));
    // ESP_LOGD(TAG, "UpdatedItem size: %d = %d + %d + %d ...", sizeof(UpdatedItem), sizeof(JsonString), sizeof(JsonVariant), sizeof(String));
}

void loop()
{
    esp32sveltekit.lps++;

    #if FT_ENABLED(FT_MOONBASE)

        #if FT_ENABLED(FT_MOONLIGHT)
            moduleAnimations.loop();
        #endif

        //1s loop
        static int lastTime1s = 0;
        if (millis() - lastTime1s > 1000)
        {
            lastTime1s = millis();
            moduleInstances.loop1s();
            moduleAnimations.loop1s();
            moduleDemo.loop1s();

            //10s loop
            static int lastTime10s = 0;
            if (millis() - lastTime10s > 10000)
            {
                lastTime10s = millis();
                moduleInstances.loop10s();
            }
        }

        while (!runInLoopTask.empty()) {
            ESP_LOGD(TAG, "Running queued function");
            runInLoopTask.back()(); //calls the last function
            runInLoopTask.pop_back();
        }

    #endif
}
