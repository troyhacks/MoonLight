# Tasks module

<img width="320" src="https://github.com/user-attachments/assets/3b6dd789-b281-4131-abad-0b7e8538de3e" />
<img width="320" src="https://github.com/user-attachments/assets/b3e50754-e58b-4b6c-8cdf-0b3a99dd3061" />

Shows system tasks info.

* Core 0: which task is currently running on core 0
* Core 1: which task is currently running on core 1

* Per task:
    * name: task name
    * Summary
       * state: Ready (🧍‍♂️), Running (🏃‍♂️), Blocked (🚧), Suspended (⏸️), deleted (🗑️), unknown (❓)
       * cpu: percentage of CPU time (per core) used
       * prio: task priority
    * stack: stack left
    * runtime: amount of cpu cycles consumed
    * core: allocated core, not necessarily used core (see above)
