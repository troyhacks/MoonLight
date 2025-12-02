/**
    @title     MoonLight
    @file      AudioSync.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/moonlight/overview/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact us for more information.
**/

#if FT_MOONLIGHT

  #include <WiFiUdp.h>

class ArtNetInDriver : public Node {
 public:
  static const char* name() { return "Art-Net In"; }
  static uint8_t dim() { return _NoD; }
  static const char* tags() { return "☸️"; }

  // Protocol Configuration
  // uint16_t ARTNET_PORT = 6454;
  // uint16_t DDP_PORT = 4048;
  WiFiUDP artnetUdp;
  WiFiUDP ddpUdp;
  uint8_t packetBuffer[1500];

  bool ddp = false;
  uint8_t view = 0;
  uint16_t port = 6454;

  void setup() override {
    addControl(ddp, "DDP", "checkbox");
    addControl(port, "port", "number", 0, 65538);
    JsonObject property;
    JsonArray values;
    property = addControl(view, "view", "select");
    values = property["values"].to<JsonArray>();
    values.add("Physical layer");
    uint8_t i = 0;
    for (VirtualLayer* layer : layerP.layers) {
      Char<32> layerName;
      layerName.format("Layer %d", i);
      values.add(layerName.c_str());
      i++;
    }
  }

  void onUpdate(const Char<20>& oldValue, const JsonObject& control) override {
    // add your custom onUpdate code here
    if (control["name"] == "ddp") {
      if (control["value"] == 0) {
      }
    }
  }

  bool init = false;

  void loop() override {
    if (!WiFi.localIP() && !ETH.localIP()) return;
    if (!init) {
      if (ddp)
        ddpUdp.begin(port);
      else
        artnetUdp.begin(port);
      EXT_LOGD(ML_TAG, "Listening for %s on port %d", ddp ? "DDP" : "Art-Net", port);
      init = true;
    }
    if (ddp)
      handleDDP();
    else
      handleArtNet();
  }

  // Art-Net Configuration
  // uint16_t artnetUniverse = 0;

  // Art-Net packet structure
  struct ArtNetHeader {
    char id[8];        // "Art-Net\0"
    uint16_t opcode;   // 0x5000 for DMX data
    uint16_t version;  // Protocol version
    uint8_t sequence;
    uint8_t physical;
    uint16_t universe;
    uint16_t length;  // DMX data length
  };

  // DDP packet structure
  struct DDPHeader {
    uint8_t flags;     // Bit 7: Push, Bit 6: Query, Bits 5-4: Timecode, Bits 3-0: reserved
    uint8_t sequence;  // Rolling sequence number
    uint8_t dataType;  // 0x01 = RGB data
    uint8_t id;        // Destination ID (0 = all)
    uint32_t offset;   // Byte offset into LED array
    uint16_t dataLen;  // Length of data in bytes
  };

  void handleArtNet() {
    // LightsHeader* header = &layer->layerP->lights.header;

    int packetSize = artnetUdp.parsePacket();

    if (packetSize >= sizeof(ArtNetHeader)) {
      artnetUdp.read(packetBuffer, packetSize);

      // Verify Art-Net packet
      if (memcmp(packetBuffer, "Art-Net", 7) == 0) {
        ArtNetHeader* header = (ArtNetHeader*)packetBuffer;
        uint16_t opcode = header->opcode;
        //
        // EXT_LOGD(ML_TAG, "%d", header->universe);

        // Check if it's a DMX packet (opcode 0x5000)
        if (opcode == 0x5000) {
          uint16_t universe = header->universe;
          uint16_t dataLength = (header->length >> 8) | (header->length << 8);  // Swap bytes

          // Process if it's our universe

          // if (universe == artnetUniverse) { // all universes welcome
            uint8_t* dmxData = packetBuffer + sizeof(ArtNetHeader);

            // Map DMX channels to LEDs (3 channels per LED: RGB)
            int numPixels = min((uint16_t)(dataLength / 3), (uint16_t)(layer->layerP->lights.header.nrOfLights));

            for (int i = 0; i < numPixels; i++) {
              memcpy(&layer->layerP->lights.channels[i * layer->layerP->lights.header.channelsPerLight], &dmxData[i * layer->layerP->lights.header.channelsPerLight], layer->layerP->lights.header.channelsPerLight);
            }

            // FastLED.show();
            // Serial.println("Art-Net: " + String(numPixels) + " pixels updated");
          // }
        }
      }
    }
  }

  void handleDDP() {
    int packetSize = ddpUdp.parsePacket();

    if (packetSize >= sizeof(DDPHeader)) {
      ddpUdp.read(packetBuffer, packetSize);

      DDPHeader* header = (DDPHeader*)packetBuffer;

      // Extract header fields
      bool pushFlag = (header->flags & 0x80) != 0;  // Bit 7
      uint8_t dataType = header->dataType;

      // Convert big-endian offset and length
      uint32_t offset = (header->offset >> 24) | ((header->offset >> 8) & 0xFF00) | ((header->offset << 8) & 0xFF0000) | (header->offset << 24);

      uint16_t dataLen = (header->dataLen >> 8) | (header->dataLen << 8);

      // Validate data type (0x01 = RGB)
      if (dataType == 0x01) {
        uint8_t* pixelData = packetBuffer + sizeof(DDPHeader);

        // Calculate starting pixel from byte offset (3 bytes per pixel)
        int startPixel = offset / 3;
        int numPixels = min((uint16_t)(dataLen / 3), (uint16_t)(layer->layerP->lights.header.nrOfLights - startPixel));

        // Update LEDs
        for (int i = 0; i < numPixels; i++) {
          int ledIndex = startPixel + i;
          if (ledIndex < layer->layerP->lights.header.nrOfLights) {
            memcpy(&layer->layerP->lights.channels[i * layer->layerP->lights.header.channelsPerLight], &pixelData[i * layer->layerP->lights.header.channelsPerLight], layer->layerP->lights.header.channelsPerLight);
          }
        }

        // Only update display if push flag is set
        if (pushFlag) {
          // FastLED.show();
          // Serial.println("DDP: " + String(numPixels) + " pixels updated (offset: " + String(startPixel) + ")");
        }
      }
    }
  }
};

#endif

// #include <WiFi.h>
