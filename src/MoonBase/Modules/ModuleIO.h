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
  pin_LED,
  pin_LED_CW,
  pin_LED_WW,
  pin_LED_R,
  pin_LED_G,
  pin_LED_B,
  pin_I2S_SD,
  pin_I2S_WS,
  pin_I2S_SCK,
  pin_I2S_MCLK,
  pin_I2C_SDA,
  pin_I2C_SCL,
  pin_ButtonPush,
  pin_ButtonToggle,
  pin_Button_LightsOn,
  pin_Relay,
  pin_Relay_LightsOn,
  pin_Voltage,
  pin_Current,
  pin_Infrared,
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
  pin_Ethernet,
  pin_SPI_SCK,
  pin_SPI_MISO,
  pin_SPI_MOSI,
  pin_PHY_CS,
  pin_PHY_IRQ,
  pin_Reserved,
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
  ESP32SvelteKit* _sveltekit;

  ModuleIO(PsychicHttpServer* server, ESP32SvelteKit* sveltekit) : Module("inputoutput", server, sveltekit) {
    EXT_LOGV(MB_TAG, "constructor");

    // #if CONFIG_IDF_TARGET_ESP32
    //     pinMode(19, OUTPUT); digitalWrite(19, HIGH); // for serg shield boards: to be done: move to new pin manager module, switch off for S3!!!! tbd: add pin manager
    // #endif

    _sveltekit = sveltekit;

    addUpdateHandler([&](const String& originId) { readPins(); }, false);
  }

  void setupDefinition(const JsonArray& controls) override {
    EXT_LOGV(MB_TAG, "");
    JsonObject control;  // state.data has one or more controls
    JsonArray rows;      // if a control is an array, this is the rows of the array

    control = addControl(controls, "boardPreset", "select");
    control["default"] = 0;
    addControlValue(control, BUILD_TARGET);  // 0 none
    addControlValue(control, "QuinLED Dig Uno v3");
    addControlValue(control, "QuinLED Dig Quad v3");
    addControlValue(control, "QuinLED Dig Octa v2");
    addControlValue(control, "QuinLED Penta");
    addControlValue(control, "QuinLED Penta Plus");
    addControlValue(control, "Serg Universal Shield v5");
    addControlValue(control, "Serg Mini Shield");
    addControlValue(control, "Mathieu SE16 v1");
    addControlValue(control, "Wladi D0");
    addControlValue(control, "Wladi P4 Nano");
    addControlValue(control, "Yves V48");
    addControlValue(control, "Troy P4 Nano");
    addControlValue(control, "Atom S3R");
    addControlValue(control, "Luxceo Mood1 Xiao Mod");
    addControlValue(control, "Cube202010");

    control = addControl(controls, "modded", "checkbox");
    control["default"] = false;

    control = addControl(controls, "maxPower", "number", 0, 500, false, "Watt");
    control["default"] = 10;

    control = addControl(controls, "jumper1", "checkbox");
    control["default"] = false;

    control = addControl(controls, "pins", "rows");
    control["filter"] = "!Unused";
    control["crud"] = "ru";

    rows = control["n"].to<JsonArray>();
    {
      control = addControl(rows, "GPIO", "number", 0, SOC_GPIO_PIN_COUNT - 1, true);  // ro

      control = addControl(rows, "usage", "select");
      control["default"] = 0;
      addControlValue(control, "Unused");  // 0
      addControlValue(control, "LED");
      addControlValue(control, "LED CW");
      addControlValue(control, "LED WW");
      addControlValue(control, "LED R");
      addControlValue(control, "LED G");
      addControlValue(control, "LED B");
      addControlValue(control, "I2S SD");
      addControlValue(control, "I2S WS");
      addControlValue(control, "I2S SCK");
      addControlValue(control, "I2S MCLK");
      addControlValue(control, "I2C SDA");
      addControlValue(control, "I2C SCL");
      addControlValue(control, "Button Push");
      addControlValue(control, "Button Toggle");
      addControlValue(control, "Button LightOn");
      addControlValue(control, "Relay");
      addControlValue(control, "Relay LightOn");
      addControlValue(control, "Voltage");
      addControlValue(control, "Current");
      addControlValue(control, "Infrared");
      addControlValue(control, "DMX in");
      addControlValue(control, "Onboard LED");
      addControlValue(control, "Onboard Key");
      addControlValue(control, "Battery");
      addControlValue(control, "Temperature");
      addControlValue(control, "Exposed");
      addControlValue(control, "SDIO CMD");
      addControlValue(control, "SDIO CLK");
      addControlValue(control, "SDIO D0");
      addControlValue(control, "SDIO D2");
      addControlValue(control, "SDIO D3");
      addControlValue(control, "SDIO D1");
      addControlValue(control, "Serial TX");
      addControlValue(control, "Serial RX");
      addControlValue(control, "Ethernet");
      addControlValue(control, "SPI SCK");
      addControlValue(control, "SPI MISO");
      addControlValue(control, "SPI MOSI");
      addControlValue(control, "PHY CS");
      addControlValue(control, "PHY IRQ");
      addControlValue(control, "Reserved");

      control = addControl(rows, "index", "number", 1, 32);  // max 32 of one type, e.g 32 led pins
      control["default"] = UINT8_MAX;

      control = addControl(rows, "summary", "text", 0, 32, true);  // ro
      control["show"] = true;                                      // only the first 3 are shown in multirows, allow here the 4th to be shown as well
      // addControl(rows, "Valid", "checkbox", false, true, true);
      // addControl(rows, "Output", "checkbox", false, true, true);
      // addControl(rows, "RTC", "checkbox", false, true, true);

      addControl(rows, "Level", "text", 0, 32, true);     // ro
      addControl(rows, "DriveCap", "text", 0, 32, true);  // ro
    }
  }

  class PinAssigner {
   public:
    JsonArray pins;

    PinAssigner(const JsonArray& pins) { this->pins = pins; }

    void assignPin(uint8_t pinNr, uint8_t usage) {
      if (lastUsage != usage) {
        argCounter = 1;
        lastUsage = usage;
      }
      pins[pinNr]["usage"] = usage;
      pins[pinNr]["index"] = argCounter++;
    }

   private:
    uint8_t argCounter = 0;
    uint8_t lastUsage = UINT8_MAX;
  };

  void setBoardPresetDefaults(uint8_t boardID) {
    JsonDocument doc;
    JsonObject object = doc.to<JsonObject>();
    object["modded"] = false;

    JsonArray pins = object["pins"].to<JsonArray>();

    PinAssigner pinAssigner(pins);

    // reset all pins
    for (int gpio_num = 0; gpio_num < SOC_GPIO_PIN_COUNT; gpio_num++) {
      JsonObject pin = pins.add<JsonObject>();
      pin["GPIO"] = gpio_num;
      pin["usage"] = 0;
      pin["index"] = 0;

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
      for (int i = 0; i < sizeof(ledPins); i++) pinAssigner.assignPin(ledPins[i], pin_LED);
      pinAssigner.assignPin(0, pin_ButtonPush);
      pinAssigner.assignPin(45, pin_ButtonPush);
      pinAssigner.assignPin(46, pin_Button_LightsOn);
      pinAssigner.assignPin(8, pin_Voltage);
      pinAssigner.assignPin(9, pin_Current);

      if (_state.data["jumper1"]) {
        pinAssigner.assignPin(5, pin_Infrared);
      } else {  // default
        pinAssigner.assignPin(5, pin_SPI_MISO);
        pinAssigner.assignPin(6, pin_SPI_MOSI);
        pinAssigner.assignPin(7, pin_SPI_SCK);
        pinAssigner.assignPin(15, pin_PHY_CS);
        pinAssigner.assignPin(18, pin_PHY_IRQ);
      }

    } else if (boardID == board_QuinLEDDigUnoV3) {
      // Dig-Uno-V3
      // esp32-d0 (4MB)
      object["maxPower"] = 50;  // max 75, but 10A fuse
      pinAssigner.assignPin(16, pin_LED);
      pinAssigner.assignPin(3, pin_LED);
      pinAssigner.assignPin(0, pin_ButtonPush);
      pinAssigner.assignPin(15, pin_Relay);
      // pinAssigner.assignPin(2, pin_I2S_SD);
      // pinAssigner.assignPin(12, pin_I2S_WS);
      // pinAssigner.assignPin(13, pin_Temperature);
      // pinAssigner.assignPin(15, pin_I2S_SCK);
      // pinAssigner.assignPin(16, pin_LED_03);
      // pinAssigner.assignPin(32, pin_Exposed);
    } else if (boardID == board_QuinLEDDigQuadV3) {
      // Dig-Quad-V3
      // esp32-d0 (4MB)
      object["maxPower"] = 150;
      uint8_t ledPins[4] = {16, 3, 1, 4};  // LED_PINS
      for (int i = 0; i < sizeof(ledPins); i++) pinAssigner.assignPin(ledPins[i], pin_LED);
      pinAssigner.assignPin(0, pin_ButtonPush);

      pinAssigner.assignPin(15, pin_Relay);

      // pinAssigner.assignPin(2, pin_I2S_SD;
      // pinAssigner.assignPin(12, pin_I2S_WS;
      // pinAssigner.assignPin(13, pin_Temperature;
      // pinAssigner.assignPin(15, pin_I2S_SCK;
      // pinAssigner.assignPin(32, pin_Exposed;
    } else if (boardID == board_QuinLEDDigOctoV2) {
      uint8_t ledPins[8] = {0, 1, 2, 3, 4, 5, 12, 13};  // LED_PINS
      for (int i = 0; i < sizeof(ledPins); i++) pinAssigner.assignPin(ledPins[i], pin_LED);
    } else if (boardID == board_QuinLEDPenta) {
      uint8_t ledPins[5] = {14, 13, 12, 4, 2};  // LED_PINS
      for (int i = 0; i < sizeof(ledPins); i++) pinAssigner.assignPin(ledPins[i], pin_LED);
      pinAssigner.assignPin(34, pin_ButtonPush);
      pinAssigner.assignPin(35, pin_ButtonPush);
      pinAssigner.assignPin(39, pin_ButtonPush);
      pinAssigner.assignPin(1, pin_I2C_SDA);
      pinAssigner.assignPin(3, pin_I2C_SCL);
    } else if (boardID == board_QuinLEDPentaPlus) {
      pinAssigner.assignPin(33, pin_LED_CW);
      pinAssigner.assignPin(32, pin_LED_WW);
      pinAssigner.assignPin(2, pin_LED_R);
      pinAssigner.assignPin(4, pin_LED_G);
      pinAssigner.assignPin(12, pin_LED_B);
      pinAssigner.assignPin(36, pin_ButtonPush);
      pinAssigner.assignPin(39, pin_ButtonPush);
      pinAssigner.assignPin(33, pin_ButtonPush);
      pinAssigner.assignPin(15, pin_I2C_SDA);
      pinAssigner.assignPin(16, pin_I2C_SCL);
      pinAssigner.assignPin(13, pin_Relay_LightsOn);
      pinAssigner.assignPin(5, pin_LED);
    } else if (boardID == board_SergMiniShield) {
      object["maxPower"] = 50;  // 10A Fuse ...
      pinAssigner.assignPin(1, pin_LED);
      pinAssigner.assignPin(3, pin_LED);
    } else if (boardID == board_SergUniShieldV5) {
      object["maxPower"] = 50;  // 10A Fuse ...
      pinAssigner.assignPin(1, pin_LED);
      pinAssigner.assignPin(3, pin_LED);
      pinAssigner.assignPin(19, pin_Relay_LightsOn);
    } else if (boardID == board_WladiD0) {
      pinAssigner.assignPin(3, pin_Voltage);
    } else if (boardID == board_WladiP4Nano) {                                           // https://shop.myhome-control.de/ABC-WLED-ESP32-P4-Shield/HW10027
      object["maxPower"] = 10;                                                           // USB compliant
      uint8_t ledPins[16] = {21, 20, 25, 5, 7, 23, 8, 27, 3, 22, 24, 4, 46, 47, 2, 48};  // LED_PINS
      for (int i = 0; i < sizeof(ledPins); i++) pinAssigner.assignPin(ledPins[i], pin_LED);
    } else if (boardID == board_YvesV48) {
      pinAssigner.assignPin(3, pin_LED);
    } else if (boardID == board_TroyP4Nano) {
      object["maxPower"] = 10;                                                            // USB compliant
      uint8_t ledPins[16] = {2, 3, 4, 5, 6, 20, 21, 22, 23, 26, 27, 32, 33, 36, 47, 48};  // LED_PINS
      for (int i = 0; i < sizeof(ledPins); i++) pinAssigner.assignPin(ledPins[i], pin_LED);
      pinAssigner.assignPin(7, pin_I2C_SDA);
      pinAssigner.assignPin(8, pin_I2C_SCL);
      pinAssigner.assignPin(9, pin_Reserved);  // I2S Sound Output Pin
      pinAssigner.assignPin(10, pin_I2S_WS);
      pinAssigner.assignPin(11, pin_I2S_SD);
      pinAssigner.assignPin(12, pin_I2S_SCK);
      pinAssigner.assignPin(13, pin_I2S_MCLK);
      pinAssigner.assignPin(14, pin_SDIO_PIN_D0);   // ESP-Hosted WiFi pins
      pinAssigner.assignPin(15, pin_SDIO_PIN_D1);   // ESP-Hosted WiFi pins
      pinAssigner.assignPin(16, pin_SDIO_PIN_D2);   // ESP-Hosted WiFi pins
      pinAssigner.assignPin(17, pin_SDIO_PIN_D3);   // ESP-Hosted WiFi pins
      pinAssigner.assignPin(18, pin_SDIO_PIN_CLK);  // ESP-Hosted WiFi pins
      pinAssigner.assignPin(19, pin_SDIO_PIN_CMD);  // ESP-Hosted WiFi pins
      pinAssigner.assignPin(24, pin_Reserved);      // USB Pins
      pinAssigner.assignPin(25, pin_Reserved);      // USB Pins
      uint8_t ethernetPins[6] = {28, 29, 30, 31, 34, 35};
      for (int i = 0; i < sizeof(ethernetPins); i++) pinAssigner.assignPin(ethernetPins[i], pin_Ethernet);  // Ethernet Pins
      pinAssigner.assignPin(37, pin_Serial_TX);
      pinAssigner.assignPin(38, pin_Serial_RX);
      // 24-25 is is USB, but so is 26-27 but they're exposed on the header and work OK for pin outout.
      // 6 is C5 wakeup - but works fine for pin outout.
      // 45 is SD power but it's NC without hacking the board.
      // 53 is for PA enable but it's exposed on header and works for WLED pin output. Best to not use it but left available.
      // 54 is "C4 EN pin" so I guess we shouldn't fuck with that.
    } else if (boardID == board_AtomS3) {
      uint8_t ledPins[4] = {5, 6, 7, 8};  // LED_PINS
      for (int i = 0; i < sizeof(ledPins); i++) pinAssigner.assignPin(ledPins[i], pin_LED);
    } else if (boardID == board_Cube202010) {
      object["maxPower"] = 50;
      uint8_t ledPins[10] = {22, 21, 14, 18, 5, 4, 2, 15, 13, 12};  // LED_PINS
                                                                    // char pins[80] = "2,3,4,16,17,18,19,21,22,23,25,26,27,32,33";  //(D0), more pins possible. to do: complete list.
      for (int i = 0; i < sizeof(ledPins); i++) pinAssigner.assignPin(ledPins[i], pin_LED);
    } else {                    // default
      object["maxPower"] = 10;  // USB compliant
      pinAssigner.assignPin(16, pin_LED);

      // trying to add more pins, but these pins not liked by esp32-d0-16MB ... 🚧
      // pinAssigner.assignPin(4, pin_LED_02;
      // pinAssigner.assignPin(5, pin_LED_03;
      // pinAssigner.assignPin(6, pin_LED_04;
      // pinAssigner.assignPin(7, pin_LED_05;
      // pinAssigner.assignPin(8, pin_LED_06;
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
    if (updatedItem.name == "boardPreset" && !_state.updateOriginId.contains("server")) {  // not done by this module: done by UI
      // if booting and modded is false or ! booting
      if ((updatedItem.oldValue == "" && _state.data["modded"] == false) || updatedItem.oldValue != "") {  // only update unmodded
        EXT_LOGD(MB_TAG, "%s %s[%d]%s[%d].%s = %s -> %s", _state.updateOriginId.c_str(), updatedItem.parent[0].c_str(), updatedItem.index[0], updatedItem.parent[1].c_str(), updatedItem.index[1], updatedItem.name.c_str(), updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
        newBoardID = updatedItem.value;  // run in sveltekit task
      }
    } else if (updatedItem.name == "modded" && !_state.updateOriginId.contains("server")) {  // not done by this module: done by UI
      if (updatedItem.value == false) {
        EXT_LOGD(MB_TAG, "%s[%d]%s[%d].%s = %s -> %s", updatedItem.parent[0].c_str(), updatedItem.index[0], updatedItem.parent[1].c_str(), updatedItem.index[1], updatedItem.name.c_str(), updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
        newBoardID = _state.data["boardPreset"];  // run in sveltekit task
      }
    } else if (updatedItem.name == "usage" && !_state.updateOriginId.contains("server")) {  // not done by this module: done by UI
      // EXT_LOGD(MB_TAG, "%s[%d]%s[%d].%s = %s -> %s", updatedItem.parent[0].c_str(), updatedItem.index[0], updatedItem.parent[1].c_str(), updatedItem.index[1], updatedItem.name.c_str(), updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
      // set pins to default if modded is turned off
      JsonDocument doc;
      JsonObject object = doc.to<JsonObject>();
      object["modded"] = true;
      update(object, ModuleState::update, _moduleName + "server");
    }
  }

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
    // run in sveltekit task
    Module::loop();

    if (newBoardID != UINT8_MAX) {
      setBoardPresetDefaults(newBoardID);  // run from sveltekit task
      newBoardID = UINT8_MAX;
    }
  }

  void readPins() {
  #if FT_ENABLED(FT_ETHERNET)
    EXT_LOGD(MB_TAG, "Try to configure ethernet");
    EthernetSettingsService* ess = _sveltekit->getEthernetSettingsService();
    #ifdef CONFIG_IDF_TARGET_ESP32S3
    ess->v_ETH_SPI_SCK = UINT8_MAX;
    ess->v_ETH_SPI_MISO = UINT8_MAX;
    ess->v_ETH_SPI_MOSI = UINT8_MAX;
    ess->v_ETH_PHY_CS = UINT8_MAX;
    ess->v_ETH_PHY_IRQ = UINT8_MAX;
    // if ethernet pins change
    // find the pins needed
    for (JsonObject pinObject : _state.data["pins"].as<JsonArray>()) {
      uint8_t usage = pinObject["usage"];
      uint8_t gpio = pinObject["GPIO"];
      if (usage == pin_SPI_SCK) ess->v_ETH_SPI_SCK = gpio;
      if (usage == pin_SPI_MISO) ess->v_ETH_SPI_MISO = gpio;
      if (usage == pin_SPI_MOSI) ess->v_ETH_SPI_MOSI = gpio;
      if (usage == pin_PHY_CS) ess->v_ETH_PHY_CS = gpio;
      if (usage == pin_PHY_IRQ) ess->v_ETH_PHY_IRQ = gpio;
    }

    // allocate the pins found
    if (ess->v_ETH_SPI_SCK != UINT8_MAX && ess->v_ETH_SPI_MISO != UINT8_MAX && ess->v_ETH_SPI_MOSI != UINT8_MAX && ess->v_ETH_PHY_CS != UINT8_MAX && ess->v_ETH_PHY_IRQ != UINT8_MAX) {
      // ess->v_ETH_PHY_TYPE = ETH_PHY_W5500;
      // ess->v_ETH_PHY_ADDR = 1;
      ess->v_ETH_PHY_RST = -1;  // not wired
      ess->initEthernet();      // restart ethernet
    }
    #endif
  #endif
  }
};

#endif
#endif
