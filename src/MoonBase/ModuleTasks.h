/**
    @title     MoonBase
    @file      ModuleTasks.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/moonbase/module/tasks/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact us for more information.
**/

#ifndef ModuleTasks_h
#define ModuleTasks_h

#if FT_MOONBASE == 1

#include "Module.h"

class ModuleTasks : public Module
{
public:

    ModuleTasks(PsychicHttpServer *server,
            ESP32SvelteKit *sveltekit
        ) : Module("tasks", server, sveltekit) {
            MB_LOGV(MB_TAG, "constructor");
    }

    void setupDefinition(JsonArray root) override{
        MB_LOGV(MB_TAG, "");
        JsonObject property; // state.data has one or more properties
        JsonArray details; // if a property is an array, this is the details of the array
        JsonArray values; // if a property is a select, this is the values of the select

        #ifndef CONFIG_IDF_TARGET_ESP32C3
            property = root.add<JsonObject>(); property["name"] = "core0"; property["type"] = "text"; property["ro"] = true;
            property = root.add<JsonObject>(); property["name"] = "core1"; property["type"] = "text"; property["ro"] = true;
        #endif

        property = root.add<JsonObject>(); property["name"] = "tasks"; property["type"] = "array"; details = property["n"].to<JsonArray>();
        {
            property = details.add<JsonObject>(); property["name"] = "name"; property["type"] = "text"; property["ro"] = true;
            property = details.add<JsonObject>(); property["name"] = "state"; property["type"] = "text"; property["ro"] = true;
            property = details.add<JsonObject>(); property["name"] = "cpu"; property["type"] = "text"; property["ro"] = true;
            property = details.add<JsonObject>(); property["name"] = "prio"; property["type"] = "number"; property["ro"] = true;
            property = details.add<JsonObject>(); property["name"] = "stack"; property["type"] = "number"; property["ro"] = true; property["max"] = 65538;
            property = details.add<JsonObject>(); property["name"] = "runtime"; property["type"] = "text"; property["ro"] = true;
            property = details.add<JsonObject>(); property["name"] = "core"; property["type"] = "number"; property["ro"] = true;
        }
    }

    void onUpdate(UpdatedItem &updatedItem) override
    {
        MB_LOGV(MB_TAG, "no handle for %s[%d]%s[%d].%s = %s -> %s", updatedItem.parent[0].c_str(), updatedItem.index[0], updatedItem.parent[1].c_str(), updatedItem.index[1], updatedItem.name.c_str(), updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
    }

    void loop1s() {

        if (!_socket->getConnectedClients()) return;  // 🌙 No need for UI tasks

        #define MAX_TASKS 30

        TaskStatus_t taskStatusArray[MAX_TASKS];
        UBaseType_t taskCount;
        uint32_t totalRunTime;

        // Get all tasks' info
        taskCount = uxTaskGetSystemState(taskStatusArray, MAX_TASKS, &totalRunTime);

        // Sort the taskStatusArray by task name
        std::sort(taskStatusArray, taskStatusArray + taskCount, [](const TaskStatus_t& a, const TaskStatus_t& b) {
            return strcmp(a.pcTaskName, b.pcTaskName) < 0;
        });

        // printf("Found %d tasks\n", taskCount);
        // printf("Name\t\tState\tPrio\tStack\tRun Time\tCPU %%\tCore\n");

        JsonDocument root;
        root["tasks"].to<JsonArray>();

        for (UBaseType_t i = 0; i < taskCount; i++) {

            JsonObject task = root["tasks"].as<JsonArray>().add<JsonObject>();

            TaskStatus_t *ts = &taskStatusArray[i];

            const char *state;
            switch (ts->eCurrentState) {
                case eRunning:   state = "Running"; break;
                case eReady:     state = "Ready"; break;
                case eBlocked:   state = "Blocked"; break;
                case eSuspended: state = "Suspended"; break;
                case eDeleted:   state = "Deleted"; break;
                default:         state = "Unknown"; break;
            }

            Char<32> cpu_percent;
            cpu_percent.format("%5.2f%%", totalRunTime ? (100.0f * ts->ulRunTimeCounter) / totalRunTime : 0.0f);

            task["name"] = ts->pcTaskName;
            task["state"] = state;
            task["cpu"] = cpu_percent.c_str();
            task["prio"] = ts->uxCurrentPriority;
            task["stack"] = ts->usStackHighWaterMark;
            task["runtime"] = ts->ulRunTimeCounter;
            task["core"] = ts->xCoreID==tskNO_AFFINITY?-1:ts->xCoreID;

            // printf("%-12s %-10s %4u\t%5u\t%10lu\t%s\t%d\n",
            //     ts->pcTaskName,
            //     state,
            //     ts->uxCurrentPriority,
            //     ts->usStackHighWaterMark,
            //     ts->ulRunTimeCounter,
            //     cpu_percent.c_str(), ts->xCoreID==tskNO_AFFINITY?-1:ts->xCoreID);
        }

        #ifndef CONFIG_IDF_TARGET_ESP32C3
            TaskHandle_t current0 = xTaskGetCurrentTaskHandleForCore(0);
            TaskHandle_t current1 = xTaskGetCurrentTaskHandleForCore(1);

            root["core0"] = pcTaskGetName(current0);
            root["core1"] = pcTaskGetName(current1);
        #endif

        
        // UpdatedItem updatedItem;
        // _state.compareRecursive("", _state.data, root, updatedItem); //fill data with doc

        JsonObject object = root.as<JsonObject>();
        _socket->emitEvent(_moduleName, object);
    }
};

#endif
#endif