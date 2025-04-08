# Modules

<img width="829" alt="image" src="https://github.com/user-attachments/assets/3384f3ba-b5e6-4993-9a8b-80c25878e176" />

## Functional

A module is a generic building block to create server and UI functionality which can be activated through the menu.

MoonBase-Modules are inspired by WLED usermods, further developed in StarBase and now in MoonBase (using the ESP32-Sveltekit infrastructure)

See [Demo](module/demo.md) and [Animations](module/animations.md) for examples

Press the ? on any module to go to the documentation.

## Technical

With Moonbase Modules it is possible to create new module entirely from one c++ class by only defining a json document describing the data structure and a function catching all the changes in the data. Http endpont and websockets are created automatically. There is no need to create any UI code, it is entirely driven by the json document.

Each module has each own documentation which can be accessed by pressing the ? and is defined in the [docs](https://github.com/ewowi/MoonBase/tree/main/docs) folder.

To create a new module:

* Create a class which inherits from Module
* Call the Module constructor with the name of the module.
    * This name will be used to set up http rest api and webserver sockets
    * See [ModuleDemo.h](https://github.com/ewowi/MoonBase/blob/main/src/custom/ModuleDemo.h)

```cpp
class ModuleDemo : public Module
{
public:

ModuleDemo(PsychicHttpServer *server
      , ESP32SvelteKit *sveltekit
      , FilesService *filesService
    ) : Module("demo", server, sveltekit, filesService) {
        ESP_LOGD(TAG, "constructor");
    }
}
```

* Implement function setupDefinition to create a json document with the datastructure
    * Store data on the file system
    * Generate the UI
    * Initialy create the module data

```cpp
void setupDefinition(JsonArray root) override{
    JsonObject property;
    JsonArray details;
    JsonArray values;

    property = root.add<JsonObject>(); property["name"] = "hostName"; property["type"] = "text"; property["default"] = "MoonLight";
    property = root.add<JsonObject>(); property["name"] = "connectionMode"; property["type"] = "select"; property["default"] = "Signal Strength"; values = property["values"].to<JsonArray>();
    values.add("Offline");
    values.add("Signal Strength");
    values.add("Priority");

    property = root.add<JsonObject>(); property["name"] = "savedNetworks"; property["type"] = "array"; details = property["n"].to<JsonArray>();
    {
        property = details.add<JsonObject>(); property["name"] = "SSID"; property["type"] = "text"; property["default"] = "ewtr"; property["min"] = 3; property["max"] = 32; 
        property = details.add<JsonObject>(); property["name"] = "Password"; property["type"] = "password"; property["default"] = "";
    }

}

```

* Implement function onUpdate to define what happens if data changes
    * struct UpdatedItem defines the update (parent property (including index in case of multiple records), name of property and value)
    * This runs in the httpd / webserver task. To run it in the main (application task use runInLoopTask - see [ModuleAnimations](https://github.com/ewowi/MoonBase/blob/main/src/custom/ModuleAnimations.h)) - as httpd stack has been increased runInLoopTask is less needed

```cpp
    void onUpdate(UpdatedItem updatedItem) override
    {
        if (equal(updatedItem.name, "lightsOn") || equal(updatedItem.name, "brightness")) {
            ESP_LOGD(TAG, "handle %s = %s -> %s", updatedItem.name, updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
            FastLED.setBrightness(_state.data["lightsOn"]?_state.data["brightness"]:0);
        } else if (equal(updatedItem.parent[0], "nodes") && equal(updatedItem.name, "animation")) {    
            ESP_LOGD(TAG, "handle %s = %s -> %s", updatedItem.name, updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
            if (updatedItem.oldValue.length())
                ESP_LOGD(TAG, "delete %s ...", updatedItem.oldValue.c_str());
            if (updatedItem.value.as<String>().length())
                compileAndRun(updatedItem.value.as<String>().c_str());
        } else
            ESP_LOGD(TAG, "no handle for %s = %s -> %s", updatedItem.name, updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
    }
```

* Add the module in [main.cpp](https://github.com/ewowi/MoonBase/blob/main/src/main.cpp)

```cpp
ModuleDemo moduleDemo = ModuleDemo(&server, &esp32sveltekit, &filesService);
...
moduleDemo.begin();
...
moduleDemo.loop();
```

* Add the module in [menu.svelte](https://github.com/ewowi/MoonBase/blob/main/interface/src/routes/menu.svelte) (this will be automated in the future)

```ts
submenu: [
   {
      title: 'Module Demo',
      icon: BulbIcon,
      href: '/custom/module?module=demo',
      feature: page.data.features.liveanimation,
   },
]
```

* This is all to create a fully functioning new module

Moonbase-Modules is implemented in:

* [Module.h](https://github.com/ewowi/MoonBase/blob/main/src/custom/Module.h) and [Module.cpp](https://github.com/ewowi/MoonBase/blob/main/src/custom/Module.cpp) will generate all the required server code
* [Module.svelte](https://github.com/ewowi/MoonBase/blob/main/interface/src/routes/custom/module/Module.svelte) will deal with the UI
* [MultiInput.svelte](https://github.com/ewowi/MoonBase/blob/main/interface/src/lib/components/custom/MultiInput.svelte) is used by Module.svelte to display the right UI widget based on what is defined in the definition json
* Modifications done in [menu.svelte](https://github.com/ewowi/MoonBase/blob/main/interface/src/routes/menu.svelte) do identify a module by href and not by title alone
