/**
    @title     MoonLight
    @file      ModuleAnimations.h
    @repo      https://github.com/ewowi/MoonBase, submit changes to this file as PRs
    @Authors   https://github.com/ewowi/MoonBase/commits/main
    @Doc       https://ewowi.github.io/MoonBase/modules/module/animations/
    @Copyright © 2025 Github MoonBase Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
**/

#ifndef ModuleAnimations_h
#define ModuleAnimations_h

#if FT_MOONLIGHT == 1

#undef TAG
#define TAG "💫"

#include "FastLED.h"
#include "../MoonBase/Module.h"

#if FT_LIVESCRIPT
    #include "ESPLiveScript.h" //note: contains declarations AND definitions, therefore can only be included once!
#endif

#include "Effect.h" //effect.h will include VirtualLayer.h which will include PhysicalLayer.h

PhysicalLayer layerP; //global declaration of the physical layer

//to do: move to virtual layer ...
void fadeToBlackBy_static(uint8_t fadeValue)
{
    layerP.layerV[0]->fadeToBlackBy(fadeValue);
}

static void _addPin(uint8_t pinNr) {layerP.addPin(pinNr);}
static void _addPixelsPre() {layerP.addPixelsPre();}
static void _addPixel(uint16_t x, uint16_t y, uint16_t z) {layerP.addPixel({x, y, z});}
static void _addPixelsPost() {layerP.addPixelsPost();}

class ModuleAnimations : public Module
{
public:

    #if FT_LIVESCRIPT
        Parser parser = Parser();
    #endif

    bool animationsChanged = false;

    ModuleAnimations(PsychicHttpServer *server,
        ESP32SvelteKit *sveltekit,
        FilesService *filesService
    ) : Module("animations", server, sveltekit, filesService) {
        ESP_LOGD(TAG, "constructor");

        addExternalVariable("leds", "CRGB *", "", (void *)layerP.leds);
        addExternalFunction("fadeToBlackBy", "void", "uint8_t", (void *)fadeToBlackBy_static);
        uint16_t (*_random16)(uint16_t)=random16; //enforce specific random16 function
        addExternalFunction("random16", "uint16_t", "uint16_t", (void *)_random16);
        addExternalFunction("sin8", "uint8_t", "uint8_t", (void *)sin8);
        addExternalFunction("cos8", "uint8_t", "uint8_t", (void *)cos8);
        addExternalFunction("delay", "void", "uint8_t", (void *)delay);
        addExternalFunction("addPin", "void", "uint8_t", (void *)_addPin);
        addExternalFunction("addPixelsPre", "void", "", (void *)_addPixelsPre);
        addExternalFunction("addPixel", "void", "uint16_t,uint16_t,uint16_t", (void *)_addPixel);
        addExternalFunction("addPixelsPost", "void", "", (void *)_addPixelsPost);
        
        for (asm_external el: external_links) {
            ESP_LOGD(TAG, "elink %s %s %d", el.shortname.c_str(), el.name.c_str(), el.type);
        }
        // ... send to client
    }

    void begin() {
        Module::begin();

        ESP_LOGD(TAG, "");

        //create a handler which recompiles the animation when the file of the current animation changes in the File Manager
        _filesService->addUpdateHandler([&](const String &originId)
        { 
            ESP_LOGD(TAG, "FilesService::updateHandler %s", originId.c_str());
            //read the file state
            _filesService->read([&](FilesState &filesState) {
                // loop over all changed files (normally only one)
                for (auto updatedItem : filesState.updatedItems) {
                    //if file is the current animation, recompile it (to do: multiple animations)
                    for (JsonObject node: _state.data["nodes"].as<JsonArray>()) {
                        String animation = node["animation"];

                        if (updatedItem == animation) {
                            ESP_LOGD(TAG, "updateHandler updatedItem %s", updatedItem.c_str());
                            compileAndRun(node["animation"], node["type"], node["error"]);
                        }
                    }
                }
            });
        });

        // _socket->registerEvent("animationsRO");
    }

    //define the data model
    void setupDefinition(JsonArray root) override {
        ESP_LOGD(TAG, "");
        JsonObject property; // state.data has one or more properties
        JsonArray details = root; // if a property is an array, this is the details of the array
        JsonArray values; // if a property is a select, this is the values of the select

        property = root.add<JsonObject>(); property["name"] = "lightsOn"; property["type"] = "checkbox"; property["default"] = true;
        property = root.add<JsonObject>(); property["name"] = "brightness"; property["type"] = "range"; property["min"] = 0; property["max"] = 255; property["default"] = 10;
        property = root.add<JsonObject>(); property["name"] = "red"; property["type"] = "range"; property["min"] = 0; property["max"] = 255; property["default"] = 128; property["color"] = "Red";
        property = root.add<JsonObject>(); property["name"] = "green"; property["type"] = "range"; property["min"] = 0; property["max"] = 255; property["default"] = 128; property["color"] = "Green";
        property = root.add<JsonObject>(); property["name"] = "blue"; property["type"] = "range"; property["min"] = 0; property["max"] = 255; property["default"] = 1280; property["color"] = "Blue";
        property = root.add<JsonObject>(); property["name"] = "preset"; property["type"] = "select"; property["default"] = "solid"; values = property["values"].to<JsonArray>();
        values.add("Preset1");
        values.add("Preset2");
        property = root.add<JsonObject>(); property["name"] = "driverOn"; property["type"] = "checkbox"; property["default"] = true;
        property = root.add<JsonObject>(); property["name"] = "pin"; property["type"] = "select"; property["default"] = 2; values = property["values"].to<JsonArray>();
        values.add("2");
        values.add("16");

        property = root.add<JsonObject>(); property["name"] = "nodes"; property["type"] = "array"; details = property["n"].to<JsonArray>();
        {
            property = details.add<JsonObject>(); property["name"] = "animation"; property["type"] = "selectFile"; property["default"] = "Random"; values = property["values"].to<JsonArray>();
            values.add("Solid");
            values.add("Random");
            values.add("Sinelon");
            values.add("Rainbow");
            values.add("Sinus");
            values.add("Lissajous");
            values.add("Lines");
            //find all the .sc files on FS
            File rootFolder = ESPFS.open("/");
            walkThroughFiles(rootFolder, [&](File folder, File file) {
                if (strstr(file.name(), ".sc")) {
                    // ESP_LOGD(TAG, "found file %s", file.path());
                    values.add((char *)file.path());
                }
            });
            rootFolder.close();
            property = details.add<JsonObject>(); property["name"] = "type"; property["type"] = "select"; property["default"] = "Effect"; values = property["values"].to<JsonArray>();
            values.add("Fixture definition");
            values.add("Fixture mapping");
            values.add("Effect");
            values.add("Modifier");
            values.add("Driver show");
            property = details.add<JsonObject>(); property["name"] = "error"; property["type"] = "text"; property["ro"] = true;
            property = details.add<JsonObject>(); property["name"] = "controls"; property["type"] = "array"; details = property["n"].to<JsonArray>();
            {
                property = details.add<JsonObject>(); property["name"] = "name"; property["type"] = "text"; property["default"] = "speed";
                property = details.add<JsonObject>(); property["name"] = "type"; property["type"] = "select"; property["default"] = "Number"; values = property["values"].to<JsonArray>();
                values.add("Number");
                values.add("Range");
                values.add("Text");
                values.add("Coordinate");
                property = details.add<JsonObject>(); property["name"] = "value"; property["type"] = "text"; property["default"] = "128";
            }
            //, "controls":[{"name":"speed","type":"range", "value":"128"}]
        }

        property = root.add<JsonObject>(); property["name"] = "scripts"; property["type"] = "array"; details = property["n"].to<JsonArray>();
        {
            property = details.add<JsonObject>(); property["name"] = "name"; property["type"] = "text"; property["ro"] = true;
            property = details.add<JsonObject>(); property["name"] = "isRunning"; property["type"] = "checkbox"; property["ro"] = true;
            property = details.add<JsonObject>(); property["name"] = "isHalted"; property["type"] = "checkbox"; property["ro"] = true;
            property = details.add<JsonObject>(); property["name"] = "exeExist"; property["type"] = "checkbox"; property["ro"] = true;
            property = details.add<JsonObject>(); property["name"] = "handle"; property["type"] = "number"; property["ro"] = true;
            property = details.add<JsonObject>(); property["name"] = "binary_size"; property["type"] = "number"; property["ro"] = true;
            property = details.add<JsonObject>(); property["name"] = "data_size"; property["type"] = "number"; property["ro"] = true;
            property = details.add<JsonObject>(); property["name"] = "error"; property["type"] = "text"; property["ro"] = true;
            property = details.add<JsonObject>(); property["name"] = "kill"; property["type"] = "button";
        }
    }

    //implement business logic
    void onUpdate(UpdatedItem &updatedItem) override
    {
        if (equal(updatedItem.name, "pin") || equal(updatedItem.name, "red") || equal(updatedItem.name, "green") || equal(updatedItem.name, "blue")) {
            ESP_LOGD(TAG, "handle %s = %s -> %s", updatedItem.name, updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());

            //addLeds twice is temp hack to make rgb sliders work
            switch (_state.data["pin"].as<int>()) {
                case 2:
                    FastLED.addLeds<WS2812B, 16, GRB>(layerP.leds, 0, layerP.nrOfLeds).setCorrection(CRGB(_state.data["red"],_state.data["green"],_state.data["blue"]));
                    FastLED.addLeds<WS2812B, 2, GRB>(layerP.leds, 0, layerP.nrOfLeds).setCorrection(CRGB(_state.data["red"],_state.data["green"],_state.data["blue"]));
                    break;
                case 16:
                    FastLED.addLeds<WS2812B, 2, GRB>(layerP.leds, 0, layerP.nrOfLeds).setCorrection(CRGB(_state.data["red"],_state.data["green"],_state.data["blue"]));
                    FastLED.addLeds<WS2812B, 16, GRB>(layerP.leds, 0, layerP.nrOfLeds).setCorrection(CRGB(_state.data["red"],_state.data["green"],_state.data["blue"]));
                    break;
                default:
                    ESP_LOGD(TAG, "unknown pin %d", _state.data["pin"].as<int>());
            }
            FastLED.setMaxPowerInMilliWatts(10000); // 5v, 2000mA, to protect usb while developing
            FastLED.setBrightness(_state.data["lightsOn"]?_state.data["brightness"]:0);
            ESP_LOGD(TAG, "FastLED.addLeds n:%d", layerP.nrOfLeds);
        } else if (equal(updatedItem.name, "lightsOn") || equal(updatedItem.name, "brightness")) {
            ESP_LOGD(TAG, "handle %s = %s -> %s", updatedItem.name, updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
            FastLED.setBrightness(_state.data["lightsOn"]?_state.data["brightness"]:0);
        } else if (equal(updatedItem.parent[0], "nodes") && (equal(updatedItem.name, "animation") || equal(updatedItem.name, "type"))) {    
            animationsChanged = true;
            JsonVariant node = _state.data["nodes"][updatedItem.index[0]];
            ESP_LOGD(TAG, "handle %s = %s -> %s", updatedItem.name, updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
            if (updatedItem.oldValue.length())
                ESP_LOGD(TAG, "delete %s %s ...", updatedItem.name, updatedItem.oldValue.c_str());
            if (!node["animation"].isNull() && !node["type"].isNull())
                compileAndRun(node["animation"], node["type"], node["error"]);
        } else if (equal(updatedItem.parent[0], "scripts") && equal(updatedItem.name, "kill")) {    
            ESP_LOGD(TAG, "handle %s[%d].%s = %s -> %s (%d)", updatedItem.parent[0], updatedItem.index[0], updatedItem.name, updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str(), updatedItem.oldValue.length());
            if (updatedItem.oldValue != "null") //do not run at boot!
                kill(updatedItem.index[0]);
        } 
        // else
        //     ESP_LOGD(TAG, "no handle for %s = %s -> %s", updatedItem.name, updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
    }

    //run effects
    void loop()
    {
        bool showLeds = true;

        if (animationsChanged) {
            animationsChanged = false;
            
            //rebuild layerP->layerV->effects
            for (Effect* effect : layerP.layerV[0]->effects) {
                ESP_LOGD(TAG, "delete effect %s", effect->name());
                delete effect;
            }
            layerP.layerV[0]->effects.clear(); //remove all effects
            for (JsonObject node: _state.data["nodes"].as<JsonArray>()) {
                ESP_LOGD(TAG, "create effect %s", node["animation"].as<String>().c_str());
                layerP.addEffect(node["animation"]); // fill the layers and effects ...
            }
        }

        layerP.loop(); //run all the effects of all virtual layers (currently only one)

        //show connected clients on the led display
        // static uint8_t lastConnectedClients = 0;
        // if (_socket->getConnectedClients() == 0) lastConnectedClients++;
        for (int i = 0; i < _socket->getConnectedClients(); i++) {
            // ESP_LOGD(TAG, "socket %d", i);
            layerP.leds[i] = CRGB(0, 0, 128);
        }

        // Serial.printf(" %s", animation.c_str());
        if (showLeds) driverShow();
    }

    //update scripts / read only values in the UI
    void loop1s() {
        if (!_socket->getConnectedClients()) return; 

        JsonDocument newData; //to only send updatedData

        //push read only variables
        //use state.data or newData?

        JsonArray scripts = newData["scripts"].to<JsonArray>(); //to: remove old array

        for (Executable &exec: scriptRuntime._scExecutables) {
            exe_info exeInfo = scriptRuntime.getExecutableInfo(exec.name);
            JsonObject object = scripts.add<JsonObject>();
            object["name"] = exec.name;
            object["isRunning"] = exec.isRunning();
            object["isHalted"] = exec.isHalted;
            object["exeExist"] = exec.exeExist;
            object["handle"] = exec.__run_handle_index;
            object["binary_size"] = exeInfo.binary_size;
            object["data_size"] = exeInfo.data_size;
            object["error"] = exec.error.error_message;
            object["kill"] = 0;
            // ESP_LOGD(TAG, "scriptRuntime exec %s r:%d h:%d, e:%d h:%d b:%d + d:%d = %d", exec.name.c_str(), exec.isRunning(), exec.isHalted, exec.exeExist, exec.__run_handle_index, exeInfo.binary_size, exeInfo.data_size, exeInfo.total_size);
        }

        //only if changed
        if (_state.data["scripts"] != newData["scripts"]) {
            _state.data["scripts"] = newData["scripts"]; //update without compareRecursive -> without handles
            JsonObject newDataObject = newData.as<JsonObject>();
            _socket->emitEvent("animations", newDataObject);
        }
            
        // char buffer[256];
        // serializeJson(doc, buffer, sizeof(buffer));
        // ESP_LOGD(TAG, "livescripts %s", buffer);
    }

    void driverShow()
    {
        if (_state.data["driverOn"])
            FastLED.show();
    }

    //ESPLiveScript functions
    //=======================

    static void show()
    {
        delay(1); //to feed the watchdog (also if loopState == 0)

        // gAnimations->driverShow();
    }

    void compileAndRun(const char * animation, const char * type, JsonVariant error) {

        #if FT_LIVESCRIPT
        
            ESP_LOGD(TAG, "animation %s %s", animation, type);

            if (animation[0] != '/') { //no sc script
                return;
            }

            //if this animation is already running, kill it ...

            // ESP_LOGD(TAG, "killAndFreeRunningProgram %s", animation.c_str());
            // runInLoopTask.push_back([&] { // run in loopTask to avoid stack overflow
            //     scriptRuntime.killAndFreeRunningProgram();
            // });

            //send UI spinner

            runningPrograms.setFunctionToSync(show);
        
            //run the recompile not in httpd but in main loopTask (otherwise we run out of stack space)
            // runInLoopTask.push_back([&, animation, type, error] {
                ESP_LOGD(TAG, "compileAndRun %s %s", animation, type);
                File file = ESPFS.open(animation);
                if (file) {
                    std::string scScript = file.readString().c_str();
                    // scScript += "void main(){setup();sync();}";
                    file.close();

                    // gAnimations = this;
                    if (equal(type, "Effect")) {
                        scScript += "#define NUM_LEDS " + std::to_string(layerP.nrOfLeds) + "\n"; //NUM_LEDS is used in arrays -> must be define e.g. uint8_t rMapRadius[NUM_LEDS];
                        scScript += "void main(){setup();while(2>1){loop();delay(1);}}"; //sync();
                    } else if (equal(type, "Fixture definition")) {
                        scScript += "void main(){addPixelsPre();setup();addPixelsPost();}";
                    }

                    // TaskHandle_t currentTask = xTaskGetCurrentTaskHandle();
                    // ESP_LOGI(TAG, "task %s %d", pcTaskGetName(currentTask), uxTaskGetStackHighWaterMark(currentTask));
            
                    // ESP_LOGD(TAG, "parsing %s", scScript.c_str());

                    Executable executable = parser.parseScript(&scScript);
                    executable.name = animation;
                    ESP_LOGD(TAG, "parsing %s done\n", animation);
                    scriptRuntime.addExe(executable); //if already exists, delete it first
                    ESP_LOGD(TAG, "addExe success %s\n", executable.exeExist?"true":"false");
        
                    if (executable.exeExist) {
                        if (equal(type, "Fixture definition")) {
                            executable.execute("main"); //background task (async - vs sync)
                            // pass = 1;
                            // liveM->executeTask(liveFixtureID, "c");
                            // pass = 2;
                            // liveM->executeTask(liveFixtureID, "c");
                        }
                        if (equal(type, "Effect")) {
                            // setup : create controls
                            // executable.execute("setup"); 
                            // send controls to UI
                            executable.executeAsTask("main"); //background task (async - vs sync)
                            //assert failed: xEventGroupSync event_groups.c:228 (uxBitsToWaitFor != 0)
                        }
                    } else
                        ESP_LOGD(TAG, "error %s", executable.error.error_message.c_str());

                    //send error to client ... not working yet
                    error.set(executable.error.error_message); //String(executable.error.error_message.c_str());
                    // _state.data["nodes"][2]["error"] = executable.error.error_message;

                }
            // });
        #endif
    
        //stop UI spinner
    }

    void kill(int index) {
        ESP_LOGD(TAG, "kill %d", index);
        if (index < scriptRuntime._scExecutables.size()) {
            scriptRuntime.kill(scriptRuntime._scExecutables[index].name);
            scriptRuntime.free(scriptRuntime._scExecutables[index].name);
            scriptRuntime.deleteExe(scriptRuntime._scExecutables[index].name);
        }
    }
}; // class ModuleAnimations

#endif
#endif