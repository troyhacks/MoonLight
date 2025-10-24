/**
    @title     MoonLight
    @file      D__Sandbox.h (Driver sandbox)
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/moonlight/overview/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact us for more information.
**/


#if FT_MOONLIGHT

// example template, do not remove
// add effects in ModuleDrivers::addNodes()
class NewDriver: public Node {
  public:

  static const char * name() {return "New Driver";}
  static uint8_t dim() {return _NoD;} // Dimensions not relevant for drivers?
  static const char * tags() {return "☸️⏳";} // use emojis see https://moonmodules.org/MoonLight/moonlight/overview/#emoji-coding, ☸️ for drivers

  uint8_t pin = 16;

  void setup() override {
    // controls will show in the UI
    // for different type of controls see other Nodes
    addControl(pin, "pin", "range", 1, SOC_GPIO_PIN_COUNT);
  }
  
  void onUpdate(String &oldValue, JsonObject control) {
    // add your custom onUpdate code here
    if (control["name"] == "pin") {
      //...
    }
  }

  bool hasOnLayout() const override { return true; } // so the mapping system knows this node has onLayout, eg each time a modifier changes
  void onLayout() override {};

  // use for continuous actions, e.g. reading data from sensors or sending data to lights (e.g. LED drivers or Art-Net)
  void loop() override {
    pinMode(pin, INPUT); 
    int value = digitalRead(pin);
    sharedData; // write value to shared data if needed, add sharedData if needed, use in other nodes (e.g. effects)
  };

  ~NewDriver() override {}; // e.g. to free allocated memory
};


#endif