/**
    @title     MoonBase
    @file      ModuleTasks.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/moonbase/tasks/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact us for more information.
**/

#ifndef ModuleTasks_h
#define ModuleTasks_h

#if FT_MOONBASE == 1

  #include "MoonBase/Module.h"

class ModuleTasks : public Module {
 public:
  ModuleTasks(PsychicHttpServer* server, ESP32SvelteKit* sveltekit) : Module("tasks", server, sveltekit) { EXT_LOGV(MB_TAG, "constructor"); }

  void setupDefinition(JsonArray root) override {
    EXT_LOGV(MB_TAG, "");
    JsonObject control;  // state.data has one or more properties
    JsonArray details;   // if a control is an array, this is the details of the array
    JsonArray values;    // if a control is a select, this is the values of the select

  #ifndef CONFIG_IDF_TARGET_ESP32C3
    control = addControl(root, "core0", "text", 0, 32, true);
    control = addControl(root, "core1", "text", 0, 32, true);
  #endif

    control = addControl(root, "tasks", "rows");
    control["filter"] = "";
    details = control["n"].to<JsonArray>();
    {
      control = addControl(details, "name", "text", 0, 32, true);
      control = addControl(details, "summary", "text", 0, 32, true);
      // control = details.add<JsonObject>(); control["name"] = "state"; control["type"] = "text"; control["ro"] = true;
      // control = details.add<JsonObject>(); control["name"] = "cpu"; control["type"] = "text"; control["ro"] = true;
      // control = details.add<JsonObject>(); control["name"] = "prio"; control["type"] = "number"; control["ro"] = true;
      control = addControl(details, "stack", "number", 0, 65538, true);
      control = addControl(details, "runtime", "text", 0, 32, true);
      control = addControl(details, "core", "number", 0, 65538, true);
    }
  }

  void loop1s() {
    if (!_socket->getConnectedClients()) return;  // 🌙 No need for UI tasks
    if (!WiFi.localIP() && !ETH.localIP()) return;

  #define MAX_TASKS 30

    TaskStatus_t taskStatusArray[MAX_TASKS];
    UBaseType_t taskCount;
    uint32_t totalRunTime = 1;

    // Get all tasks' info
    taskCount = uxTaskGetSystemState(taskStatusArray, MAX_TASKS, &totalRunTime);

    // Sort the taskStatusArray by task name
    std::sort(taskStatusArray, taskStatusArray + taskCount, [](const TaskStatus_t& a, const TaskStatus_t& b) { return strcmp(a.pcTaskName, b.pcTaskName) < 0; });

    // printf("Found %d tasks\n", taskCount);
    // printf("Name\t\tState\tPrio\tStack\tRun Time\tCPU %%\tCore\n");

    JsonDocument root;
    root["tasks"].to<JsonArray>();

    for (UBaseType_t i = 0; i < taskCount; i++) {
      JsonObject task = root["tasks"].as<JsonArray>().add<JsonObject>();

      TaskStatus_t* ts = &taskStatusArray[i];

      const char* state;
      switch (ts->eCurrentState) {
      case eRunning:
        state = "🏃‍♂️";
        break;  // Running
      case eReady:
        state = "🧍‍♂️";
        break;  // Ready
      case eBlocked:
        state = "🚧";
        break;  // Blocked
      case eSuspended:
        state = "⏸️";
        break;  // Suspended
      case eDeleted:
        state = "🗑️";
        break;  // Deleted
      default:
        state = "❓";
        break;  // Unknown
      }

      Char<32> summary;
      summary.format("%s %d%% @ P%d", state, (uint32_t)(100ULL * ts->ulRunTimeCounter / totalRunTime), ts->uxCurrentPriority);

      task["name"] = ts->pcTaskName;
      task["summary"] = summary.c_str();
      // task["state"] = state;
      // task["cpu"] = cpu_percent.c_str();
      // task["prio"] = ts->uxCurrentPriority;
      task["stack"] = ts->usStackHighWaterMark;
      task["runtime"] = ts->ulRunTimeCounter / 1000000;  // in seconds
      task["core"] = ts->xCoreID == tskNO_AFFINITY ? -1 : ts->xCoreID;

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
    // _socket->emitEvent(_moduleName, object);
    update(object, ModuleState::update, _moduleName + "server");
  }
};

#endif
#endif