/**
 *   ESP32 SvelteKit
 *
 *   A simple, secure and extensible framework for IoT projects for ESP32 platforms
 *   with responsive Sveltekit front-end built with TailwindCSS and DaisyUI.
 *   https://github.com/theelims/ESP32-sveltekit
 *
 *   Copyright (C) 2018 - 2023 rjwats
 *   Copyright (C) 2023 - 2025 theelims
 *
 *   All Rights Reserved. This software may be modified and distributed under
 *   the terms of the LGPL v3 license. See the LICENSE file for details.
 **/

#include <ESP32SvelteKit.h>
#include <PsychicHttpServer.h>

TaskHandle_t effectTaskHandle = NULL;
TaskHandle_t driverTaskHandle = NULL;

// 🌙
#if FT_ENABLED(FT_MOONBASE)
    #include "MoonBase/FileManager.h"
    #include "MoonBase/ModuleInstances.h"
    #include "MoonBase/ModuleTasks.h"
    #include "MoonBase/ModulePins.h"

    // 💫
    #if FT_ENABLED(FT_MOONLIGHT)
        #include "MoonLight/ModuleLightsControl.h"
        #include "MoonLight/ModuleVirtual.h"
        #include "MoonLight/ModulePhysical.h"
        #if FT_ENABLED(FT_LIVESCRIPT)
            #include "MoonLight/ModuleLiveScripts.h"
        #endif
        #include "MoonLight/ModuleChannelView.h"
        #include "MoonLight/ModuleMoonLightInfo.h"
    #endif
#endif

#define SERIAL_BAUD_RATE 115200

#undef TAG
#define TAG "🌙"

PsychicHttpServer server;

ESP32SvelteKit esp32sveltekit(&server, NROF_END_POINTS); //🌙 pio variable

// 🌙
#if FT_ENABLED(FT_MOONBASE)
    FileManager fileManager = FileManager(&server, &esp32sveltekit);
    ModuleInstances moduleInstances = ModuleInstances(&server, &esp32sveltekit);
    ModuleTasks moduleTasks = ModuleTasks(&server, &esp32sveltekit);
    ModulePins modulePins = ModulePins(&server, &esp32sveltekit);

    // 💫
    #if FT_ENABLED(FT_MOONLIGHT)
        ModuleLightsControl moduleLightsControl = ModuleLightsControl(&server, &esp32sveltekit, &fileManager);
        ModuleVirtual moduleVirtual = ModuleVirtual(&server, &esp32sveltekit, &fileManager);
        ModulePhysical modulePhysical = ModulePhysical(&server, &esp32sveltekit);
        #if FT_ENABLED(FT_LIVESCRIPT)
            ModuleLiveScripts moduleLiveScripts = ModuleLiveScripts(&server, &esp32sveltekit, &fileManager, &moduleVirtual);
        #endif
        ModuleChannelView moduleChannelView = ModuleChannelView(&server, &esp32sveltekit);
        ModuleMoonLightInfo moduleMoonLightInfo = ModuleMoonLightInfo(&server, &esp32sveltekit);
    #endif
#endif

volatile xSemaphoreHandle effectSemaphore = xSemaphoreCreateBinary();
volatile xSemaphoreHandle driverSemaphore = xSemaphoreCreateBinary();

void effectTask(void* pvParameters) {
    // 🌙

    for (;;) {
        esp32sveltekit.lps++; // 🌙

        #if FT_ENABLED(FT_MOONLIGHT)
            if (xSemaphoreTake(effectSemaphore, pdMS_TO_TICKS(100))==pdFALSE) {
                // ESP_LOGE(SVK_TAG, "effectSemaphore wait too long");
            }
            if (layerP.lights.header.isPositions == 0) {//otherwise lights is used for positions etc.
                layerP.loop(); //run all the effects of all virtual layers (currently only one)
            }
            xSemaphoreGive(driverSemaphore);
        #endif
            
        while (!runInTask1.empty()) {
            ESP_LOGD(TAG, "runInTask1 %d", runInTask1.size());
            runInTask1.front()();
            runInTask1.erase(runInTask1.begin());
        }

        vTaskDelay(1); // yield to other tasks, 1 tick (~1ms)
    }
}

void driverTask(void* pvParameters) {
    // 🌙

    // layerP.setup2();

    for (;;) {

        #if FT_ENABLED(FT_MOONLIGHT)
            xSemaphoreTake(driverSemaphore, pdMS_TO_TICKS(100));

            if (layerP.lights.header.isPositions == 0) {//otherwise lights is used for positions etc.
                for (Node *node: layerP.nodes) {
                    if (node->on)
                        node->loop();
                }
            }
            xSemaphoreGive(effectSemaphore);
        #endif

        while (!runInTask2.empty()) {
            ESP_LOGD(TAG, "runInTask2 %d", runInTask2.size());
            runInTask2.front()();
            runInTask2.erase(runInTask2.begin());
        }

        vTaskDelay(1); // yield to other tasks, 1 tick (~1ms)
    }
}

void setup()
{
    // start serial and filesystem
    Serial.begin(SERIAL_BAUD_RATE);

    // delay(5000); // 🌙 to capture all the serial output

    // 🌙 safeMode
    if (esp_reset_reason() != ESP_RST_UNKNOWN && esp_reset_reason() != ESP_RST_POWERON && esp_reset_reason() != ESP_RST_SW && esp_reset_reason() != ESP_RST_USB) { //see verbosePrintResetReason
        //ESP_RST_USB is after usb flashing! since esp-idf5
        safeModeMB = true;
    }

    // start ESP32-SvelteKit
    esp32sveltekit.begin();

    // MoonBase
    #if FT_ENABLED(FT_MOONBASE)
        fileManager.begin();
        moduleInstances.begin();
        moduleTasks.begin();
        modulePins.begin();
    #endif
    
    // MoonLight
    #if FT_ENABLED(FT_MOONLIGHT)
        moduleVirtual.begin();
        modulePhysical.begin();
        moduleLightsControl.begin();
        moduleChannelView.begin();
        moduleMoonLightInfo.begin();
        #if FT_ENABLED(FT_LIVESCRIPT)
            moduleLiveScripts.begin();
        #endif
    #endif

    esp32sveltekit.addLoopFunction([]() { 

        moduleVirtual.loop();
        modulePhysical.loop();

        EVERY_N_SECONDS(1) {
            //set shared data (eg used in scrolling text effect)
            sharedData.fps = esp32sveltekit.getAnalyticsService()->lps;
            sharedData.connectionStatus = (uint8_t) esp32sveltekit.getConnectionStatus();
            sharedData.clientListSize = esp32sveltekit.getServer()->getClientList().size();
            sharedData.connectedClients = esp32sveltekit.getSocket()->getConnectedClients(); 

            moduleInstances.loop1s();

            #if FT_ENABLED(FT_MOONLIGHT)
                #if FT_ENABLED(FT_LIVESCRIPT)
                    moduleLiveScripts.loop1s();
                #endif
            #endif

            moduleTasks.loop1s(); 
            modulePins.loop1s(); 
        }

        EVERY_N_SECONDS(10) {
            moduleInstances.loop10s();
        }

        #if FT_ENABLED(FT_MOONLIGHT) && FT_ENABLED(FT_MONITOR)

            EVERY_N_MILLIS(layerP.lights.header.nrOfLights / 12) {
                EventSocket *_socket = esp32sveltekit.getSocket();

                moduleLightsControl.read([&](ModuleState& _state) {
                    if (layerP.lights.header.isPositions == 2) { //send to UI
                        if (_socket->getConnectedClients() && _state.data["monitorOn"])
                            _socket->emitEvent("monitor", (char *)&layerP.lights, sizeof(LightsHeader) + MIN(layerP.lights.header.nrOfLights * 3, MAX_CHANNELS));
                        layerP.lights.header.isPositions = 0;
                    } else if (layerP.lights.header.isPositions == 0) {//send to UI
                        if (_socket->getConnectedClients() && _state.data["monitorOn"])
                            _socket->emitEvent("monitor", (char *)&layerP.lights, sizeof(LightsHeader) + MIN(layerP.lights.header.nrOfLights * layerP.lights.header.channelsPerLight, MAX_CHANNELS));
                    }
                });
            }

        #endif
    });

    xSemaphoreGive(effectSemaphore);  //Allow effectTask to run first

    // 🌙
    xTaskCreateUniversal(
        effectTask,              // task function
        "AppEffectTask",            // name
        8 * 1024,             // stack size in words (without livescripts we can do with 12...)
        NULL,                  // parameter
        8,                     // priority (between 5 and 10: ASYNC_WORKER_TASK_PRIORITY and Restart/Sleep), don't set it higher then 10...
        &effectTaskHandle,       // task handle
        1                      // core (0 or 1)
    );

    xTaskCreateUniversal(
        driverTask,              // task function
        "AppDriverTask",            // name
        8 * 1024,             // stack size in words (without livescripts we can do with 12...)
        NULL,                  // parameter
        8,                     // priority (between 5 and 10: ASYNC_WORKER_TASK_PRIORITY and Restart/Sleep), don't set it higher then 10...
        &driverTaskHandle,       // task handle
        1                      // core (0 or 1)
    );

    // #if CONFIG_IDF_TARGET_ESP32
    //     pinMode(19, OUTPUT); digitalWrite(19, HIGH); // for serg shield boards: to be done: move to new pin manager module, switch off for S3!!!! tbd: add pin manager
    // #endif
}

void loop()
{
    // Delete Arduino loop task, as it is not needed in this example
    vTaskDelete(NULL);
}
