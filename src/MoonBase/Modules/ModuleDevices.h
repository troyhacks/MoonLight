/**
    @title     MoonBase
    @file      ModuleDevices.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/moonbase/devices/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact us for more information.
**/

#ifndef ModuleDevices_h
#define ModuleDevices_h

#if FT_MOONBASE == 1

  #include "MoonBase/Module.h"
  #include "MoonBase/Utilities.h"

struct UDPMessage {
  uint8_t rommel[6];
  Char<32> name;
};

class ModuleDevices : public Module {
 public:
  WiFiUDP deviceUDP;
  uint16_t deviceUDPPort = 65506;
  bool deviceUDPConnected = false;

  ModuleDevices(PsychicHttpServer* server, ESP32SvelteKit* sveltekit) : Module("devices", server, sveltekit) { MB_LOGV(MB_TAG, "constructor"); }

  void setupDefinition(JsonArray root) override {
    MB_LOGV(MB_TAG, "");
    JsonObject property;  // state.data has one or more properties
    JsonArray details;    // if a property is an array, this is the details of the array
    JsonArray values;     // if a property is a select, this is the values of the select

    property = root.add<JsonObject>();
    property["name"] = "devices";
    property["type"] = "array";
    details = property["n"].to<JsonArray>();
    {
      property = details.add<JsonObject>();
      property["name"] = "name";
      property["type"] = "mdnsName";
      property["ro"] = true;
      property = details.add<JsonObject>();
      property["name"] = "ip";
      property["type"] = "ip";
      property["ro"] = true;
      property = details.add<JsonObject>();
      property["name"] = "time";
      property["type"] = "time";
      property["ro"] = true;
      property = details.add<JsonObject>();
      property["name"] = "mac";
      property["type"] = "text";
      property["ro"] = true;
    }
  }

  void onUpdate(UpdatedItem& updatedItem) override {
    MB_LOGV(MB_TAG, "no handle for %s[%d]%s[%d].%s = %s -> %s", updatedItem.parent[0].c_str(), updatedItem.index[0], updatedItem.parent[1].c_str(), updatedItem.index[1], updatedItem.name.c_str(),
            updatedItem.oldValue.c_str(), updatedItem.value.as<String>().c_str());
  }

  void loop1s() {
    if (!_socket->getConnectedClients()) return;

    if (!deviceUDPConnected) return;

    readUDP();
  }

  void loop10s() {
    if (!_socket->getConnectedClients()) return;

    if (!deviceUDPConnected) {
      deviceUDPConnected = deviceUDP.begin(deviceUDPPort);
      MB_LOGD(ML_TAG, "deviceUDPConnected %d i:%d p:%d", deviceUDPConnected, deviceUDP.remoteIP()[3], deviceUDPPort);
    }

    if (!deviceUDPConnected) return;

    writeUDP();
  }

  void updateDevices(const char* name, IPAddress ip) {
    if (_state.data["devices"].isNull()) _state.data["devices"].to<JsonArray>();

    JsonObject device = JsonObject();

    for (JsonObject dev : _state.data["devices"].as<JsonArray>()) {
      if (dev["ip"] == ip.toString()) {
        device = dev;
        // MB_LOGD(ML_TAG, "updated ...%d %s", ip[3], name);
      }
    }
    if (device.isNull()) {
      device = _state.data["devices"].as<JsonArray>().add<JsonObject>();
      MB_LOGD(ML_TAG, "added ...%d %s", ip[3], name);
      device["ip"] = ip.toString();
    }

    device["name"] = name;           // name can change
    device["time"] = time(nullptr);  // time will change

    if (!_socket->getConnectedClients()) return;

    // sort in vector
    std::vector<JsonObject> v;
    for (JsonObject obj : _state.data["devices"].as<JsonArray>()) v.push_back(obj);
    std::sort(v.begin(), v.end(), [](JsonObject a, JsonObject b) { return a["name"] < b["name"]; });

    // send only the readonly values ...

    // add sorted to devices
    JsonDocument devices;
    devices["devices"].to<JsonArray>();
    for (auto& obj : v) devices["devices"].add(obj);

    JsonObject data = devices.as<JsonObject>();
    _socket->emitEvent(_moduleName, data);
  }

  void readUDP() {
    size_t packetSize = deviceUDP.parsePacket();
    if (packetSize > 0) {
      char buffer[packetSize];
      deviceUDP.read(buffer, packetSize);
      // MB_LOGD(ML_TAG, "UDP packet read from %d: %s (%d)", deviceUDP.remoteIP()[3], buffer+6, packetSize);

      updateDevices(buffer + 6, deviceUDP.remoteIP());
    }
  }

  void writeUDP() {
    if (deviceUDP.beginPacket(IPAddress(255, 255, 255, 255), deviceUDPPort)) {
      UDPMessage message;
      message.name = esp32sveltekit.getWiFiSettingsService()->getHostname().c_str();
      deviceUDP.write((uint8_t*)&message, sizeof(message));
      deviceUDP.endPacket();
      // MB_LOGV(MB_TAG, "UDP packet written (%d)", WiFi.localIP()[3]);

      updateDevices(message.name.c_str(), WiFi.localIP());
    }
  }
};

#endif
#endif