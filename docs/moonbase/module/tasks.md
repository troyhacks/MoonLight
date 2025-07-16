# Tasks module

<img width="522" src="https://github.com/user-attachments/assets/3934eec3-9382-45e1-9462-3968f4e86b4c" />

Shows system tasks info.

* Core 0: which task is currently running on core 0
* Core 1: which task is currently running on core 1

* Per task:
    * name: task name
    * state: Ready, Running, Blocked, Suspended
    * cpu: percentage of CPU time (per core) used
    * prio: task priority
    * stack: stack left
    * runtime: amount of cpu cycles consumed
    * core: allocated core, not necessarily used core (see above)
