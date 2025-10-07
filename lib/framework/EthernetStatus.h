#ifndef EthernetStatus_h
#define EthernetStatus_h

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

#include <WiFi.h>

#include <ArduinoJson.h>
#include <PsychicHttp.h>
#include <IPAddress.h>
#include <SecurityManager.h>
#include <EthernetSettingsService.h>

#define ETHERNET_STATUS_SERVICE_PATH "/rest/ethernetStatus"

class EthernetStatus
{
public:
    EthernetStatus(PsychicHttpServer *server, SecurityManager *securityManager, EthernetSettingsService *ethernetSettingsService);

    void begin();

    bool isActive();

private:
    PsychicHttpServer *_server;
    SecurityManager *_securityManager;
    EthernetSettingsService *_ethernetSettingsService;
    esp_err_t ethernetStatus(PsychicRequest *request);
};

#endif // end EthernetStatus_h
