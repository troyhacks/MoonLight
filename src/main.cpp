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

 #include <Arduino.h>

// Threshold (bytes) above which allocations go into PSRAM
constexpr size_t PSRAM_THRESHOLD = 0;  //87K free, works fine until now
// constexpr size_t PSRAM_THRESHOLD = 512;  //recommended ... ? 32K free, (Small stuff (pointers, FreeRTOS objects, WiFi stack internals) → must stay in internal RAM....)?
// constexpr size_t PSRAM_THRESHOLD = 64;  //65K free, fallback if 0 gives problems?

// Override global new/delete
void* operator new(size_t size) {
    void *ptr = nullptr;
    if (size > PSRAM_THRESHOLD) {
        // Serial.printf("new %d\n", size);
        // Try PSRAM first
        ptr = heap_caps_malloc_prefer(size, 2, MALLOC_CAP_SPIRAM, MALLOC_CAP_DEFAULT);
        if (ptr) return ptr;  // success
    }
    // 
    Serial.printf("new Fallback: internal RAM %d\n", size);
    ptr = heap_caps_malloc(size, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    return ptr;
}

void operator delete(void* ptr) noexcept {
    // Serial.printf("delete\n");
    heap_caps_free(ptr);
}

#include <ESP32SvelteKit.h>
#include <PsychicHttpServer.h>
#include "MoonBase/Utilities.h" // for runInTask1/2

#define SERIAL_BAUD_RATE 115200

PsychicHttpServer server;

ESP32SvelteKit esp32sveltekit(&server, NROF_END_POINTS); //🌙 pio variable

// 🌙
#if FT_ENABLED(FT_MOONBASE)
    #include "MoonBase/FileManager.h"
    #include "MoonBase/ModuleDevices.h"
    #include "MoonBase/ModuleTasks.h"
    #include "MoonBase/ModulePins.h"

    FileManager fileManager = FileManager(&server, &esp32sveltekit);
    ModuleDevices moduleDevices = ModuleDevices(&server, &esp32sveltekit);
    ModuleTasks moduleTasks = ModuleTasks(&server, &esp32sveltekit);
    ModulePins modulePins = ModulePins(&server, &esp32sveltekit);

    // 💫
    #if FT_ENABLED(FT_MOONLIGHT)
        #include "MoonLight/ModuleLightsControl.h"
        #include "MoonLight/ModuleEffects.h"
        #include "MoonLight/ModuleDrivers.h"
        #include "MoonLight/ModuleChannels.h"
        #include "MoonLight/ModuleMoonLightInfo.h"
        ModuleLightsControl moduleLightsControl = ModuleLightsControl(&server, &esp32sveltekit, &fileManager);
        ModuleEffects moduleEffects = ModuleEffects(&server, &esp32sveltekit, &fileManager);
        ModuleDrivers moduleDrivers = ModuleDrivers(&server, &esp32sveltekit);
        #if FT_ENABLED(FT_LIVESCRIPT)
            #include "MoonLight/ModuleLiveScripts.h"
            ModuleLiveScripts moduleLiveScripts = ModuleLiveScripts(&server, &esp32sveltekit, &fileManager, &moduleEffects);
        #endif
        ModuleChannels moduleChannels = ModuleChannels(&server, &esp32sveltekit);
        ModuleMoonLightInfo moduleMoonLightInfo = ModuleMoonLightInfo(&server, &esp32sveltekit);

        volatile xSemaphoreHandle effectSemaphore = xSemaphoreCreateBinary();
        volatile xSemaphoreHandle driverSemaphore = xSemaphoreCreateBinary();

        TaskHandle_t effectTaskHandle = NULL;
        TaskHandle_t driverTaskHandle = NULL;

        void effectTask(void* pvParameters) {
            // 🌙

            for (;;) {
                esp32sveltekit.lps++; // 🌙 todo: not moonlight specific?

                if (xSemaphoreTake(effectSemaphore, pdMS_TO_TICKS(100))==pdFALSE) {
                    // MB_LOGE(ML_TAG, "effectSemaphore wait too long"); //happens if no driver!, but let effects continue (for monitor) at 10 fps
                }

                layerP.loop(); //run all the effects of all virtual layers (currently only one)

                xSemaphoreGive(driverSemaphore);
                    
                while (!runInTask1.empty()) {
                    // MB_LOGD(ML_TAG, "runInTask1 %d", runInTask1.size());
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

                xSemaphoreTake(driverSemaphore, pdMS_TO_TICKS(100));

                layerP.loopDrivers();

                xSemaphoreGive(effectSemaphore);

                while (!runInTask2.empty()) {
                    MB_LOGV(MB_TAG, "runInTask2 %d", runInTask2.size());
                    runInTask2.front()();
                    runInTask2.erase(runInTask2.begin());
                }

                vTaskDelay(1); // yield to other tasks, 1 tick (~1ms)
            }
        }
    #endif
#endif

// 🌙 Custom log output function - 🚧
static int custom_vprintf(const char* fmt, va_list args)
{
    // Example 1: Write to a custom UART or buffer
    char buffer[256];
    int len = vsnprintf(buffer, sizeof(buffer), fmt, args);

    Serial.printf("🌙"); //to test it works - 🚧 to send logging to UI
    
    // Send to custom output (e.g., external UART, network, file, etc.)
    // custom_uart_write(buffer, len);
    
    // For this example, we'll also write to stdout
    return vprintf(fmt, args);
}

#if USE_M5UNIFIED
    #include <M5Unified.h>
    #include "moonlight_logo.h"
#endif

void setup()
{
    //🌙 use ESP_IDF_LOG
    esp_log_set_vprintf(custom_vprintf);
    esp_log_level_set("*", LOG_LOCAL_LEVEL); //use the platformio setting here

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
        moduleDevices.begin();
        moduleTasks.begin();
        modulePins.begin();

        // MoonLight
        #if FT_ENABLED(FT_MOONLIGHT)
            moduleEffects.begin();
            moduleDrivers.begin();
            moduleLightsControl.begin();
            moduleChannels.begin();
            moduleMoonLightInfo.begin();
            #if FT_ENABLED(FT_LIVESCRIPT)
                moduleLiveScripts.begin();
            #endif

            xSemaphoreGive(effectSemaphore);  //Allow effectTask to run first

            // 🌙
            xTaskCreateUniversal(
                effectTask,              // task function
                "AppEffectTask",            // name
                8 * 1024,             // stack size in words (without livescripts we can do with 12...)
                NULL,                  // parameter
                2,                     // priority (between 5 and 10: ASYNC_WORKER_TASK_PRIORITY and Restart/Sleep), don't set it higher then 10...
                &effectTaskHandle,       // task handle
                1                      // core (0 or 1)
            );

            xTaskCreateUniversal(
                driverTask,              // task function
                "AppDriverTask",            // name
                4 * 1024,             // stack size in words (without livescripts we can do with 12...)
                NULL,                  // parameter
                2,                     // priority (between 5 and 10: ASYNC_WORKER_TASK_PRIORITY and Restart/Sleep), don't set it higher then 10...
                &driverTaskHandle,       // task handle
                1                      // core (0 or 1)
            );
        #endif

        //run UI stuff in the sveltekit task
        esp32sveltekit.addLoopFunction([]() { 

            #if FT_ENABLED(FT_MOONLIGHT)
                moduleEffects.loop(); //requestUIUpdate
                moduleDrivers.loop(); //requestUIUpdate
                moduleLightsControl.loop(); //monitor
            #endif

            //every second
            static unsigned long lastSecond = 0;
            if (millis() - lastSecond >= 1000) {
                lastSecond = millis();

                moduleDevices.loop1s();
                moduleTasks.loop1s(); 
                modulePins.loop1s(); 

                #if FT_ENABLED(FT_MOONLIGHT)
                    //set shared data (eg used in scrolling text effect)
                    sharedData.fps = esp32sveltekit.getAnalyticsService()->lps;
                    sharedData.connectionStatus = (uint8_t) esp32sveltekit.getConnectionStatus();
                    sharedData.clientListSize = esp32sveltekit.getServer()->getClientList().size();
                    sharedData.connectedClients = esp32sveltekit.getSocket()->getConnectedClients(); 

                    #if FT_ENABLED(FT_LIVESCRIPT)
                        moduleLiveScripts.loop1s();
                    #endif
                #endif
            }

            //every 10 seconds
            static unsigned long last10Second = 0;
            if (millis() - last10Second >= 10000) {
                last10Second = millis();
                
                moduleDevices.loop10s();
            }

        });

    #endif //FT_MOONBASE

    // #if CONFIG_IDF_TARGET_ESP32
    //     pinMode(19, OUTPUT); digitalWrite(19, HIGH); // for serg shield boards: to be done: move to new pin manager module, switch off for S3!!!! tbd: add pin manager
    // #endif

    #if USE_M5UNIFIED
        auto cfg = M5.config();
        M5.begin(cfg);
        // M5.Display.fillScreen(BLACK);

        M5.Display.drawPng(moonlight_logo_png, sizeof(moonlight_logo_png),0, 0, 0, 0, 0, 0, 100/320, 100/320);

        // M5.Display.fillRect(50, 50, 100, 100, RED);
        // M5.Display.setTextColor(WHITE);
        // M5.Display.setTextSize(2);
        // M5.Display.drawString("MoonLight", 0, 0);
    #endif
}

void loop()
{
    #if USE_M5UNIFIED
        M5.update();
        delay(100);
    #else
        // Delete Arduino loop task, as it is not needed in this example
        // vTaskDelete(NULL);
    #endif
}