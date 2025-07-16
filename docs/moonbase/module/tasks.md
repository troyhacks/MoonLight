# Tasks module

<img width="522" alt="image" src="https://github.com/user-attachments/assets/68cbbef1-bcd7-4333-a4d9-904d3e08b243" />

Shows system tasks info.

* Core 0: which task is currently running on core 0
* Core 1: which task is currently running on core 1

* Per task:
    * name: task name
    * state: running, blocking, ...
    * cpu: percentage of CPU time (per core) used
    * prio: task priority
    * stack: stack left
    * runtime: amount of cpu cycles consumed
    * core: allocated core, not necessarily used core
