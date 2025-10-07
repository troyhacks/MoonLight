#pragma once

/**
 *   ESP32 SvelteKit
 *
 *   A simple, secure and extensible framework for IoT projects for ESP32 platforms
 *   with responsive Sveltekit front-end built with TailwindCSS and DaisyUI.
 *   https://github.com/theelims/ESP32-sveltekit
 *
 *   Copyright (C) 2023 - 2025 theelims
 *
 *   All Rights Reserved. This software may be modified and distributed under
 *   the terms of the LGPL v3 license. See the LICENSE file for details.
 **/

#include <EventSocket.h>
#include <JsonUtils.h>

#define EVENT_BATTERY "battery"

class BatteryService
{
public:
    BatteryService(EventSocket *socket);

    void begin();

    void updateSOC(float stateOfCharge);
    
    void setCharging(boolean isCharging);
    boolean isCharging();

    void updateVoltage(float voltage); // 🌙
    void updateCurrent(float current); // 🌙

    int getSOC();

private:
    EventSocket *_socket;
    int _lastSOC = -1; // 🌙
    float _lastVoltage = -1; // 🌙
    float _lastCurrent = -1; // 🌙
    boolean _isCharging = false;

    void batteryEvent();
};
