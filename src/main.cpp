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

#ifdef BOARD_HAS_PSRAM

// #include <cstddef> // suggested by copilot to surpress operator warning : first parameter of allocation function must be of type 'size_t' - but made no difference

// Threshold (bytes) above which allocations go into PSRAM
constexpr size_t PSRAM_THRESHOLD = 0;  // 87K free, works fine until now
// constexpr size_t PSRAM_THRESHOLD = 512;  //recommended ... ? 32K free, (Small stuff (pointers, FreeRTOS objects, WiFi stack internals) → must stay in internal RAM....)?
// constexpr size_t PSRAM_THRESHOLD = 64;  //65K free, fallback if 0 gives problems?

// Override global new/delete
void* operator new(size_t size) {
  void* ptr = nullptr;
  if (size > PSRAM_THRESHOLD) {
    // Serial.printf("new %d\n", size);
    // Try PSRAM first
    ptr = heap_caps_malloc_prefer(size, 2, MALLOC_CAP_SPIRAM, MALLOC_CAP_INTERNAL);
    if (ptr) return ptr;  // success
  }
  //
  Serial.printf("'new' Fallback to internal RAM %d\n", size);
  ptr = heap_caps_malloc(size, MALLOC_CAP_INTERNAL);

  if (!ptr) {
    Serial.printf("new: CRITICAL - failed to allocate %d bytes\n", size);
    // throw std::bad_alloc(); // Standard requires throwing std::bad_alloc on failure
  }

  return ptr;
}

void operator delete(void* ptr) noexcept {
  // Serial.printf("'delete' using allocMB\n");
  heap_caps_free(ptr);
}

// sized delete overloads (C++14/17 compatibility)
void operator delete(void* ptr, size_t size) noexcept {
  // Serial.printf("'delete' sized %d using allocMB\n", size);
  heap_caps_free(ptr);
}

void* operator new[](size_t size) {
  // Serial.printf("'new[]' using allocMB %d\n", size);
  return operator new(size);  // Reuse logic
}

void operator delete[](void* ptr) noexcept {
  // Serial.printf("'delete[]' using allocMB\n");
  operator delete(ptr);  // Reuse logic
}

// sized delete overloads (C++14/17 compatibility)
void operator delete[](void* ptr, size_t size) noexcept {
  // Serial.printf("'delete[]' sized %d using allocMB\n", size);
  operator delete(ptr);
}
#endif

#include <ESP32SvelteKit.h>
#include <PsychicHttpServer.h>

#include "MoonBase/Utilities.h"  // for runInAppTask

#define SERIAL_BAUD_RATE 115200

PsychicHttpServer server;

ESP32SvelteKit esp32sveltekit(&server, NROF_END_POINTS);  // 🌙 pio variable

// 🌙
#if FT_ENABLED(FT_MOONBASE)
  #include "MoonBase/Modules/FileManager.h"
  #include "MoonBase/Modules/ModuleDevices.h"
  #include "MoonBase/Modules/ModuleIO.h"
  #include "MoonBase/Modules/ModuleTasks.h"

FileManager fileManager = FileManager(&server, &esp32sveltekit);
ModuleDevices moduleDevices = ModuleDevices(&server, &esp32sveltekit);
ModuleTasks moduleTasks = ModuleTasks(&server, &esp32sveltekit);
ModuleIO moduleIO = ModuleIO(&server, &esp32sveltekit);

  // 💫
  #if FT_ENABLED(FT_MOONLIGHT)
    #include "MoonLight/Modules/ModuleChannels.h"
    #include "MoonLight/Modules/ModuleDrivers.h"
    #include "MoonLight/Modules/ModuleEffects.h"
    #include "MoonLight/Modules/ModuleLightsControl.h"
    #include "MoonLight/Modules/ModuleMoonLightInfo.h"
ModuleLightsControl moduleLightsControl = ModuleLightsControl(&server, &esp32sveltekit, &fileManager);
ModuleEffects moduleEffects = ModuleEffects(&server, &esp32sveltekit, &fileManager);                        // fileManager for Live Scripts
ModuleDrivers moduleDrivers = ModuleDrivers(&server, &esp32sveltekit, &fileManager, &moduleLightsControl);  // fileManager for Live Scripts, Lights control for drivers
    #if FT_ENABLED(FT_LIVESCRIPT)
      #include "MoonLight/Modules/ModuleLiveScripts.h"
ModuleLiveScripts moduleLiveScripts = ModuleLiveScripts(&server, &esp32sveltekit, &fileManager, &moduleEffects, &moduleDrivers);
    #endif
ModuleChannels moduleChannels = ModuleChannels(&server, &esp32sveltekit);
ModuleMoonLightInfo moduleMoonLightInfo = ModuleMoonLightInfo(&server, &esp32sveltekit);

volatile xSemaphoreHandle effectSemaphore = xSemaphoreCreateBinary();
volatile xSemaphoreHandle driverSemaphore = xSemaphoreCreateBinary();

TaskHandle_t effectTaskHandle = NULL;
TaskHandle_t driverTaskHandle = NULL;

void effectTask(void* pvParameters) {
  // 🌙

  layerP.setup();  // setup virtual layers (no node setup here as done in addNode)

  for (;;) {
    esp32sveltekit.lps++;  // 🌙 todo: not moonlight specific?

    if (xSemaphoreTake(effectSemaphore, pdMS_TO_TICKS(100)) == pdFALSE) {
      // EXT_LOGW(ML_TAG, "effectSemaphore wait too long"); //happens if no driver!, but let effects continue (for monitor) at 10 fps
    }

    layerP.loop();  // run all the effects of all virtual layers (currently only one layer)

    xSemaphoreGive(driverSemaphore);

    std::vector<std::function<void()>> tasks;
    {
      std::lock_guard<std::mutex> lock(runInAppTask_mutex);
      tasks.swap(runInAppTask);  // move all into local vector
    }
    for (auto& task : tasks) task(); //run the tasks

    vTaskDelay(1);  // yield to other tasks, 1 tick (~1ms)
  }
}

void driverTask(void* pvParameters) {
  // 🌙

  // layerP.setup() done in effectTask

  for (;;) {
    xSemaphoreTake(driverSemaphore, pdMS_TO_TICKS(100));

    layerP.loopDrivers();

    xSemaphoreGive(effectSemaphore);

    vTaskDelay(1);  // yield to other tasks, 1 tick (~1ms)
  }
}
  #endif
#endif

// 🌙 Custom log output function - 🚧
#ifdef USE_ESP_IDF_LOG
static int custom_vprintf(const char* fmt, va_list args) {
  // Example 1: Write to a custom UART or buffer
  char buffer[256];
  int len = vsnprintf(buffer, sizeof(buffer), fmt, args);

  // Serial.printf("🌙"); //to test it works - 🚧 to send logging to UI

  // Send to custom output (e.g., external UART, network, file, etc.)
  // custom_uart_write(buffer, len);

  // For this example, we'll also write to stdout
  return vprintf(fmt, args);
}
#endif

#if USE_M5UNIFIED
  #include <M5Unified.h>
#endif

void setup() {
#ifdef USE_ESP_IDF_LOG  // 🌙
  esp_log_set_vprintf(custom_vprintf);
  esp_log_level_set("*", LOG_LOCAL_LEVEL);  // use the platformio setting here
#endif

  // start serial and filesystem
  Serial.begin(SERIAL_BAUD_RATE);

  // delay(5000);  // 🌙 to capture all the serial output

  // 🌙 safeMode
  if (esp_reset_reason() != ESP_RST_UNKNOWN && esp_reset_reason() != ESP_RST_POWERON && esp_reset_reason() != ESP_RST_SW && esp_reset_reason() != ESP_RST_USB) {  // see verbosePrintResetReason
    // ESP_RST_USB is after usb flashing! since esp-idf5
    safeModeMB = true;
  }

  // check sizes ...
  //   sizeof(esp32sveltekit);                // 4152
  //   sizeof(WiFiSettingsService);           // 456
  //   sizeof(SystemStatus);                  // 16
  //   sizeof(UploadFirmwareService);         // 32
  //   sizeof(HttpEndpoint<ModuleState>);     // 152
  //   sizeof(EventEndpoint<ModuleState>);    // 112
  //   sizeof(WebSocketServer<ModuleState>);  // 488
  //   sizeof(FSPersistence<ModuleState>);    // 128
  //   sizeof(PsychicHttpServer*);            // 8
  //   sizeof(HttpEndpoint<APSettings>);      // 152
  //   sizeof(FSPersistence<APSettings>);     // 128
  //   sizeof(APSettingsService);             // 600;
  //   sizeof(PsychicWebSocketHandler);       // 336
  //   sizeof(fileManager);                   // 864
  //   sizeof(Module);                        // 1144
  //   sizeof(moduleDevices);                 // 1320
  //   sizeof(moduleIO);                      // 1144
  // #if FT_ENABLED(FT_MOONLIGHT)
  //   sizeof(moduleEffects);        // 1208
  //   sizeof(moduleDrivers);        // 1216
  //   sizeof(moduleLightsControl);  // 1176
  //   #if FT_ENABLED(FT_LIVESCRIPT)
  //   sizeof(moduleLiveScripts);  // 1176
  //   #endif
  //   sizeof(moduleChannels);        // 1144
  //   sizeof(moduleMoonLightInfo);   // 1144
  //   sizeof(layerP.lights);         // 56
  //   sizeof(layerP.lights.header);  // 40
  // #endif

  // start ESP32-SvelteKit
  esp32sveltekit.begin();

// MoonBase
#if FT_ENABLED(FT_MOONBASE)
  fileManager.begin();
  moduleDevices.begin();
  moduleTasks.begin();
  moduleIO.begin();

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

  xSemaphoreGive(effectSemaphore);  // Allow effectTask to run first

  // 🌙
  xTaskCreateUniversal(effectTask,                     // task function
                       "AppEffectTask",                // name
                       (psramFound() ? 6 : 4) * 1024,  // d0-tuning... stack size (without livescripts we can do with 12...). updated from 4 to 6 to support preset loop
                       NULL,                           // parameter
                       3,                              // priority (between 5 and 10: ASYNC_WORKER_TASK_PRIORITY and Restart/Sleep), don't set it higher then 10...
                       &effectTaskHandle,              // task handle
                       1                               // core (0 or 1)
  );

  xTaskCreateUniversal(driverTask,                     // task function
                       "AppDriverTask",                // name
                       (psramFound() ? 6 : 4) * 1024,  // d0-tuning... stack size
                       NULL,                           // parameter
                       3,                              // priority (between 5 and 10: ASYNC_WORKER_TASK_PRIORITY and Restart/Sleep), don't set it higher then 10...
                       &driverTaskHandle,              // task handle
                       1                               // core (0 or 1)
  );
  #endif

  // run UI stuff in the sveltekit task
  esp32sveltekit.addLoopFunction([]() {

  #if FT_ENABLED(FT_MOONLIGHT)
    moduleEffects.loop();        // requestUIUpdate
    moduleDrivers.loop();        // requestUIUpdate
    moduleLightsControl.loop();  // monitor
  #endif

    // every second
    static unsigned long lastSecond = 0;
    if (millis() - lastSecond >= 1000) {
      lastSecond = millis();

      moduleDevices.loop1s();
      moduleTasks.loop1s();

  #if FT_ENABLED(FT_MOONLIGHT)
      // set shared data (eg used in scrolling text effect)
      sharedData.fps = esp32sveltekit.getAnalyticsService()->lps;
      sharedData.connectionStatus = (uint8_t)esp32sveltekit.getConnectionStatus();
      sharedData.clientListSize = esp32sveltekit.getServer()->getClientList().size();
      sharedData.connectedClients = esp32sveltekit.getSocket()->getConnectedClients();

    #if FT_ENABLED(FT_LIVESCRIPT)
      moduleLiveScripts.loop1s();
    #endif
  #endif
    }

    // every 10 seconds
    static unsigned long last10Second = 0;
    if (millis() - last10Second >= 10000) {
      last10Second = millis();

      moduleDevices.loop10s();
    }
  });

#endif  // FT_MOONBASE

#if USE_M5UNIFIED
  auto cfg = M5.config();
  M5.begin(cfg);
  // M5.Display.fillScreen(BLACK);
  #if USE_M5UNIFIEDDisplay
  M5.Display.drawPng(moonmanpng, moonmanpng_len, 0, 0, 0, 0, 0, 0, 100 / 320, 100 / 320);

    // M5.Display.fillRect(50, 50, 100, 100, RED);
    // M5.Display.setTextColor(WHITE);
    // M5.Display.setTextSize(2);
    // M5.Display.drawString("MoonLight", 0, 0);
  #endif
#endif
}

void loop() {
#if USE_M5UNIFIEDDDisplay
  M5.update();
  delay(100);
#else
  // Delete Arduino loop task, as it is not needed in this example
  vTaskDelete(NULL);
#endif
}