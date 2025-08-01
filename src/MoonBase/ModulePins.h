/**
    @title     MoonBase
    @file      ModulePins.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/moonbase/module/tasks/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact us for more information.
**/

#ifndef ModulePins_h
#define ModulePins_h

#if FT_MOONBASE == 1

#include "Module.h"

class ModulePins : public Module
{
public:

    ModulePins(PsychicHttpServer *server,
            ESP32SvelteKit *sveltekit
        ) : Module("pins", server, sveltekit) {
            ESP_LOGD(TAG, "constructor");
    }

    void setupDefinition(JsonArray root) override{
        ESP_LOGD(TAG, "");
        JsonObject property; // state.data has one or more properties
        JsonArray details; // if a property is an array, this is the details of the array
        JsonArray values; // if a property is a select, this is the values of the select

        property = root.add<JsonObject>(); property["name"] = "tasks"; property["type"] = "array"; details = property["n"].to<JsonArray>();
        {
            property = details.add<JsonObject>(); property["name"] = "GPIO"; property["type"] = "number"; property["ro"] = true;
            property = details.add<JsonObject>(); property["name"] = "Valid"; property["type"] = "checkbox"; property["ro"] = true;
            property = details.add<JsonObject>(); property["name"] = "Output"; property["type"] = "checkbox"; property["ro"] = true;
            property = details.add<JsonObject>(); property["name"] = "RTC"; property["type"] = "checkbox"; property["ro"] = true;
            property = details.add<JsonObject>(); property["name"] = "Level"; property["type"] = "text"; property["ro"] = true;
            property = details.add<JsonObject>(); property["name"] = "DriveCap"; property["type"] = "text"; property["ro"] = true;
            property = details.add<JsonObject>(); property["name"] = "PinInfo"; property["type"] = "text"; property["ro"] = true;
        }
    }

    void onUpdate(UpdatedItem &updatedItem) override
    {
        ESP_LOGD(TAG, "no handle for %s[%d]%s[%d].%s = %s -> %s", updatedItem.parent[0], updatedItem.index[0], updatedItem.parent[1], updatedItem.index[1], updatedItem.name, updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
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


        _state.data["tasks"].to<JsonArray>(); //empty

        for (int gpio_num = 0; gpio_num < SOC_GPIO_PIN_COUNT; gpio_num++) {
            JsonObject task = _state.data["tasks"].as<JsonArray>().add<JsonObject>();

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
            if (is_rtc_gpio) {
                rtc_level = rtc_gpio_get_level((gpio_num_t)gpio_num);
            }
            
            // Get drive capability (if output capable)
            gpio_drive_cap_t drive_cap = GPIO_DRIVE_CAP_DEFAULT;
            esp_err_t drive_result = ESP_FAIL;
            if (is_output_valid) {
                drive_result = gpio_get_drive_capability((gpio_num_t)gpio_num, &drive_cap);
            }
        
            task["GPIO"] = gpio_num;
            task["Valid"] = is_valid;
            task["Output"] = is_output_valid;
            task["RTC"] = is_rtc_gpio;
            task["Level"] = (level >= 0) ? (level ? "HIGH" : "LOW") : "N/A";
            task["DriveCap"] = (drive_result == ESP_OK) ? drive_cap_to_string(drive_cap) : "N/A";
            // task["PinInfo"] = ;
        }

        JsonObject tasksData = _state.data.as<JsonObject>();
        _socket->emitEvent("tasks", tasksData);

    }

};

#endif
#endif