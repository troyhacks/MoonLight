# Tasks module

<img width="522" alt="image" src="https://github.com/user-attachments/assets/68cbbef1-bcd7-4333-a4d9-904d3e08b243" />

Shows system tasks info.

* task["name"] = ts->pcTaskName;
* task["state"] = state;
* task["cpu"] = cpu_percent.c_str();
* task["prio"] = ts->uxCurrentPriority;
* task["stack"] = ts->usStackHighWaterMark;
* task["runtime"] = ts->ulRunTimeCounter;
* task["core"] = ts->xCoreID==tskNO_AFFINITY?-1:ts->xCoreID;
