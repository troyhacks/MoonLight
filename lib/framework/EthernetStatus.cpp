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

#include <EthernetStatus.h>

EthernetStatus::EthernetStatus(PsychicHttpServer *server,
                   SecurityManager *securityManager,
                   EthernetSettingsService *ethernetSettingsService) : _server(server),
                                                           _securityManager(securityManager),
                                                           _ethernetSettingsService(ethernetSettingsService)
{
}
void EthernetStatus::begin()
{
    _server->on(ETHERNET_STATUS_SERVICE_PATH,
                HTTP_GET,
                _securityManager->wrapRequest(std::bind(&EthernetStatus::ethernetStatus, this, std::placeholders::_1),
                                              AuthenticationPredicates::IS_AUTHENTICATED));

    ESP_LOGV(SVK_TAG, "Registered GET endpoint: %s", ETHERNET_STATUS_SERVICE_PATH);
}

esp_err_t EthernetStatus::ethernetStatus(PsychicRequest *request)
{
    PsychicJsonResponse response = PsychicJsonResponse(request, false);
    JsonObject root = response.getRoot();

    root["status"] = _ethernetSettingsService->getEthernetNetworkStatus();
    root["ip_address"] = WiFi.softAPIP().toString();
    root["mac_address"] = WiFi.softAPmacAddress();
    root["station_num"] = WiFi.softAPgetStationNum();

    return response.send();
}

bool EthernetStatus::isActive()
{
    return _ethernetSettingsService->getEthernetNetworkStatus() == EthernetNetworkStatus::E_ACTIVE ? true : false;
}
