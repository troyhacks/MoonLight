/**
    @title     MoonBase
    @file      ModuleIO.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/moonbase/inputoutput/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact us for more information.
**/

#ifndef ModuleIO_h
  #define ModuleIO_h

  #if FT_MOONBASE == 1

    #include "MoonBase/Module.h"

class ModuleIO : public Module {
 public:
  ModuleIO(PsychicHttpServer* server, ESP32SvelteKit* sveltekit) : Module("inputoutput", server, sveltekit) {
    EXT_LOGV(MB_TAG, "constructor");

    // #if CONFIG_IDF_TARGET_ESP32
    //     pinMode(19, OUTPUT); digitalWrite(19, HIGH); // for serg shield boards: to be done: move to new pin manager module, switch off for S3!!!! tbd: add pin manager
    // #endif
  }

  enum IO_Boards {
    board_none,  //
    board_QuinLEDDigUnoV3,
    board_QuinLEDDigQuadV3,
    board_QuinLEDDigOctoV2,
    board_QuinLEDPenta,
    board_QuinLEDPentaPlus,
    board_SergUniShieldV5,
    board_SergMiniShield,
    board_SE16V1,
    board_WladiD0,
    board_WladiP4Nano,
    board_YvesV48,
    board_TroyP4Nano,
    board_AtomS3,
    board_LuxceoMood1XiaoMod,
    board_count
  };

  enum IO_Pins {
    pin_none,
    pin_LED01,
    pin_LED02,
    pin_LED03,
    pin_LED04,
    pin_LED05,
    pin_LED06,
    pin_LED07,
    pin_LED08,
    pin_LED09,
    pin_LED10,
    pin_LED11,
    pin_LED12,
    pin_LED13,
    pin_LED14,
    pin_LED15,
    pin_LED16,
    pin_LED17,
    pin_LED18,
    pin_LED19,  // LED pins
    pin_LED20,  // LED pins
    pin_LEDCW,
    pin_LEDWW,
    pin_LEDB,
    pin_LEDG,
    pin_LEDR,
    pin_Button01,
    pin_Button02,
    pin_Button03,
    pin_Voltage,
    pin_Current,
    pin_IR,
    pin_Relais,
    pin_RelaisBrightness,
    pin_DMX,
    pin_MicSD,
    pin_MicWS,
    pin_MicSCK,
    pin_MicMCLK,
    pin_OnBoardLed,
    pin_OnBoardKey,
    pin_Battery,
    pin_Temperature,
    pin_Exposed,
    pin_I2CSDA,
    pin_I2CSCL,
    pin_count
  };

  void setupDefinition(JsonArray root) override {
    EXT_LOGV(MB_TAG, "");
    JsonObject control;  // state.data has one or more controls
    JsonArray details;   // if a control is an array, this is the details of the array
    JsonArray values;    // if a control is a select, this is the values of the select

    control = addControl(root, "boardPreset", "select");
    control["default"] = 0;
    values = control["values"].to<JsonArray>();
    values.add(BUILD_TARGET);
    values.add("QuinLED Dig Uno V3");
    values.add("QuinLED Dig Quad V3");
    values.add("QuinLED Dig Octa V2");
    values.add("QuinLED Penta");
    values.add("QuinLED Penta Plus");
    values.add("Serg Universal Shield V5");
    values.add("Serg Mini Shield");
    values.add("Mathieu SE16 V1");
    values.add("Wladi D0");
    values.add("Wladi P4 Nano");
    values.add("Yves V48");
    values.add("Troy P4 Nano");
    values.add("Atom S3R");
    values.add("Luxceo Mood1 Xiao Mod");

    control = addControl(root, "modded", "checkbox");
    control["default"] = false;

    control = addControl(root, "maxPower", "number", 0, 500, false, "Watt");
    control["default"] = 10;
    // control = addControl(root, "search", "text", 0, 64);
    // control["default"] = "!none";

    control = addControl(root, "pins", "array");
    control["search"] = "!none";
    details = control["n"].to<JsonArray>();
    {
      control = addControl(details, "GPIO", "number", 0, SOC_GPIO_PIN_COUNT - 1, true);  // ro

      control = addControl(details, "pinFunction", "select");
      control["default"] = 0;
      values = control["values"].to<JsonArray>();
      values.add("None");    // 0
      values.add("LED 01");  // 1
      values.add("LED 02");  // 2
      values.add("LED 03");  // 3
      values.add("LED 04");  // 4
      values.add("LED 05");  // 5
      values.add("LED 06");
      values.add("LED 07");
      values.add("LED 08");
      values.add("LED 09");
      values.add("LED 10");
      values.add("LED 11");
      values.add("LED 12");
      values.add("LED 13");
      values.add("LED 14");
      values.add("LED 15");
      values.add("LED 16");
      values.add("LED 17");
      values.add("LED 18");
      values.add("LED 19");  // 19
      values.add("LED 20");  // 20
      values.add("LED CW");
      values.add("LED WW");
      values.add("LED R");
      values.add("LED G");
      values.add("LED B");
      values.add("Button 01");  // 26
      values.add("Button 02");
      values.add("Button 03");
      values.add("Voltage");  // 29
      values.add("Current");  // 30
      values.add("IR");       // 31
      values.add("Relais");
      values.add("Relais Brightness");
      values.add("DMX");
      values.add("Mic SD");
      values.add("Mic WS");
      values.add("Mic SCK");
      values.add("Mic MCLK");
      values.add("On Board LED");
      values.add("On Board Key");
      values.add("Battery");
      values.add("Temperature");
      values.add("Exposed");
      values.add("I2S SDA");
      values.add("I2S SCL");

      control = addControl(details, "summary", "text", 0, 32, true);  // ro
      // control = details.add<JsonObject>(); control["name"] = "Valid"; control["type"] = "checkbox"; control["ro"] = true;
      // control = details.add<JsonObject>(); control["name"] = "Output"; control["type"] = "checkbox"; control["ro"] = true;
      // control = details.add<JsonObject>(); control["name"] = "RTC"; control["type"] = "checkbox"; control["ro"] = true;

      control = addControl(details, "Level", "text", 0, 32, true);     // ro
      control = addControl(details, "DriveCap", "text", 0, 32, true);  // ro
    }
  }

  void setBoardPresetDefaults(uint8_t boardID) {
    JsonDocument doc;
    JsonObject object = doc.to<JsonObject>();
    object["modded"] = false;

    JsonArray pins = object["pins"].to<JsonArray>();

    // reset all pins
    for (int gpio_num = 0; gpio_num < SOC_GPIO_PIN_COUNT; gpio_num++) {
      JsonObject pin = pins.add<JsonObject>();
      pin["GPIO"] = gpio_num;
      pin["pinFunction"] = 0;

      // Check if GPIO is valid
      bool is_valid = GPIO_IS_VALID_GPIO(gpio_num);
      bool is_output_valid = GPIO_IS_VALID_OUTPUT_GPIO(gpio_num);
      bool is_rtc_gpio = rtc_gpio_is_valid_gpio((gpio_num_t)gpio_num);

      // Get current level (works for both input and output pins)
      int level = -1;
      if (is_valid) {
        level = gpio_get_level((gpio_num_t)gpio_num);
      }

      // For RTC GPIOs, can also use RTC-specific read
      int rtc_level = -1;
    #ifndef CONFIG_IDF_TARGET_ESP32C3
      if (is_rtc_gpio) {
        rtc_level = rtc_gpio_get_level((gpio_num_t)gpio_num);  // to do find c3 alternative
      }
    #endif

      // Get drive capability (if output capable)
      gpio_drive_cap_t drive_cap = GPIO_DRIVE_CAP_DEFAULT;
      esp_err_t drive_result = ESP_FAIL;
      if (is_output_valid) {
        drive_result = gpio_get_drive_capability((gpio_num_t)gpio_num, &drive_cap);
      }

      Char<32> summary;
      summary.format("%s%s%s", is_valid ? "✅" : "", is_output_valid ? " 💡" : "", is_rtc_gpio ? " ⏰" : "");

      pin["GPIO"] = gpio_num;
      pin["summary"] = summary.c_str();
      // pin["Valid"] = is_valid;
      // pin["Output"] = is_output_valid;
      // pin["RTC"] = is_rtc_gpio;
      pin["Level"] = (level >= 0) ? (level ? "HIGH" : "LOW") : "N/A";
      pin["DriveCap"] = (drive_result == ESP_OK) ? drive_cap_to_string(drive_cap) : "N/A";
    }

    if (boardID == board_SE16V1) {
      object["maxPower"] = 500;
      uint8_t ledPins[16] = {47, 48, 21, 38, 14, 39, 13, 40, 12, 41, 11, 42, 10, 2, 3, 1};  // LED_PINS
      for (int i = 0; i < sizeof(ledPins); i++) pins[ledPins[i]]["pinFunction"] = pin_LED01 + i;
      pins[0]["pinFunction"] = pin_Button01;
      pins[45]["pinFunction"] = pin_Button02;
      pins[46]["pinFunction"] = pin_Button03;
      pins[8]["pinFunction"] = pin_Voltage;
      pins[9]["pinFunction"] = pin_Current;
      pins[5]["pinFunction"] = pin_IR;
    } else if (boardID == board_QuinLEDDigUnoV3) {
      object["maxPower"] = 75;
      pins[0]["pinFunction"] = pin_Button01;
      pins[1]["pinFunction"] = pin_LED02;
      pins[3]["pinFunction"] = pin_LED02;
      pins[2]["pinFunction"] = pin_MicSD;
      pins[12]["pinFunction"] = pin_MicWS;
      pins[13]["pinFunction"] = pin_Temperature;
      pins[15]["pinFunction"] = pin_MicSCK;
      pins[16]["pinFunction"] = pin_LED01;
      pins[32]["pinFunction"] = pin_Exposed;
    } else if (boardID == board_QuinLEDDigQuadV3) {
      object["maxPower"] = 150;
      uint8_t ledPins[4] = {16, 3, 1, 4};  // LED_PINS
      for (int i = 0; i < sizeof(ledPins); i++) pins[ledPins[i]]["pinFunction"] = pin_LED01 + i;
      pins[0]["pinFunction"] = pin_Button01;
      pins[2]["pinFunction"] = pin_MicSD;
      pins[12]["pinFunction"] = pin_MicWS;
      pins[13]["pinFunction"] = pin_Temperature;
      pins[15]["pinFunction"] = pin_MicSCK;
      pins[32]["pinFunction"] = pin_Exposed;
    } else if (boardID == board_QuinLEDDigOctoV2) {
      uint8_t ledPins[8] = {0, 1, 2, 3, 4, 5, 12, 13};  // LED_PINS
      for (int i = 0; i < sizeof(ledPins); i++) pins[ledPins[i]]["pinFunction"] = pin_LED01 + i;
    } else if (boardID == board_QuinLEDPenta) {
      uint8_t ledPins[5] = {14, 13, 12, 4, 2};  // LED_PINS
      for (int i = 0; i < sizeof(ledPins); i++) pins[ledPins[i]]["pinFunction"] = pin_LED01 + i;
      pins[34]["pinFunction"] = pin_Button01;
      pins[35]["pinFunction"] = pin_Button02;
      pins[39]["pinFunction"] = pin_Button03;
      pins[1]["pinFunction"] = pin_I2CSDA;
      pins[3]["pinFunction"] = pin_I2CSCL;
    } else if (boardID == board_QuinLEDPentaPlus) {
      pins[33]["pinFunction"] = pin_LEDCW;
      pins[32]["pinFunction"] = pin_LEDWW;
      pins[2]["pinFunction"] = pin_LEDR;
      pins[4]["pinFunction"] = pin_LEDG;
      pins[12]["pinFunction"] = pin_LEDB;
      pins[36]["pinFunction"] = pin_Button01;
      pins[39]["pinFunction"] = pin_Button02;
      pins[34]["pinFunction"] = pin_Button03;
      pins[15]["pinFunction"] = pin_I2CSDA;
      pins[16]["pinFunction"] = pin_I2CSCL;
      pins[13]["pinFunction"] = pin_RelaisBrightness;
      pins[05]["pinFunction"] = pin_LED01;
    } else if (boardID == board_SergMiniShield) {
      object["maxPower"] = 50;  // 10A Fuse ...
      pins[1]["pinFunction"] = pin_LED01;
      pins[3]["pinFunction"] = pin_LED02;
    } else if (boardID == board_SergUniShieldV5) {
      object["maxPower"] = 50;  // 10A Fuse ...
      pins[1]["pinFunction"] = pin_LED01;
      pins[3]["pinFunction"] = pin_LED02;
      pins[19]["pinFunction"] = pin_RelaisBrightness;
    } else if (boardID == board_WladiD0) {
      pins[3]["pinFunction"] = pin_Voltage;
    } else if (boardID == board_WladiP4Nano) {
      object["maxPower"] = 10;                                                            // USB compliant
      uint8_t ledPins[16] = {2, 3, 4, 5, 6, 20, 21, 22, 23, 26, 27, 32, 33, 36, 47, 48};  // LED_PINS
      for (int i = 0; i < sizeof(ledPins); i++) pins[ledPins[i]]["pinFunction"] = pin_LED01 + i;
    } else if (boardID == board_YvesV48) {
      pins[3]["pinFunction"] = pin_LED01;
    } else if (boardID == board_TroyP4Nano) {
      object["maxPower"] = 10;                                                            // USB compliant
      uint8_t ledPins[16] = {2, 3, 4, 5, 6, 20, 21, 22, 23, 26, 27, 32, 33, 36, 47, 48};  // LED_PINS
      for (int i = 0; i < sizeof(ledPins); i++) pins[ledPins[i]]["pinFunction"] = pin_LED01 + i;
    } else if (boardID == board_AtomS3) {
      uint8_t ledPins[4] = {5, 6, 7, 8};  // LED_PINS
      for (int i = 0; i < sizeof(ledPins); i++) pins[ledPins[i]]["pinFunction"] = pin_LED01 + i;
    } else {                    // default
      object["maxPower"] = 10;  // USB compliant
      // pins[2]["pinFunction"] = pin_LED00;
      pins[16]["pinFunction"] = pin_LED01;
    }
    // String xxx;
    // serializeJson(_state.data, xxx);
    // EXT_LOGD(MB_TAG, "%s", xxx.c_str());
    // serializeJson(object, xxx);
    // EXT_LOGD(MB_TAG, "%s", xxx.c_str());

    update(object, ModuleState::update, _moduleName);
  }

  void onUpdate(UpdatedItem& updatedItem) override {
    // EXT_LOGD(MB_TAG, "%s[%d]%s[%d].%s = %s -> %s", updatedItem.parent[0].c_str(), updatedItem.index[0], updatedItem.parent[1].c_str(), updatedItem.index[1], updatedItem.name.c_str(), updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
    if (updatedItem.name == "boardPreset") {
      setBoardPresetDefaults(updatedItem.value);
    } else if (updatedItem.name == "modded" && updateOriginId != _moduleName) {  // not done by this module: done by UI
      if (updatedItem.value == false) setBoardPresetDefaults(_state.data["boardPreset"]);
      // set pins to default
    } else if (updatedItem.name == "pinFunction" && updateOriginId != _moduleName) {  // not done by this module: done by UI
      JsonDocument doc;
      JsonObject object = doc.to<JsonObject>();
      object["modded"] = true;
      update(object, ModuleState::update, _moduleName);
    }
  }

  // void updateHandler(const String& originId) override { EXT_LOGD(MB_TAG, "originId: %s", originId.c_str()); }

  // Function to convert drive capability to string
  const char* drive_cap_to_string(gpio_drive_cap_t cap) {
    switch (cap) {
    case GPIO_DRIVE_CAP_0:
      return "WEAK";
    case GPIO_DRIVE_CAP_1:
      return "STRONGER";
    case GPIO_DRIVE_CAP_2:
      return "MEDIUM";
    case GPIO_DRIVE_CAP_3:
      return "STRONGEST";
    default:
      return "UNKNOWN";
    }
  }
};

  #endif
#endif

// format:
// {
//   "pins": [
//     {
//       "GPIO": 0,
//       "Valid": true,
//       "Output": true,
//       "RTC": true,
//       "Level": "HIGH",
//       "DriveCap": "MEDIUM",
//     },
//     {
//       "GPIO": 1,
//       "Valid": true,
//       "Output": true,
//       "RTC": false,
//       "Level": "HIGH",
//       "DriveCap": "MEDIUM",
//     },