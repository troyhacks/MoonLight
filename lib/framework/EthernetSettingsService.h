#ifndef EthernetSettingsConfig_h
#define EthernetSettingsConfig_h

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

#include <SettingValue.h>
#include <HttpEndpoint.h>
#include <FSPersistence.h>
#include <JsonUtils.h>
// #include <WiFi.h>

// #include <DNSServer.h>
// #include <IPAddress.h>

//🌙 redundant as all set in factory_settings.ini
// #ifndef FACTORY_AP_PROVISION_MODE
// #define FACTORY_AP_PROVISION_MODE AP_MODE_DISCONNECTED
// #endif

// #ifndef FACTORY_AP_SSID
// #define FACTORY_AP_SSID "ESP32-SvelteKit-#{unique_id}"
// #endif

// #ifndef FACTORY_AP_PASSWORD
// #define FACTORY_AP_PASSWORD "esp-sveltekit"
// #endif

// #ifndef FACTORY_AP_LOCAL_IP
// #define FACTORY_AP_LOCAL_IP "192.168.4.1"
// #endif

// #ifndef FACTORY_AP_GATEWAY_IP
// #define FACTORY_AP_GATEWAY_IP "192.168.4.1"
// #endif

// #ifndef FACTORY_AP_SUBNET_MASK
// #define FACTORY_AP_SUBNET_MASK "255.255.255.0"
// #endif

// #ifndef FACTORY_AP_CHANNEL
// #define FACTORY_AP_CHANNEL 1
// #endif

// #ifndef FACTORY_AP_SSID_HIDDEN
// #define FACTORY_AP_SSID_HIDDEN false
// #endif

// #ifndef FACTORY_AP_MAX_CLIENTS
// #define FACTORY_AP_MAX_CLIENTS 4
// #endif

#define ETHERNET_SETTINGS_FILE "/.config/ethernetSettings.json" // 🌙 use /.config (hidden folder)
#define ETHERNET_SETTINGS_SERVICE_PATH "/rest/ethernetSettings"

// #define AP_MODE_ALWAYS 0
// #define AP_MODE_DISCONNECTED 1
// #define AP_MODE_NEVER 2

// #define MANAGE_NETWORK_DELAY 10000
// #define DNS_PORT 53

enum EthernetNetworkStatus
{
    E_ACTIVE = 0,
    E_INACTIVE,
    E_LINGERING
};

class EthernetSettings
{
public:

    IPAddress localIP;
    IPAddress gatewayIP;
    IPAddress subnetMask;

    bool operator==(const EthernetSettings &settings) const
    {
        return localIP == settings.localIP && gatewayIP == settings.gatewayIP && subnetMask == settings.subnetMask;
    }

    static void read(EthernetSettings &settings, JsonObject &root)
    {
        root["local_ip"] = settings.localIP.toString();
        root["gateway_ip"] = settings.gatewayIP.toString();
        root["subnet_mask"] = settings.subnetMask.toString();
    }

    static StateUpdateResult update(JsonObject &root, EthernetSettings &settings)
    {
        EthernetSettings newSettings = {};

        JsonUtils::readIPStr(root, "local_ip", newSettings.localIP, FACTORY_AP_LOCAL_IP);
        JsonUtils::readIPStr(root, "gateway_ip", newSettings.gatewayIP, FACTORY_AP_GATEWAY_IP);
        JsonUtils::readIPStr(root, "subnet_mask", newSettings.subnetMask, FACTORY_AP_SUBNET_MASK);

        if (newSettings == settings)
        {
            return StateUpdateResult::UNCHANGED;
        }
        settings = newSettings;
        return StateUpdateResult::CHANGED;
    }
};

class EthernetSettingsService : public StatefulService<EthernetSettings>
{
public:
    EthernetSettingsService(PsychicHttpServer *server, FS *fs, SecurityManager *securityManager);

    void begin();
    void loop();
    EthernetNetworkStatus getEthernetNetworkStatus();
    void recoveryMode();

private:
    PsychicHttpServer *_server;
    SecurityManager *_securityManager;
    HttpEndpoint<EthernetSettings> _httpEndpoint;
    FSPersistence<EthernetSettings> _fsPersistence;

    // // for the captive portal
    // DNSServer *_dnsServer;

    // for the mangement delay loop
    // volatile unsigned long _lastManaged;
    // volatile boolean _reconfigureAp;
    // volatile boolean _recoveryMode = false;

    void reconfigureAP();
    void manageAP();
    void startAP();
    void stopAP();
    // void handleDNS();
};

#endif // end EthernetSettingsConfig_h
