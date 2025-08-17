---
hide:
  - navigation
  - toc
---

# MoonLight

<div class="moonlight-hero">

**Professional ESP32 LED control platform for stunning 2D & 3D light effects**

Drive up to 12,288 LEDs at 120 FPS with web-based control. Built for creators, makers, and lighting professionals.

<div class="moonlight-buttons">
  <a href="gettingstarted/overview/" class="moonlight-btn">🚀 Get Started</a>
  <a href="https://github.com/MoonModules/MoonLight" class="moonlight-btn ">⭐ GitHub</a> <!-- moonlight-btn-secondary -->
  <a href="https://discord.gg/TC8NSUSCdV" class="moonlight-btn">💬 Discord</a> <!-- moonlight-btn-secondary -->
</div>

</div>

<div class="moonlight-stats">
  <div class="moonlight-stat">
    <span class="moonlight-stat-number">12K+</span>
    <div class="moonlight-stat-label">LEDs at 120 FPS</div>
  </div>
  <div class="moonlight-stat">
    <span class="moonlight-stat-number">65K+</span>
    <div class="moonlight-stat-label">Max LEDs Supported</div>
  </div>
  <div class="moonlight-stat">
    <span class="moonlight-stat-number">16</span>
    <div class="moonlight-stat-label">Parallel LED Strips</div>
  </div>
  <div class="moonlight-stat">
    <span class="moonlight-stat-number">3D</span>
    <div class="moonlight-stat-label">Effects Engine</div>
  </div>
</div>

---

## What is MoonLight?

MoonLight is software which runs on [ESP32 microcontrollers](https://www.espressif.com/en/products/socs/esp32) providing a web-browser interface to control lights in particular and any IOT service in general. ESP32 devices can be connected to your local network via WiFi or Ethernet, MoonLight can be controlled via a web browser selecting the IP-address or network name of the device.

## 🚀 High Performance LED Control

- **12,288 LEDs at 120 FPS** - Maximum performance for demanding applications
- **65,000+ LEDs** - Scale to massive installations at lower framerates  
- **16 Parallel Strips** - Drive multiple LED strips simultaneously
- **120 Virtual Strips** - Advanced mapping and control capabilities

## 🌐 Web-Based Interface

MoonLight can be installed on ESP32 by means of a web installer or if you are a developer, via Visual Studio Code with the PlatformIO plugin installed.

- Intuitive browser-based control panel built with **SvelteKit**
- Control from any device on your network
- No apps required - works in any modern browser
- Real-time updates and responsive design

## 🎨 Advanced Effects Engine

- **2D & 3D Effects** - Professional-grade visual effects
- **Node-based System** - Flexible effect creation and modification  
- **Virtual & Physical Layers** - Advanced mapping capabilities
- **WLED Compatibility** - Audio sync and ecosystem integration

## 🔧 Complete IoT Platform

MoonLight's core functionality is called **MoonBase**, Lighting specific functionality is called **MoonLight**. MoonLight is a standalone product for end users controlling lights but can also be used to create custom IoT services:

- Create GitHub forks for custom applications
- Build anything from smart plugs to greenhouse management
- Utilize ESP32 GPIO ports and network protocols
- Extensible architecture for developers

## 📊 Technical Specifications

| Feature | Specification |
|---------|---------------|
| **Max LEDs (High FPS)** | 12,288 @ 120 FPS |
| **Max LEDs (Standard)** | 65,000+ @ lower FPS |
| **Parallel Strips** | Up to 16 strips |
| **Virtual Strips** | Up to 120 strips |
| **Protocols** | WiFi, Ethernet, Art-Net, DMX |
| **Hardware** | ESP32 microcontrollers |
| **Interface** | SvelteKit web application |

## 🌟 Key Features

### MoonBase Functionality
- Monitor and manage devices
- WiFi management  
- Firmware updates
- System status and restart
- File manager
- Device manager

### MoonLight Features  
- Multiple light control (LED strips, panels, DMX)
- Art-Net protocol support
- Effects on virtual and physical layers
- Node-based effect system
- Real-time performance monitoring

## 🚀 Quick Start

1. **Install via Web Installer** - Flash your ESP32 device easily
2. **Connect to Network** - Configure WiFi or Ethernet  
3. **Access Interface** - Open browser to device IP address
4. **Start Creating** - Design and deploy your LED effects

!!! info "Current Release"
    Release v0.5.8 is a developer release. MoonLight 0.6.0 for end users expected **September 2025** with presets controller, user-friendly installer, and enhanced features.

## 📈 Project Status

- **Latest Release**: [v0.5.8](https://github.com/MoonModules/MoonLight/releases) (Developer Preview)
- **Progress Tracking**: [Kanban Board](https://github.com/users/MoonModules/projects/2/)
- **Documentation**: [Complete Docs](https://moonmodules.org/MoonLight/)
- **Evolution**: [From StarMod to MoonLight](https://moonmodules.org/Star-Mod-Base-Light-Moon-Svelte-Live)

## 🤝 Community & Support

MoonLight is a [MoonModules.org](https://moonmodules.org) project by lighting enthusiasts who also created WLED MM and contribute to WLED.

- **GitHub**: [Star the Repository](https://github.com/MoonModules/MoonLight) ⭐
- **Discord**: [Join MoonLight Community](https://discord.gg/TC8NSUSCdV) 💬  
- **Reddit**: [r/moonmodules](https://reddit.com/r/moonmodules) 🗨️
- **Discussions**: [GitHub Discussions](https://github.com/MoonModules/MoonLight/discussions)

## 👨‍💻 For Developers

We need developers to help with:

- Creating new layouts, effects, and modifiers
- CI/CD automations
- MIDI controller integration  
- Web installer improvements
- UI/UX enhancements
- Documentation and tutorials

Contact us on [Discord](https://discord.com/channels/700041398778331156/1203994211301728296) or [Reddit](https://reddit.com/r/moonmodules) if you want to contribute!

## ❤️ Support the Project

If you appreciate MoonLight, consider supporting development:

- ⭐ [Star the repository](https://github.com/MoonModules/MoonLight)
- 💰 [GitHub Sponsors](https://github.com/sponsors/ewowi)  
- 💵 [PayPal Donation](https://www.paypal.com/donate?business=moonmodules@icloud.com)

## 📄 License & Acknowledgments

**License**: GPL-v3

MoonLight builds upon these excellent projects:
- [ESP32-sveltekit](https://github.com/theelims/ESP32-sveltekit) - Base framework
- [PsychicHttp](https://github.com/hoeken/PsychicHttp) - Web server library
- [FastLED](https://github.com/FastLED/FastLED) - LED driver library
- [ArduinoJson](https://github.com/bblanchon/ArduinoJson) - JSON library
- [ESPLiveScript](https://github.com/hpwit/ESPLiveScript) - Scripts
- [I2SClocklessLedDriver](https://github.com/hpwit/I2SClocklessLedDriver): Physical driver
- [I2SClocklessVirtualLedDriver](https://github.com/hpwit/I2SClocklessVirtualLedDriver): Virtual Driver
- [WLED-sync](https://github.com/netmindz/WLED-sync): Audio

---

**⚠️ Disclaimer**: Using this software is at your own risk. Contributors are not responsible for any issues including but not limited to spontaneous combustion of LED strips, houses, or the heat death of the universe! 🔥🌌