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

#include <BatteryService.h>

BatteryService::BatteryService(EventSocket *socket) : _socket(socket)
{
}

void BatteryService::updateSOC(float stateOfCharge)
{
    _lastSOC = (int)round(stateOfCharge);
    batteryEvent();
}

void BatteryService::setCharging(boolean isCharging)
{
    _isCharging = isCharging;
    batteryEvent();
}

boolean BatteryService::isCharging()
{
    return _isCharging;
}

// 🌙
void BatteryService::updateVoltage(float voltage)
{
    _lastVoltage = voltage;
    batteryEvent();
}

// 🌙
void BatteryService::updateCurrent(float current)
{
    _lastCurrent = current;
    batteryEvent();
}

int BatteryService::getSOC()
{
    return _lastSOC;
}

void BatteryService::begin()
{
    _socket->registerEvent(EVENT_BATTERY);
}

void BatteryService::batteryEvent()
{
    if (!_socket->getConnectedClients()) return;  // 🌙 No need for UI tasks

    JsonDocument doc;
    doc["soc"] = _lastSOC;
    doc["charging"] = _isCharging;
    doc["voltage"] = _lastVoltage;
    doc["current"] = _lastCurrent;
    JsonObject jsonObject = doc.as<JsonObject>();
    _socket->emitEvent(EVENT_BATTERY, jsonObject);
}
