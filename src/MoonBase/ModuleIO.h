/**
    @title     MoonBase
    @file      ModuleIO.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/moonbase/inputoutput/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact us for more information.
**/

#ifndef ModuleIO_h
#define ModuleIO_h

#if FT_MOONBASE == 1

#include "Module.h"

class ModuleIO : public Module
{
public:

    ModuleIO(PsychicHttpServer *server,
            ESP32SvelteKit *sveltekit
        ) : Module("inputoutput", server, sveltekit) {
        MB_LOGV(MB_TAG, "constructor");
        
        // #if CONFIG_IDF_TARGET_ESP32
        //     pinMode(19, OUTPUT); digitalWrite(19, HIGH); // for serg shield boards: to be done: move to new pin manager module, switch off for S3!!!! tbd: add pin manager
        // #endif


    }

    void setupDefinition(JsonArray root) override{
        MB_LOGV(MB_TAG, "");
        JsonObject property; // state.data has one or more properties
        JsonArray details; // if a property is an array, this is the details of the array
        JsonArray values; // if a property is a select, this is the values of the select

        property = root.add<JsonObject>(); property["name"] = "pins"; property["type"] = "array"; details = property["n"].to<JsonArray>();
        {
            property = details.add<JsonObject>(); property["name"] = "GPIO"; property["type"] = "number"; property["ro"] = true;
            property = details.add<JsonObject>(); property["name"] = "summary"; property["type"] = "text"; property["ro"] = true;
            // property = details.add<JsonObject>(); property["name"] = "Valid"; property["type"] = "checkbox"; property["ro"] = true;
            // property = details.add<JsonObject>(); property["name"] = "Output"; property["type"] = "checkbox"; property["ro"] = true;
            // property = details.add<JsonObject>(); property["name"] = "RTC"; property["type"] = "checkbox"; property["ro"] = true;
            property = details.add<JsonObject>(); property["name"] = "PinInfo"; property["type"] = "text"; property["ro"] = true;
            property = details.add<JsonObject>(); property["name"] = "Level"; property["type"] = "text"; property["ro"] = true;
            property = details.add<JsonObject>(); property["name"] = "DriveCap"; property["type"] = "text"; property["ro"] = true;
        }
    }

    void onUpdate(UpdatedItem &updatedItem) override
    {
        MB_LOGV(MB_TAG, "no handle for %s[%d]%s[%d].%s = %s -> %s", updatedItem.parent[0].c_str(), updatedItem.index[0], updatedItem.parent[1].c_str(), updatedItem.index[1], updatedItem.name.c_str(), updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
    }

    // Function to convert drive capability to string
const char* drive_cap_to_string(gpio_drive_cap_t cap) {
    switch(cap) {
        case GPIO_DRIVE_CAP_0: return "WEAK";
        case GPIO_DRIVE_CAP_1: return "STRONGER";
        case GPIO_DRIVE_CAP_2: return "MEDIUM";
        case GPIO_DRIVE_CAP_3: return "STRONGEST";
        default: return "UNKNOWN";
    }
}

    void loop1s() {

        if (!_socket->getConnectedClients()) return;  // 🌙 No need for UI 

        // _state.data["pins"].to<JsonArray>(); //empty

        JsonDocument root;
        root["pins"].to<JsonArray>();

        #ifdef LED_PINS
            Char<64> ledPins;
            ledPins = LED_PINS;
        #endif

        for (int gpio_num = 0; gpio_num < SOC_GPIO_PIN_COUNT; gpio_num++) {
            JsonObject task = root["pins"].as<JsonArray>().add<JsonObject>();

            // Check if GPIO is valid
            bool is_valid = GPIO_IS_VALID_GPIO(gpio_num);
            bool is_output_valid = GPIO_IS_VALID_OUTPUT_GPIO(gpio_num);
            bool is_rtc_gpio = rtc_gpio_is_valid_gpio((gpio_num_t)gpio_num);
            
            // Get current level (works for both input and output pins)
            int level = -1;
            if (is_valid) {
                level = gpio_get_level((gpio_num_t)gpio_num);
            }
            
            // For RTC GPIOs, can also use RTC-specific read
            int rtc_level = -1;
            #ifndef CONFIG_IDF_TARGET_ESP32C3
                if (is_rtc_gpio) {
                    rtc_level = rtc_gpio_get_level((gpio_num_t)gpio_num); //to do find c3 alternative
                }
            #endif
            
            // Get drive capability (if output capable)
            gpio_drive_cap_t drive_cap = GPIO_DRIVE_CAP_DEFAULT;
            esp_err_t drive_result = ESP_FAIL;
            if (is_output_valid) {
                drive_result = gpio_get_drive_capability((gpio_num_t)gpio_num, &drive_cap);
            }

            Char<32> summary;
            summary.format("%s%s%s", is_valid?"✅":"", is_output_valid?" 💡":"", is_rtc_gpio?" ⏰":"");
        
            task["GPIO"] = gpio_num;
            task["summary"] = summary.c_str();
            // task["Valid"] = is_valid;
            // task["Output"] = is_output_valid;
            // task["RTC"] = is_rtc_gpio;
            task["Level"] = (level >= 0) ? (level ? "HIGH" : "LOW") : "N/A";
            task["DriveCap"] = (drive_result == ESP_OK) ? drive_cap_to_string(drive_cap) : "N/A";
            task["PinInfo"] = "";
            #ifdef VOLTAGE_PIN
                if (gpio_num == VOLTAGE_PIN)
                    task["PinInfo"] = "Voltage";
            #endif
            #ifdef CURRENT_PIN
                if (gpio_num == CURRENT_PIN)
                    task["PinInfo"] = "Current";
            #endif
            #ifdef BATTERY_PIN
                if (gpio_num == BATTERY_PIN)
                    task["PinInfo"] = "Battery";
            #endif
            #ifdef LED_PINS

                ledPins.split(",",  [&](const char *token, uint8_t sequence) {
                    if (gpio_num == atoi(token)) {
                        // MB_LOGD(MB_TAG, "token: %s == %s (%d)", task["GPIO"].as<String>().c_str(), token, sequence);
                        Char<32> text;
                        text.format("LED #%d", sequence+1);
                        task["PinInfo"] = text.c_str();
                    }
                });
            #endif
        }

        // UpdatedItem updatedItem;
        // _state.compareRecursive("", _state.data, root, updatedItem); //fill data with doc

        // serializeJson(root, Serial); Serial.println();

        JsonObject object = root.as<JsonObject>();
        _socket->emitEvent(_moduleName, object);
    }
};

#endif
#endif


// format:
// {
//   "pins": [
//     {
//       "GPIO": 0,
//       "Valid": true,
//       "Output": true,
//       "RTC": true,
//       "Level": "HIGH",
//       "DriveCap": "MEDIUM",
//       "PinInfo": ""
//     },
//     {
//       "GPIO": 1,
//       "Valid": true,
//       "Output": true,
//       "RTC": false,
//       "Level": "HIGH",
//       "DriveCap": "MEDIUM",
//       "PinInfo": ""
//     },