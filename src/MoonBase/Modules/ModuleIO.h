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

enum IO_PinUsage {
  pin_Unused,  // 0
  pin_LED_01,
  pin_LED_02,
  pin_LED_03,
  pin_LED_04,
  pin_LED_05,
  pin_LED_06,
  pin_LED_07,
  pin_LED_08,
  pin_LED_09,
  pin_LED_10,
  pin_LED_11,
  pin_LED_12,
  pin_LED_13,
  pin_LED_14,
  pin_LED_15,
  pin_LED_16,
  pin_LED_17,
  pin_LED_18,
  pin_LED_19,  // LED pins
  pin_LED_20,  // LED pins
  pin_LED_CW,
  pin_LED_WW,
  pin_LED_B,
  pin_LED_G,
  pin_LED_R,
  pin_I2S_SD,
  pin_I2S_WS,
  pin_I2S_SCK,
  pin_I2S_MCLK,
  pin_I2C_SDA,
  pin_I2C_SCL,
  pin_Button_01,
  pin_Button_02,
  pin_Button_03,
  pin_Voltage,
  pin_Current,
  pin_Infrared,
  pin_Relais,
  pin_Relais_Brightness,
  pin_DMX,
  pin_OnBoardLed,
  pin_OnBoardKey,
  pin_Battery,
  pin_Temperature,
  pin_Exposed,
  pin_SDIO_PIN_CMD,
  pin_SDIO_PIN_CLK,
  pin_SDIO_PIN_D0,
  pin_SDIO_PIN_D2,
  pin_SDIO_PIN_D3,
  pin_SDIO_PIN_D1,
  pin_Serial_TX,
  pin_Serial_RX,
  pin_Reserved,
  pin_Ethernet,
  pin_count
};

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
  board_Cube202010,
  board_count
};

class ModuleIO : public Module {
 public:
  ModuleIO(PsychicHttpServer* server, ESP32SvelteKit* sveltekit) : Module("inputoutput", server, sveltekit) {
    EXT_LOGV(MB_TAG, "constructor");

    // #if CONFIG_IDF_TARGET_ESP32
    //     pinMode(19, OUTPUT); digitalWrite(19, HIGH); // for serg shield boards: to be done: move to new pin manager module, switch off for S3!!!! tbd: add pin manager
    // #endif
  }

  void setupDefinition(JsonArray root) override {
    EXT_LOGV(MB_TAG, "");
    JsonObject control;  // state.data has one or more controls
    JsonArray details;   // if a control is an array, this is the details of the array
    JsonArray values;    // if a control is a select, this is the values of the select

    control = addControl(root, "boardPreset", "select");
    control["default"] = 0;
    values = control["values"].to<JsonArray>();
    values.add(BUILD_TARGET);  // 0 none
    values.add("QuinLED Dig Uno v3");
    values.add("QuinLED Dig Quad v3");
    values.add("QuinLED Dig Octa v2");
    values.add("QuinLED Penta");
    values.add("QuinLED Penta Plus");
    values.add("Serg Universal Shield v5");
    values.add("Serg Mini Shield");
    values.add("Mathieu SE16 v1");
    values.add("Wladi D0");
    values.add("Wladi P4 Nano");
    values.add("Yves V48");
    values.add("Troy P4 Nano");
    values.add("Atom S3R");
    values.add("Luxceo Mood1 Xiao Mod");
    values.add("Cube202010");

    control = addControl(root, "modded", "checkbox");
    control["default"] = false;

    control = addControl(root, "maxPower", "number", 0, 500, false, "Watt");
    control["default"] = 10;

    control = addControl(root, "pins", "rows");
    control["filter"] = "!Unused";
    details = control["n"].to<JsonArray>();
    {
      control = addControl(details, "GPIO", "number", 0, SOC_GPIO_PIN_COUNT - 1, true);  // ro

      control = addControl(details, "usage", "select");
      control["default"] = 0;
      values = control["values"].to<JsonArray>();
      values.add("Unused");  // 0
      values.add("LED 01");
      values.add("LED 02");
      values.add("LED 03");
      values.add("LED 04");
      values.add("LED 05");
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
      values.add("LED 19");
      values.add("LED 20");
      values.add("LED CW");
      values.add("LED WW");
      values.add("LED R");
      values.add("LED G");
      values.add("LED B");
      values.add("I2S SD");
      values.add("I2S WS");
      values.add("I2S SCK");
      values.add("I2S MCLK");
      values.add("I2C SDA");
      values.add("I2C SCL");
      values.add("Button 01");
      values.add("Button 02");
      values.add("Button 03");
      values.add("Voltage");
      values.add("Current");
      values.add("IR");
      values.add("Relais");
      values.add("Relais Brightness");
      values.add("DMX");
      values.add("On Board LED");
      values.add("On Board Key");
      values.add("Battery");
      values.add("Temperature");
      values.add("Exposed");
      values.add("SDIO_PIN_CMD");
      values.add("SDIO_PIN_CLK");
      values.add("SDIO_PIN_D0");
      values.add("SDIO_PIN_D2");
      values.add("SDIO_PIN_D3");
      values.add("SDIO_PIN_D1");
      values.add("Serial TX");
      values.add("Serial RX");
      values.add("Reserved");
      values.add("Ethernet");

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
      pin["usage"] = 0;

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
      for (int i = 0; i < sizeof(ledPins); i++) pins[ledPins[i]]["usage"] = pin_LED_01 + i;
      pins[0]["usage"] = pin_Button_01;
      pins[45]["usage"] = pin_Button_02;
      pins[46]["usage"] = pin_Button_03;
      pins[8]["usage"] = pin_Voltage;
      pins[9]["usage"] = pin_Current;
      pins[5]["usage"] = pin_Infrared;
    } else if (boardID == board_QuinLEDDigUnoV3) {
      object["maxPower"] = 75;
      pins[0]["usage"] = pin_Button_01;
      pins[1]["usage"] = pin_LED_01;
      pins[3]["usage"] = pin_LED_02;
      pins[2]["usage"] = pin_I2S_SD;
      pins[12]["usage"] = pin_I2S_WS;
      pins[13]["usage"] = pin_Temperature;
      pins[15]["usage"] = pin_I2S_SCK;
      pins[16]["usage"] = pin_LED_03;
      pins[32]["usage"] = pin_Exposed;
    } else if (boardID == board_QuinLEDDigQuadV3) {
      object["maxPower"] = 150;
      uint8_t ledPins[4] = {16, 3, 1, 4};  // LED_PINS
      for (int i = 0; i < sizeof(ledPins); i++) pins[ledPins[i]]["usage"] = pin_LED_01 + i;
      pins[0]["usage"] = pin_Button_01;
      pins[2]["usage"] = pin_I2S_SD;
      pins[12]["usage"] = pin_I2S_WS;
      pins[13]["usage"] = pin_Temperature;
      pins[15]["usage"] = pin_I2S_SCK;
      pins[32]["usage"] = pin_Exposed;
    } else if (boardID == board_QuinLEDDigOctoV2) {
      uint8_t ledPins[8] = {0, 1, 2, 3, 4, 5, 12, 13};  // LED_PINS
      for (int i = 0; i < sizeof(ledPins); i++) pins[ledPins[i]]["usage"] = pin_LED_01 + i;
    } else if (boardID == board_QuinLEDPenta) {
      uint8_t ledPins[5] = {14, 13, 12, 4, 2};  // LED_PINS
      for (int i = 0; i < sizeof(ledPins); i++) pins[ledPins[i]]["usage"] = pin_LED_01 + i;
      pins[34]["usage"] = pin_Button_01;
      pins[35]["usage"] = pin_Button_02;
      pins[39]["usage"] = pin_Button_03;
      pins[1]["usage"] = pin_I2C_SDA;
      pins[3]["usage"] = pin_I2C_SCL;
    } else if (boardID == board_QuinLEDPentaPlus) {
      pins[33]["usage"] = pin_LED_CW;
      pins[32]["usage"] = pin_LED_WW;
      pins[2]["usage"] = pin_LED_R;
      pins[4]["usage"] = pin_LED_G;
      pins[12]["usage"] = pin_LED_B;
      pins[36]["usage"] = pin_Button_01;
      pins[39]["usage"] = pin_Button_02;
      pins[34]["usage"] = pin_Button_03;
      pins[15]["usage"] = pin_I2C_SDA;
      pins[16]["usage"] = pin_I2C_SCL;
      pins[13]["usage"] = pin_Relais_Brightness;
      pins[05]["usage"] = pin_LED_01;
    } else if (boardID == board_SergMiniShield) {
      object["maxPower"] = 50;  // 10A Fuse ...
      pins[1]["usage"] = pin_LED_01;
      pins[3]["usage"] = pin_LED_02;
    } else if (boardID == board_SergUniShieldV5) {
      object["maxPower"] = 50;  // 10A Fuse ...
      pins[1]["usage"] = pin_LED_01;
      pins[3]["usage"] = pin_LED_02;
      pins[19]["usage"] = pin_Relais_Brightness;
    } else if (boardID == board_WladiD0) {
      pins[3]["usage"] = pin_Voltage;
    } else if (boardID == board_WladiP4Nano) {                                           // https://shop.myhome-control.de/ABC-WLED-ESP32-P4-Shield/HW10027
      object["maxPower"] = 10;                                                           // USB compliant
      uint8_t ledPins[16] = {21, 20, 25, 5, 7, 23, 8, 27, 3, 22, 24, 4, 46, 47, 2, 48};  // LED_PINS
      for (int i = 0; i < sizeof(ledPins); i++) pins[ledPins[i]]["usage"] = pin_LED_01 + i;
    } else if (boardID == board_YvesV48) {
      pins[3]["usage"] = pin_LED_01;
    } else if (boardID == board_TroyP4Nano) {
      object["maxPower"] = 10;                                                            // USB compliant
      uint8_t ledPins[16] = {2, 3, 4, 5, 6, 20, 21, 22, 23, 26, 27, 32, 33, 36, 47, 48};  // LED_PINS
      for (int i = 0; i < sizeof(ledPins); i++) pins[ledPins[i]]["usage"] = pin_LED_01 + i;
      pins[7]["usage"] = pin_I2C_SDA;
      pins[8]["usage"] = pin_I2C_SCL;
      pins[9]["usage"] = pin_Reserved;  // I2S Sound Output Pin
      pins[10]["usage"] = pin_I2S_WS;
      pins[11]["usage"] = pin_I2S_SD;
      pins[12]["usage"] = pin_I2S_SCK;
      pins[13]["usage"] = pin_I2S_MCLK;
      pins[14]["usage"] = pin_SDIO_PIN_D0;   // ESP-Hosted WiFi pins
      pins[15]["usage"] = pin_SDIO_PIN_D1;   // ESP-Hosted WiFi pins
      pins[16]["usage"] = pin_SDIO_PIN_D2;   // ESP-Hosted WiFi pins
      pins[17]["usage"] = pin_SDIO_PIN_D3;   // ESP-Hosted WiFi pins
      pins[18]["usage"] = pin_SDIO_PIN_CLK;  // ESP-Hosted WiFi pins
      pins[19]["usage"] = pin_SDIO_PIN_CMD;  // ESP-Hosted WiFi pins
      pins[24]["usage"] = pin_Reserved;      // USB Pins
      pins[25]["usage"] = pin_Reserved;      // USB Pins
      uint8_t ethernetPins[6] = {28, 29, 30, 31, 34, 35};
      for (int i = 0; i < sizeof(ethernetPins); i++) pins[ethernetPins[i]]["usage"] = pin_Ethernet;  // Ethernet Pins
      pins[37]["usage"] = pin_Serial_TX;
      pins[38]["usage"] = pin_Serial_RX;
      // 24-25 is is USB, but so is 26-27 but they're exposed on the header and work OK for pin outout.
      // 6 is C5 wakeup - but works fine for pin outout.
      // 45 is SD power but it's NC without hacking the board.
      // 53 is for PA enable but it's exposed on header and works for WLED pin output. Best to not use it but left available.
      // 54 is "C4 EN pin" so I guess we shouldn't fuck with that.
    } else if (boardID == board_AtomS3) {
      uint8_t ledPins[4] = {5, 6, 7, 8};  // LED_PINS
      for (int i = 0; i < sizeof(ledPins); i++) pins[ledPins[i]]["usage"] = pin_LED_01 + i;
    } else if (boardID == board_Cube202010) {
      object["maxPower"] = 50;
      uint8_t ledPins[10] = {22, 21, 14, 18, 5, 4, 2, 15, 13, 12};  // LED_PINS
                                                                    // char pins[80] = "2,3,4,16,17,18,19,21,22,23,25,26,27,32,33";  //(D0), more pins possible. to do: complete list.
      for (int i = 0; i < sizeof(ledPins); i++) pins[ledPins[i]]["usage"] = pin_LED_01 + i;
    } else {                    // default
      object["maxPower"] = 10;  // USB compliant
      pins[16]["usage"] = pin_LED_01;
    }
    // String xxx;
    // serializeJson(_state.data, xxx);
    // EXT_LOGD(MB_TAG, "%s", xxx.c_str());
    // EXT_LOGD(MB_TAG, "%s", xxx.c_str());

    EXT_LOGD(ML_TAG, "boardID %d", boardID);
    // serializeJson(object, Serial);Serial.println();

    update(object, ModuleState::update, _moduleName + "server");
  }

  uint8_t newBoardID = UINT8_MAX;

  void onUpdate(const UpdatedItem& updatedItem) override {
    // run in sveltekit task
    //  EXT_LOGD(MB_TAG, "%s[%d]%s[%d].%s = %s -> %s", updatedItem.parent[0].c_str(), updatedItem.index[0], updatedItem.parent[1].c_str(), updatedItem.index[1], updatedItem.name.c_str(), updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
    //  if (updatedItem.oldValue == "") return;
    if (updatedItem.name == "boardPreset" && !updateOriginId.contains("server")) {  // not done by this module: done by UI
      newBoardID = updatedItem.value; //run in sveltekit task
    } else if (updatedItem.name == "modded" && !updateOriginId.contains("server")) {  // not done by this module: done by UI
      if (updatedItem.value == false) {
        newBoardID = _state.data["boardPreset"]; //run in sveltekit task
      }
    } else if (updatedItem.name == "usage" && !updateOriginId.contains("server")) {  // not done by this module: done by UI
      // set pins to default if modded is turned off
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

  void loop() override {
    //run in sveltekit task
    Module::loop();
    if (newBoardID != UINT8_MAX) {
      setBoardPresetDefaults(newBoardID);  // run from sveltekit task
      newBoardID = UINT8_MAX;
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