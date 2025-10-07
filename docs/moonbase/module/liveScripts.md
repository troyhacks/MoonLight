# Live Scripts module

<img width="396" alt="image" src="https://github.com/user-attachments/assets/953346f2-780d-4c61-8e73-7ba7bd228041" />

MoonLight is based on nodes (see [Effects](https://moonmodules.org/MoonLight/moonbase/module/effects/) and [Drivers](https://moonmodules.org/MoonLight/moonbase/module/drivers/)) where each node can run precompiled layouts, effects or modifiers but alternatively, each node can also run a live script, uploaded on the esp32 file system (.sc files).
Currently this only works on esp32-S3 boards as normal esp32 boards do not have enough memory to flash MoonLight with the Live Scripts option.

This module shows all running live scripts

* Scrips: Running Live scripts (🚧)
* Press the edit button to stop start or kill a script (current bug: double click the button)

## How to run a live script

**Step 1**: Select [Moonbase / File Manager](https://moonmodules.org/MoonLight/moonbase/files/) from the menu and select a location to store live scripts. Create a **folder** if needed (press the second + button):

<img width="300" alt="image" src="https://github.com/user-attachments/assets/85eeccf2-3f0d-4bf2-ba0a-e3407ff05fc2" />

**Step 2**: Go to this folder and create a **file** (press the first + button). Give the file a name ending with .sc and enter the code. See [github livescripts](https://github.com/MoonModules/MoonLight/tree/main/misc/livescripts) for example scripts (E_*.sc files are effects L_*.sc files are layouts). Alternatively you can also upload a live script with by pressing the Upload File button. Press save:

<img width="300" alt="image" src="https://github.com/user-attachments/assets/3b7eca3c-ae57-43f6-910e-3fc03f6fa380" />

**Step 3**: Select [MoonLight / Effects](https://moonmodules.org/MoonLight/moonbase/module/effects/) or [MoonLight / Drivers](https://moonmodules.org/MoonLight/moonbase/module/drivers/) from the menu. Create a new effect node (🔥) by pressing the + button or update an existing effect node (🔥) by pressing the ✐-button. Open the drop down and scroll to the bottom where live script files are shown

<img width="300" alt="image" src="https://github.com/user-attachments/assets/60f99421-aa74-4aa7-805d-05125cc5f222" />

**Step 4**: Select the script, the script will be compiled (takes a second or so) and executed. You can see the effect controls on the bottom of the screen (speed and branches in this example), change them to customise the effect: 

<img width="398" alt="image" src="https://github.com/user-attachments/assets/0ccb7e23-c3cc-4dfa-8d89-9fc86b1ff5f5" />

**Step 5**: Live scripts can be edited in the node press the ✐-button of a live script node and press the v next to Edit <livescript.sc> and the same edit window as in Step 2 opens. Do your edits and press save. The  script recompiles and will restart the effect. (Alternatively if a script is running and you edit a file in the file manager, the script will also recompile and rerun). For a list of functions currently supported, see [live script functions](https://github.com/MoonModules/MoonLight/blob/3058728e043a3f956e79cf88d99684d02d6c2b38/src/MoonLight/Nodes.cpp#L136-L181). This list is dynamic and will increase overtime, send a [ping](https://discord.com/channels/700041398778331156/1369578126450884608) if you would like function(s) to be added.

<img width="300" alt="image" src="https://github.com/user-attachments/assets/4552d564-85ea-454f-a6c4-5265146cdcfc" />

**Step 6**: An overview of running live scripts can be found in [MoonLight / LiveScripts](https://moonmodules.org/MoonLight/moonbase/module/liveScripts) 

